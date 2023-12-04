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

#define mrtstr_memcmp_load _mm512_stream_load_si512
#define mrtstr_memcmp_loadu _mm512_loadu_si512
#define mrtstr_memcmp_cmp _mm512_cmpneq_epi64_mask

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrtstr_memcmp_simd_t;
#define MRTSTR_MEMCMP_SIMD_SIZE 32
#define MRTSTR_MEMCMP_SIMD_SHIFT 5

#define mrtstr_memcmp_load _mm256_stream_load_si256
#define mrtstr_memcmp_loadu _mm256_loadu_si256
#define mrtstr_memcmp_cmp(x, y) (~_mm256_movemask_epi8(_mm256_cmpeq_epi64(x, y)))

#elif defined(__SSE2__)

typedef __m128i mrtstr_memcmp_simd_t;
#define MRTSTR_MEMCMP_SIMD_SIZE 16
#define MRTSTR_MEMCMP_SIMD_SHIFT 4

#define mrtstr_memcmp_load _mm_stream_load_si128
#define mrtstr_memcmp_loadu _mm_loadu_si128

#ifdef __SSE4_1__
#define mrtstr_memcmp_cmp(x, y) (~_mm_movemask_epi8(_mm_cmpeq_epi64(x, y)))
#else
#define mrtstr_memcmp_cmp(x, y) (~_mm_movemask_epi8(_mm_cmpeq_epi32(x, y)))
#endif

#else
#define MRTSTR_MEMCMP_NOSIMD

typedef unsigned long long mrtstr_memcmp_simd_t;
#define MRTSTR_MEMCMP_SIMD_SIZE 8

#define mrtstr_memcmp_load(x) *x
#define mrtstr_memcmp_loadu mrtstr_memcmp_load
#define mrtstr_memcmp_cmp(x, y) x != y

#endif

#define MRTSTR_MEMCMP_SIMD_MASK (MRTSTR_MEMCMP_SIMD_SIZE - 1)
#define MRTSTR_MEMCMP_TCHK (MRTSTR_MEMCMP_SIMD_SIZE * MRTSTR_THREAD_COUNT)
#define MRTSTR_MEMCMP_TLIMIT (65536 * MRTSTR_MEMCMP_TCHK - 1)

#define mrtstr_memcmp_sub(x, y, s)                 \
    do                                             \
    {                                              \
        for (; s; x++, y++, s--)                   \
        {                                          \
            block1 = mrtstr_memcmp_load(x);        \
            block2 = mrtstr_memcmp_load(y);        \
            if (mrtstr_memcmp_cmp(block1, block2)) \
            {                                      \
                res->res = MRTSTR_FALSE;           \
                                                   \
                mrstr_free(data);                  \
                return;                            \
            }                                      \
        }                                          \
    } while (0)

#define mrtstr_init_memcmp                                    \
    do                                                        \
    {                                                         \
        data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCMP_T)); \
        data->size = size;                                    \
        data->s1lock = str1->lock + i;                        \
        data->s1mutex = &str1->mutex;                         \
        data->s2lock = str2->lock + i;                        \
        data->s2mutex = &str2->mutex;                         \
        data->res = res;                                      \
    } while (0)

#define mrtstr_init_memcmp2                                    \
    do                                                         \
    {                                                          \
        data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCMP2_T)); \
        data->size = size;                                     \
        data->s1lock = str1->lock + i;                         \
        data->s1mutex = &str1->mutex;                          \
        data->res = res;                                       \
    } while (0)

