/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifdef __AVX512F__

typedef __m512i mrtstr_memcmp_simd_t;
#define MRTSTR_MEMCMP_SIMD_SIZE 64
#define MRTSTR_MEMCMP_SIMD_SHIFT 6

#define mrtstr_memcmp_load _mm512_load_si512
#define mrtstr_memcmp_loadu _mm512_loadu_si512
#define mrtstr_memcmp_cmp _mm512_cmpneq_epi64_mask

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrtstr_memcmp_simd_t;
#define MRTSTR_MEMCMP_SIMD_SIZE 32
#define MRTSTR_MEMCMP_SIMD_SHIFT 5

#define mrtstr_memcmp_load _mm256_load_si256
#define mrtstr_memcmp_loadu _mm256_loadu_si256
#define mrtstr_memcmp_cmp(x, y) (~_mm256_movemask_epi8(_mm256_cmpeq_epi64(x, y)))

#elif defined(__SSE2__)

typedef __m128i mrtstr_memcmp_simd_t;
#define MRTSTR_MEMCMP_SIMD_SIZE 16
#define MRTSTR_MEMCMP_SIMD_SHIFT 4

#define mrtstr_memcmp_load _mm_load_si128
#define mrtstr_memcmp_loadu _mm_loadu_si128
#define mrtstr_memcmp_cmp(x, y) (_mm_movemask_epi8(_mm_cmpeq_epi32(x, y)) != 0xffff)

#else
#define MRTSTR_MEMCMP_NOSIMD

typedef unsigned long long mrtstr_memcmp_simd_t;
#define MRTSTR_MEMCMP_SIMD_SIZE 8

#define mrtstr_memcmp_load(x) *x
#define mrtstr_memcmp_loadu mrtstr_memcmp_load
#define mrtstr_memcmp_cmp(x, y) x != y

#endif

#define MRTSTR_MEMCMP_FACTOR (MRTSTR_SIMD_SIZE / MRTSTR_MEMCMP_SIMD_SIZE)

#ifndef MRTSTR_MEMCMP_NOSIMD
#define MRTSTR_MEMCMP_SIMD_MASK (MRTSTR_MEMCMP_SIMD_SIZE - 1)
#endif

#define mrtstr_memcmp_proc(x, y, p, f)         \
    do                                         \
    {                                          \
        block1 = mrtstr_memcmp_load(x);        \
        block2 = mrtstr_memcmp_loadu(y);       \
        if (mrtstr_memcmp_cmp(block1, block2)) \
        {                                      \
            p res->res = MRTSTR_FALSE;         \
            f;                                 \
        }                                      \
    } while (0)

#define mrtstr_memcmp_sub(x, y, p, s, f) \
    for (; s; x++, y++, s--)             \
        mrtstr_memcmp_proc(x, y, p, f)

#define mrtstr_memcmp_waitsub                                    \
    while (!mrtstr_threads.free_threads && data->size > 0x10000) \
    {                                                            \
        for (j = 0; j < 0x10000; s1block++, s2block++, j++)      \
            mrtstr_memcmp_proc(s1block, s2block, ,               \
                mrstr_free(data); break);                        \
                                                                 \
        data->size -= 0x10000;                                   \
    }

#define mrtstr_memcmp_str      \
    do                         \
    {                          \
        data->str1 = s1block;  \
        data->str2 = s2block;  \
                               \
        s1block += data->size; \
        s2block += data->size; \
    } while (0)

#define mrtstr_memcmp_rem                                         \
    do                                                            \
    {                                                             \
        size *= MRTSTR_THREAD_COUNT - i;                          \
                                                                  \
        mrtstr_memcmp_simd_t block1, block2;                      \
        mrtstr_size_t j;                                          \
        while (size >= 0x10000)                                   \
        {                                                         \
            if (!res->res)                                        \
                goto ret;                                         \
                                                                  \
            for (j = 0; j < 0x10000; s1block++, s2block++, j++)   \
                mrtstr_memcmp_proc(s1block, s2block, , goto ret); \
                                                                  \
            size -= 0x10000;                                      \
        }                                                         \
                                                                  \
        if (!res->res)                                            \
            goto ret;                                             \
                                                                  \
        for (; size; s1block++, s2block++, size--)                \
            mrtstr_memcmp_proc(s1block, s2block, , goto ret);     \
                                                                  \
        goto ret;                                                 \
    } while (0)

#define mrtstr_memcmp_init                                    \
    do                                                        \
    {                                                         \
        data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCMP_T)); \
        if (!data)                                            \
        {                                                     \
            if (!i)                                           \
                goto single;                                  \
            goto rem;                                         \
        }                                                     \
                                                              \
        data->size = size;                                    \
        data->s1lock = str1->lock + i;                        \
        data->s1mutex = &str1->mutex;                         \
        data->s2lock = str2->lock + i;                        \
        data->s2mutex = &str2->mutex;                         \
        data->res = res;                                      \
    } while (0)

