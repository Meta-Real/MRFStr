/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifdef __AVX512F__

typedef __m512i mrtstr_memchr_simd_t;
#define MRTSTR_MEMCHR_SIMD_SIZE 64
#define MRTSTR_MEMCHR_SIMD_SHIFT 6

#define mrtstr_memchr_set(x, y) x = _mm512_set1_epi8(y)
#define mrtstr_memchr_load _mm512_load_si512
#define mrtstr_memchr_cmp _mm512_cmpeq_epi8_mask

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrtstr_memchr_simd_t;
#define MRTSTR_MEMCHR_SIMD_SIZE 32
#define MRTSTR_MEMCHR_SIMD_SHIFT 5

#define mrtstr_memchr_set(x, y) x = _mm256_set1_epi8(y)
#define mrtstr_memchr_load _mm256_load_si256
#define mrtstr_memchr_cmp(x, y) _mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y))

#elif defined(__SSE2__)

typedef __m128i mrtstr_memchr_simd_t;
#define MRTSTR_MEMCHR_SIMD_SIZE 16
#define MRTSTR_MEMCHR_SIMD_SHIFT 4

#define mrtstr_memchr_set(x, y) x = _mm_set1_epi8(y)
#define mrtstr_memchr_load _mm_load_si128
#define mrtstr_memchr_cmp(x, y) _mm_movemask_epi8(_mm_cmpeq_epi8(x, y))

#else
#define MRTSTR_MEMCHR_NOSIMD

typedef unsigned long long mrtstr_memchr_simd_t;
#define MRTSTR_MEMCHR_SIMD_SIZE 8
#define MRTSTR_MEMCHR_SIMD_SHIFT 3

#define mrtstr_memchr_set(x, y) \
    do                          \
    {                           \
        x = y | (y << 8);       \
        x |= (x << 16);         \
        x |= (x << 32);         \
    } while (0)

#define mrtstr_memchr_load(x) *x
#define mrtstr_memchr_cmp(r, x, y)                                    \
    do                                                                \
    {                                                                 \
        x ^= y;                                                       \
        r = (x - 0x1010101010101010ULL) & ~x & 0x8080808080808080ULL; \
    } while (0)

#endif

#define MRTSTR_MEMCMP_FACTOR (MRTSTR_SIMD_SIZE / MRTSTR_MEMCHR_SIMD_SIZE)
#define MRTSTR_MEMCHR_SIMD_MASK (MRTSTR_MEMCHR_SIMD_SIZE - 1)

#define mrtstr_memchr_init                                    \
    do                                                        \
    {                                                         \
        data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCHR_T)); \
        if (!data)                                            \
        {                                                     \
            if (!i)                                           \
                goto single;                                  \
            goto rem;                                         \
        }                                                     \
                                                              \
        data->chr = cblock;                                   \
        data->size = size;                                    \
        data->lock = str->lock + i;                           \
        data->mutex = &str->mutex;                            \
        data->res = res;                                      \
    } while (0)

struct __MRTSTR_MEMCHR_T
{
    mrtstr_memchr_simd_t *str;
    mrtstr_memchr_simd_t chr;
    mrtstr_size_t size;

    mrtstr_lock_t *lock;
    pthread_mutex_t *mutex;

    mrtstr_bres_t *res;
};
typedef struct __MRTSTR_MEMCHR_T *mrtstr_memchr_t;

void mrtstr_memchr_threaded(void *args);

void mrtstr_memchr(mrtstr_bres_t *res, mrtstr_ct str, mrtstr_chr_t chr, mrtstr_size_t size)
{
    mrtstr_memchr_simd_t *sblock = (mrtstr_memchr_simd_t*)str->data;
    mrtstr_memchr_simd_t cblock;
    mrtstr_memchr_set(cblock, chr);

    mrtstr_size_t rem;
    if (size <= MRTSTR_SIMD_TLIMIT)
    {
single:
        rem = size & MRTSTR_MEMCHR_SIMD_MASK;
        size >>= MRTSTR_MEMCHR_SIMD_SHIFT;

        mrtstr_memchr_simd_t block;
#ifdef MRTSTR_MEMCHR_NOSIMD
        mrtstr_bool_t rres;
#endif
        for (; size; sblock++, size--)
        {
            block = mrtstr_memchr_load(sblock);

#ifdef MRTSTR_MEMCHR_NOSIMD
            mrtstr_memchr_cmp(rres, block, cblock);
            if (rres)
#else
            if (mrtstr_memchr_cmp(block, cblock))
#endif
            {
                res->res = MRTSTR_TRUE;
                return;
            }
        }

        res->res = memchr(sblock, chr, rem) != NULL;
        return;
    }

    res->res = MRTSTR_FALSE;

    rem = size % MRTSTR_SIMD_TCHK;
    size = size / MRTSTR_SIMD_TCHK * MRTSTR_MEMCMP_FACTOR;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_memchr_simd_t block;
#ifdef MRTSTR_MEMCHR_NOSIMD
    mrtstr_bool_t rres;
#endif
    mrtstr_memchr_t data;
    if (mrtstr_locked(str) && str->forced)
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            for (; str->lock[i];);
            mrtstr_memchr_init;

            while (!mrtstr_threads.free_threads && data->size > 0x10000)
            {
                for (j = 0; j < 0x10000; sblock++, j++)
                {
                    block = mrtstr_memchr_load(sblock);

#ifdef MRTSTR_MEMCHR_NOSIMD
                    mrtstr_memchr_cmp(rres, block, cblock);
                    if (rres)
#else
                    if (mrtstr_memchr_cmp(block, cblock))
#endif
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        break;
                    }
                }

                data->size -= 0x10000;
            }

            if (data->size <= 0x10000)
            {
                for (; data->size; sblock++, data->size--)
                {
                    block = mrtstr_memchr_load(sblock);

#ifdef MRTSTR_MEMCHR_NOSIMD
                    mrtstr_memchr_cmp(rres, block, cblock);
                    if (rres)
#else
                    if (mrtstr_memchr_cmp(block, cblock))
#endif
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        break;
                    }
                }

                mrstr_free(data);
            }
            else
            {
                data->str = sblock;
                sblock += data->size;

                str->lock[i] = 1;
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_memchr_threaded, data);
            }
        }
    else
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            mrtstr_memchr_init;

            while (!mrtstr_threads.free_threads && data->size > 0x10000)
            {
                for (j = 0; j < 0x10000; sblock++, j++)
                {
                    block = mrtstr_memchr_load(sblock);

#ifdef MRTSTR_MEMCHR_NOSIMD
                    mrtstr_memchr_cmp(rres, block, cblock);
                    if (rres)
#else
                    if (mrtstr_memchr_cmp(block, cblock))
#endif
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        break;
                    }
                }

                data->size -= 0x10000;
            }

            if (data->size <= 0x10000)
            {
                for (; data->size; sblock++, data->size--)
                {
                    block = mrtstr_memchr_load(sblock);

#ifdef MRTSTR_MEMCHR_NOSIMD
                    mrtstr_memchr_cmp(rres, block, cblock);
                    if (rres)
#else
                    if (mrtstr_memchr_cmp(block, cblock))
#endif
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        break;
                    }
                }

                mrstr_free(data);
            }
            else
            {
                data->str = sblock;
                sblock += data->size;

                mrtstr_lock_inc(str->lock[i], &str->mutex);
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_memchr_threaded, data);
            }
        }