#define mrtstr_wait_process                                        \
    do                                                             \
    {                                                              \
        while (!mrtstr_threads.free_threads && data->size > 65536) \
        {                                                          \
            for (j = 0; j < 65536; s1block++, s2block++, j++)      \
            {                                                      \
                block1 = mrtstr_memcmp_load(s1block);              \
                block2 = mrtstr_memcmp_load(s2block);              \
                if (mrtstr_memcmp_cmp(block1, block2))             \
                {                                                  \
                    res->res = MRTSTR_FALSE;                       \
                                                                   \
                    mrstr_free(data);                              \
                    return;                                        \
                }                                                  \
            }                                                      \
                                                                   \
            data->size -= 65536;                                   \
        }                                                          \
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

    if (size <= MRTSTR_MEMCMP_TLIMIT)
    {
#ifdef MRTSTR_MEMCMP_NOSIMD
        res->res = !memcmp(s1block, s2block, size);
#else
        mrtstr_size_t rem = size & MRTSTR_MEMCMP_SIMD_MASK;
        size >>= MRTSTR_MEMCMP_SIMD_SHIFT;

        mrtstr_memcmp_simd_t block1, block2;
        for (; size; s1block++, s2block++, size--)
        {
            block1 = mrtstr_memcmp_load(s1block);
            block2 = mrtstr_memcmp_load(s2block);
            if (mrtstr_memcmp_cmp(block1, block2))
            {
                res->res = MRTSTR_FALSE;
                return;
            }
        }

        res->res = !memcmp(s1block, s2block, rem);
#endif
        return;
    }

    res->res = MRTSTR_TRUE;

    mrtstr_size_t rem = size % MRTSTR_MEMCMP_TCHK;
    size /= MRTSTR_MEMCMP_TCHK;

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
                mrtstr_init_memcmp;
                mrtstr_wait_process;

                if (data->size <= 65536)
                {
                    mrtstr_memcmp_sub(s1block, s2block, data->size);
                    mrstr_free(data);
                }
                else
                {
                    data->str1 = s1block;
                    data->str2 = s2block;

                    s1block += data->size;
                    s2block += data->size;

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
                mrtstr_init_memcmp;
                mrtstr_wait_process;

                if (data->size <= 65536)
                {
                    mrtstr_memcmp_sub(s1block, s2block, data->size);
                    mrstr_free(data);
                }
                else
                {
                    data->str1 = s1block;
                    data->str2 = s2block;

                    s1block += data->size;
                    s2block += data->size;

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
            mrtstr_init_memcmp;
            mrtstr_wait_process;

            if (data->size <= 65536)
            {
                mrtstr_memcmp_sub(s1block, s2block, data->size);
                mrstr_free(data);
            }
            else
            {
                data->str1 = s1block;
                data->str2 = s2block;

                s1block += data->size;
                s2block += data->size;

                mrtstr_lock_inc(str1->lock[i], &str1->mutex);
                str2->lock[i] = 1;
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_memcmp_threaded, data);
            }
        }
    else
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            mrtstr_init_memcmp;
            mrtstr_wait_process;

            if (data->size <= 65536)
            {
                mrtstr_memcmp_sub(s1block, s2block, data->size);
                mrstr_free(data);
            }
            else
            {
                data->str1 = s1block;
                data->str2 = s2block;

                s1block += data->size;
                s2block += data->size;

                mrtstr_lock_inc(str1->lock[i], &str1->mutex);
                mrtstr_lock_inc(str2->lock[i], &str2->mutex);
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_memcmp_threaded, data);
            }
        }

    mrtstr_bool_t rres = !memcmp((mrtstr_data_ct)s2block, (mrtstr_data_ct)s1block, rem);
    if (!rres)
        res->res = MRTSTR_FALSE;

    str1->forced = MRTSTR_FALSE;
    str2->forced = MRTSTR_FALSE;
}