#define mrtstr_memcmp_treturn                                \
    do                                                       \
    {                                                        \
        mrtstr_lock_dec(*data->s1lock, data->s1mutex);       \
        mrtstr_lock_dec(*data->s2lock, data->s2mutex);       \
        mrtstr_lock_dec(data->res->lock, &data->res->mutex); \
                                                             \
        mrstr_free(data);                                    \
        return;                                              \
    } while (0)

#define mrtstr_memcmp2_init                                    \
    do                                                         \
    {                                                          \
        data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCMP2_T)); \
        if (!data)                                             \
            goto rem;                                          \
                                                               \
        data->size = size;                                     \
        data->s1lock = str1->lock + i;                         \
        data->s1mutex = &str1->mutex;                          \
        data->res = res;                                       \
    } while (0)

#define mrtstr_memcmp2_treturn                               \
    do                                                       \
    {                                                        \
        mrtstr_lock_dec(*data->s1lock, data->s1mutex);       \
        mrtstr_lock_dec(data->res->lock, &data->res->mutex); \
                                                             \
        mrstr_free(data);                                    \
        return;                                              \
    } while (0)

struct __MRTSTR_MEMCMP_T
{
    mrtstr_memcmp_simd_t *str1;
    mrtstr_memcmp_simd_t *str2;
    mrtstr_size_t size;

    mrtstr_lock_t *s1lock;
    pthread_mutex_t *s1mutex;

    mrtstr_lock_t *s2lock;
    pthread_mutex_t *s2mutex;

    mrtstr_bres_t *res;
};
typedef struct __MRTSTR_MEMCMP_T *mrtstr_memcmp_t;

struct __MRTSTR_MEMCMP2_T
{
    mrtstr_memcmp_simd_t *str1;
    mrtstr_memcmp_simd_t *str2;
    mrtstr_size_t size;

    mrtstr_lock_t *s1lock;
    pthread_mutex_t *s1mutex;

    mrtstr_bres_t *res;
};
typedef struct __MRTSTR_MEMCMP2_T *mrtstr_memcmp2_t;

void mrtstr_memcmp_threaded(void *args);
void mrtstr_memcmp2_threaded(void *args);

void mrtstr_memcmp(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_ct str2, mrtstr_size_t size)
{
    mrtstr_memcmp_simd_t *s1block = (mrtstr_memcmp_simd_t*)str1->data;
    mrtstr_memcmp_simd_t *s2block = (mrtstr_memcmp_simd_t*)str2->data;

    if (size <= MRTSTR_SIMD_TLIMIT)
    {
single:
#ifdef MRTSTR_MEMCMP_NOSIMD
        res->res = !memcmp(s1block, s2block, size);
#else
        mrtstr_size_t rem = size & MRTSTR_MEMCMP_SIMD_MASK;
        size >>= MRTSTR_MEMCMP_SIMD_SHIFT;

        mrtstr_memcmp_simd_t block1, block2;
        mrtstr_memcmp_sub(s1block, s2block, , size, return);
        res->res = !memcmp(s1block, s2block, rem);
#endif
        return;
    }

    res->res = MRTSTR_TRUE;

    mrtstr_size_t rem = size % MRTSTR_SIMD_TCHK;
    size = size / MRTSTR_SIMD_TCHK * MRTSTR_MEMCMP_FACTOR;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_memcmp_simd_t block1, block2;
    mrtstr_memcmp_t data;
    if (mrtstr_locked(str1) && str1->forced)
    {
        if (mrtstr_locked(str2) && str2->forced)
            for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
            {
                for (; str1->lock[i];);
                for (; str2->lock[i];);
                mrtstr_memcmp_init;
                mrtstr_memcmp_waitsub;

                if (data->size <= 0x10000)
                {
                    mrtstr_memcmp_sub(s1block, s2block, , data->size,
                        mrstr_free(data); break);
                    mrstr_free(data);
                }
                else
                {
                    mrtstr_memcmp_str;

                    str1->lock[i] = 1;
                    str2->lock[i] = 1;
                    mrtstr_lock_inc(res->lock, &res->mutex);
                    mrtstr_load_threads(mrtstr_memcmp_threaded, data);
                }
            }
        else
            for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
            {
                for (; str1->lock[i];);
                mrtstr_memcmp_init;
                mrtstr_memcmp_waitsub;

                if (data->size <= 0x10000)
                {
                    mrtstr_memcmp_sub(s1block, s2block, , data->size,
                        mrstr_free(data); break);
                    mrstr_free(data);
                }
                else
                {
                    mrtstr_memcmp_str;

                    str1->lock[i] = 1;
                    mrtstr_lock_inc(str2->lock[i], &str2->mutex);
                    mrtstr_lock_inc(res->lock, &res->mutex);
                    mrtstr_load_threads(mrtstr_memcmp_threaded, data);
                }
            }
    }
    else if (mrtstr_locked(str2) && str2->forced)
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            for (; str2->lock[i];);
            mrtstr_memcmp_init;
            mrtstr_memcmp_waitsub;

            if (data->size <= 0x10000)
            {
                mrtstr_memcmp_sub(s1block, s2block, , data->size,
                    mrstr_free(data); break);
                mrstr_free(data);
            }
            else
            {
                mrtstr_memcmp_str;

                mrtstr_lock_inc(str1->lock[i], &str1->mutex);
                str2->lock[i] = 1;
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_memcmp_threaded, data);
            }
        }
    else
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            mrtstr_memcmp_init;
            mrtstr_memcmp_waitsub;

            if (data->size <= 0x10000)
            {
                mrtstr_memcmp_sub(s1block, s2block, , data->size,
                    mrstr_free(data); break);
                mrstr_free(data);
            }
            else
            {
                mrtstr_memcmp_str;

                mrtstr_lock_inc(str1->lock[i], &str1->mutex);
                mrtstr_lock_inc(str2->lock[i], &str2->mutex);
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_memcmp_threaded, data);
            }
        }