ret:
    if (!res->res && memchr(sblock, chr, rem))
        res->res = MRTSTR_TRUE;

    str->forced = MRTSTR_FALSE;
    return;

rem:
    size *= MRTSTR_THREAD_COUNT - i;

    while (size >= 0x10000)
    {
        if (res->res)
            goto ret;

        for (j = 0; j < 0x10000; sblock++, j++)
        {
            block = mrtstr_memchr_load(sblock);

#ifdef MRTSTR_MEMCHR_NOSIMD
            mrtstr_memchr_cmp(rres, block, cblock);
            if (rres)
#else
            if (mrtstr_memchr_cmp(block, cblock))
#endif
            {
                res->res = MRTSTR_TRUE;
                goto ret;
            }
        }

        size -= 0x10000;
    }

    if (res->res)
        goto ret;

    for (; size; sblock++, size--)
    {
        block = mrtstr_memchr_load(sblock);

#ifdef MRTSTR_MEMCHR_NOSIMD
        mrtstr_memchr_cmp(rres, block, cblock);
        if (rres)
#else
        if (mrtstr_memchr_cmp(block, cblock))
#endif
        {
            res->res = MRTSTR_TRUE;
            goto ret;
        }
    }

    if (memchr(sblock, chr, rem))
        res->res = MRTSTR_TRUE;

    goto ret;
}

void mrtstr_memchr_threaded(void *args)
{
    mrtstr_memchr_t data = (mrtstr_memchr_t)args;

    mrtstr_size_t i;
    mrtstr_memchr_simd_t block;
#ifdef MRTSTR_MEMCHR_NOSIMD
    mrtstr_bool_t rres;
#endif
    while (data->size > 0x10000)
    {
        if (data->res->res)
        {
            mrtstr_lock_dec(*data->lock, data->mutex);
            mrtstr_lock_dec(data->res->lock, &data->res->mutex);

            mrstr_free(data);
            return;
        }

        for (i = 0; i < 0x10000; data->str++, i++)
        {
            block = mrtstr_memchr_load(data->str);

#ifdef MRTSTR_MEMCHR_NOSIMD
            mrtstr_memchr_cmp(rres, block, data->chr);
            if (rres)
#else
            if (mrtstr_memchr_cmp(block, data->chr))
#endif
            {
                data->res->res = MRTSTR_TRUE;

                mrtstr_lock_dec(*data->lock, data->mutex);
                mrtstr_lock_dec(data->res->lock, &data->res->mutex);

                mrstr_free(data);
                return;
            }
        }

        data->size -= 0x10000;
    }

    if (data->res->res)
    {
        mrtstr_lock_dec(*data->lock, data->mutex);
        mrtstr_lock_dec(data->res->lock, &data->res->mutex);

        mrstr_free(data);
        return;
    }

    for (; data->size; data->str++, data->size--)
    {
        block = mrtstr_memchr_load(data->str);

#ifdef MRTSTR_MEMCHR_NOSIMD
        mrtstr_memchr_cmp(rres, block, data->chr);
        if (rres)
#else
        if (mrtstr_memchr_cmp(block, data->chr))
#endif
        {
            data->res->res = MRTSTR_TRUE;

            mrtstr_lock_dec(*data->lock, data->mutex);
            mrtstr_lock_dec(data->res->lock, &data->res->mutex);

            mrstr_free(data);
            return;
        }
    }

    mrtstr_lock_dec(*data->lock, data->mutex);
    mrtstr_lock_dec(data->res->lock, &data->res->mutex);

    mrstr_free(data);
}