void mrtstr_memcmp2(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2, mrtstr_size_t size)
{
    mrtstr_memcmp_simd_t *s1block = (mrtstr_memcmp_simd_t*)str1->data;
    mrtstr_memcmp_simd_t *s2block = (mrtstr_memcmp_simd_t*)str2;

    if (size <= MRTSTR_MEMCMP_TLIMIT)
    {
#ifdef MRTSTR_MEMCMP_NOSIMD
        res->res = !memcmp(s1block, s2block, size);
#else
        mrtstr_size_t rem = size & MRTSTR_MEMCMP_SIMD_MASK;
        size >>= MRTSTR_MEMCMP_SIMD_SHIFT;

        mrtstr_memcmp_simd_t block1, block2;
        for (; size; s1block++, s2block++, size--)
        {
            block1 = mrtstr_memcmp_load(s1block);
            block2 = mrtstr_memcmp_loadu(s2block);
            if (mrtstr_memcmp_cmp(block1, block2))
            {
                res->res = MRTSTR_FALSE;
                return;
            }
        }

        res->res = !memcmp(s1block, s2block, rem);
#endif
        return;
    }

    res->res = MRTSTR_TRUE;

    mrtstr_size_t rem = size % MRTSTR_MEMCMP_TCHK;
    size /= MRTSTR_MEMCMP_TCHK;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_memcmp_simd_t block1, block2;
    mrtstr_memcmp2_t data;
    if (mrtstr_locked(str1) && str1->forced)
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            for (; str1->lock[i];);
            mrtstr_init_memcmp2;
            mrtstr_wait_process;

            if (data->size <= 65536)
            {
                mrtstr_memcmp_sub(s1block, s2block, data->size);
                mrstr_free(data);
            }
            else
            {
                data->str1 = s1block;
                data->str2 = s2block;

                s1block += data->size;
                s2block += data->size;

                str1->lock[i] = 1;
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_memcmp2_threaded, data);
            }
        }
    else
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            mrtstr_init_memcmp2;
            mrtstr_wait_process;

            if (data->size <= 65536)
            {
                mrtstr_memcmp_sub(s1block, s2block, data->size);
                mrstr_free(data);
            }
            else
            {
                data->str1 = s1block;
                data->str2 = s2block;

                s1block += data->size;
                s2block += data->size;

                mrtstr_lock_inc(str1->lock[i], &str1->mutex);
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_memcmp2_threaded, data);
            }
        }

    mrtstr_bool_t rres = !memcmp(s2block, s1block, rem);
    if (!rres)
        res->res = MRTSTR_FALSE;

    str1->forced = MRTSTR_FALSE;
}

void mrtstr_memcmp_threaded(void *args)
{
    mrtstr_memcmp_t data = (mrtstr_memcmp_t)args;

    mrtstr_size_t i;
    mrtstr_memcmp_simd_t block1, block2;
    while (data->size > 65536)
    {
        if (!data->res->res)
            mrtstr_memcmp_treturn;

        for (i = 0; i < 65536; data->str1++, data->str2++, i++)
        {
            block1 = mrtstr_memcmp_load(data->str1);
            block2 = mrtstr_memcmp_load(data->str2);
            if (mrtstr_memcmp_cmp(block1, block2))
            {
                data->res->res = MRTSTR_FALSE;
                mrtstr_memcmp_treturn;
            }
        }

        data->size -= 65536;
    }

    if (!data->res->res)
        mrtstr_memcmp_treturn;

    for (; data->size; data->str1++, data->str2++, data->size--)
    {
        block1 = mrtstr_memcmp_load(data->str1);
        block2 = mrtstr_memcmp_load(data->str2);
        if (mrtstr_memcmp_cmp(block1, block2))
        {
            data->res->res = MRTSTR_FALSE;
            mrtstr_memcmp_treturn;
        }
    }

    mrtstr_memcmp_treturn;
}

void mrtstr_memcmp2_threaded(void *args)
{
    mrtstr_memcmp2_t data = (mrtstr_memcmp2_t)args;

    mrtstr_size_t i;
    mrtstr_memcmp_simd_t block1, block2;
    while (data->size > 65536)
    {
        if (!data->res->res)
            mrtstr_memcmp2_treturn;

        for (i = 0; i < 65536; data->str1++, data->str2++, i++)
        {
            block1 = mrtstr_memcmp_load(data->str1);
            block2 = mrtstr_memcmp_load(data->str2);
            if (mrtstr_memcmp_cmp(block1, block2))
            {
                data->res->res = MRTSTR_FALSE;
                mrtstr_memcmp2_treturn;
            }
        }

        data->size -= 65536;
    }

    if (!data->res->res)
        mrtstr_memcmp2_treturn;

    for (; data->size; data->str1++, data->str2++, data->size--)
    {
        block1 = mrtstr_memcmp_load(data->str1);
        block2 = mrtstr_memcmp_load(data->str2);
        if (mrtstr_memcmp_cmp(block1, block2))
        {
            data->res->res = MRTSTR_FALSE;
            mrtstr_memcmp2_treturn;
        }
    }

    mrtstr_memcmp2_treturn;
}