ret:
    if (res->res && memcmp(s1block, s2block, rem))
        res->res = MRTSTR_FALSE;

    str1->forced = MRTSTR_FALSE;
    str2->forced = MRTSTR_FALSE;
    return;

rem:
    mrtstr_memcmp_rem;
}

void mrtstr_memcmp2(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2, mrtstr_size_t size)
{
    mrtstr_memcmp_simd_t *s1block = (mrtstr_memcmp_simd_t*)str1->data;
    mrtstr_memcmp_simd_t *s2block = (mrtstr_memcmp_simd_t*)str2;

    if (size <= MRTSTR_SIMD_TLIMIT)
    {
#ifdef MRTSTR_MEMCMP_NOSIMD
        res->res = !memcmp(s1block, s2block, size);
#else
        mrtstr_size_t rem = size & MRTSTR_MEMCMP_SIMD_MASK;
        size >>= MRTSTR_MEMCMP_SIMD_SHIFT;

        mrtstr_memcmp_simd_t block1, block2;
        mrtstr_memcmp_sub(s1block, s2block, , size, return);
        res->res = !memcmp(s1block, s2block, rem);
#endif
        return;
    }

    res->res = MRTSTR_TRUE;

    mrtstr_size_t rem = size % MRTSTR_SIMD_TCHK;
    size = size / MRTSTR_SIMD_TCHK * MRTSTR_MEMCMP_FACTOR;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_memcmp_simd_t block1, block2;
    mrtstr_memcmp2_t data;
    if (mrtstr_locked(str1) && str1->forced)
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            for (; str1->lock[i];);
            mrtstr_memcmp2_init;
            mrtstr_memcmp_waitsub;

            if (data->size <= 0x10000)
            {
                mrtstr_memcmp_sub(s1block, s2block, , data->size,
                    mrstr_free(data); break);
                mrstr_free(data);
            }
            else
            {
                mrtstr_memcmp_str;

                str1->lock[i] = 1;
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_memcmp2_threaded, data);
            }
        }
    else
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            mrtstr_memcmp2_init;
            mrtstr_memcmp_waitsub;

            if (data->size <= 0x10000)
            {
                mrtstr_memcmp_sub(s1block, s2block, , data->size,
                    mrstr_free(data); break);
                mrstr_free(data);
            }
            else
            {
                mrtstr_memcmp_str;

                mrtstr_lock_inc(str1->lock[i], &str1->mutex);
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_memcmp2_threaded, data);
            }
        }

ret:
    if (res->res && memcmp(s1block, s2block, rem))
        res->res = MRTSTR_FALSE;

    str1->forced = MRTSTR_FALSE;
    return;

rem:
    mrtstr_memcmp_rem;
}

void mrtstr_memcmp_threaded(void *args)
{
    mrtstr_memcmp_t data = (mrtstr_memcmp_t)args;

    mrtstr_size_t i;
    mrtstr_memcmp_simd_t block1, block2;
    while (data->size > 0x10000)
    {
        if (!data->res->res)
            mrtstr_memcmp_treturn;

        for (i = 0; i < 0x10000; data->str1++, data->str2++, i++)
            mrtstr_memcmp_proc(data->str1, data->str2, data->,
                mrtstr_memcmp_treturn);

        data->size -= 0x10000;
    }

    if (!data->res->res)
        mrtstr_memcmp_treturn;

    for (; data->size; data->str1++, data->str2++, data->size--)
        mrtstr_memcmp_proc(data->str1, data->str2, data->,
            mrtstr_memcmp_treturn);

    mrtstr_memcmp_treturn;
}

void mrtstr_memcmp2_threaded(void *args)
{
    mrtstr_memcmp2_t data = (mrtstr_memcmp2_t)args;

    mrtstr_size_t i;
    mrtstr_memcmp_simd_t block1, block2;
    while (data->size > 0x10000)
    {
        if (!data->res->res)
            mrtstr_memcmp2_treturn;

        for (i = 0; i < 0x10000; data->str1++, data->str2++, i++)
            mrtstr_memcmp_proc(data->str1, data->str2, data->,
                mrtstr_memcmp2_treturn);

        data->size -= 0x10000;
    }

    if (!data->res->res)
        mrtstr_memcmp2_treturn;

    for (; data->size; data->str1++, data->str2++, data->size--)
        mrtstr_memcmp_proc(data->str1, data->str2, data->,
            mrtstr_memcmp2_treturn);

    mrtstr_memcmp2_treturn;
}
