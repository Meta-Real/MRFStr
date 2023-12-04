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
#define mrtstr_memchr_load _mm512_stream_load_si512
#define mrtstr_memchr_cmp _mm512_cmpeq_epi8_mask

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrtstr_memchr_simd_t;
#define MRTSTR_MEMCHR_SIMD_SIZE 32
#define MRTSTR_MEMCHR_SIMD_SHIFT 5

#define mrtstr_memchr_set(x, y) x = _mm256_set1_epi8(y)
#define mrtstr_memchr_load _mm256_stream_load_si256
#define mrtstr_memchr_cmp(x, y) _mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y))

#elif defined(__SSE2__)

typedef __m128i mrtstr_memchr_simd_t;
#define MRTSTR_MEMCHR_SIMD_SIZE 16
#define MRTSTR_MEMCHR_SIMD_SHIFT 4

#define mrtstr_memchr_set(x, y) x = _mm_set1_epi8(y)
#define mrtstr_memchr_load _mm_stream_load_si128
#define mrtstr_memchr_cmp(x, y) _mm_movemask_epi8(_mm_cmpeq_epi8(x, y))

#else
#define MRTSTR_MEMCHR_NOSIMD

typedef unsigned long long mrtstr_memchr_simd_t;
#define MRTSTR_MEMCHR_SIMD_SIZE 8
#define MRTSTR_MEMCHR_SIMD_SHIFT 3

#define mrtstr_memchr_set(x, y)                                    \
    do                                                             \
    {                                                              \
        x = y;                                                     \
        for (mrtstr_bit_t i = 1; i < MRTSTR_MEMCHR_SIMD_SIZE; i++) \
            x = x << 8 | y;                                        \
    } while (0)

#define mrtstr_memchr_load(x) *x
#define mrtstr_memchr_cmp(x, y) \
    ((x ^ y) - 0x01010101010101010ULL) & ~(x ^ y) & 0x8080808080808080LL

#endif

#define MRTSTR_MEMCHR_SIMD_MASK (MRTSTR_MEMCHR_SIMD_SIZE - 1)

#define MRTSTR_MEMCHR_TCHK (MRTSTR_MEMCHR_SIMD_SIZE * MRTSTR_THREAD_COUNT)
#define MRTSTR_MEMCHR_TLIMIT (65536 * MRTSTR_MEMCHR_TCHK - 1)

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
    mrtstr_memchr_simd_t block;
    mrtstr_memchr_set(block, chr);

    if (size <= MRTSTR_MEMCHR_TLIMIT)
    {
        mrtstr_size_t rem = size & MRTSTR_MEMCHR_SIMD_MASK;
        size >>= MRTSTR_MEMCHR_SIMD_SHIFT;

        mrtstr_memchr_simd_t tblock;
        for (; size; sblock++, size--)
        {
            tblock = mrtstr_memchr_load(sblock);
            if (mrtstr_memchr_cmp(tblock, block))
            {
                res->res = MRTSTR_TRUE;
                return;
            }
        }

        res->res = memchr(sblock, chr, rem) != NULL;
        return;
    }

    res->res = MRTSTR_FALSE;

    mrtstr_size_t rem = size % MRTSTR_MEMCHR_TCHK;
    size /= MRTSTR_MEMCHR_TCHK;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_memchr_simd_t tblock;
    mrtstr_memchr_t data;
    if (mrtstr_locked(str) && str->forced)
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            for (; str->lock[i];);

            data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCHR_T));
            data->chr = block;
            data->size = size;
            data->lock = str->lock + i;
            data->mutex = &str->mutex;
            data->res = res;

            while (!mrtstr_threads.free_threads && data->size > 65536)
            {
                for (j = 0; j < 65536; sblock++, j++)
                {
                    tblock = mrtstr_memchr_load(sblock);
                    if (mrtstr_memchr_cmp(tblock, block))
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        return;
                    }
                }

                data->size -= 65536;
            }

            if (data->size <= 65536)
            {
                for (; data->size; sblock++, data->size--)
                {
                    tblock = mrtstr_memchr_load(sblock);
                    if (mrtstr_memchr_cmp(tblock, block))
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        return;
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
            data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCHR_T));
            data->chr = block;
            data->size = size;
            data->lock = str->lock + i;
            data->mutex = &str->mutex;
            data->res = res;

            while (!mrtstr_threads.free_threads && data->size > 65536)
            {
                for (j = 0; j < 65536; sblock++, j++)
                {
                    tblock = mrtstr_memchr_load(sblock);
                    if (mrtstr_memchr_cmp(tblock, block))
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        return;
                    }
                }

                data->size -= 65536;
            }

            if (data->size <= 65536)
            {
                for (; data->size; sblock++, data->size--)
                {
                    tblock = mrtstr_memchr_load(sblock);
                    if (mrtstr_memchr_cmp(tblock, block))
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        return;
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

    mrtstr_data_t rres = memchr(sblock, chr, rem);
    if (!rres)
        res->res = MRTSTR_FALSE;

    str->forced = MRTSTR_FALSE;
}

void mrtstr_memchr_threaded(void *args)
{
    mrtstr_memchr_t data = (mrtstr_memchr_t)args;

    mrtstr_size_t i;
    mrtstr_memchr_simd_t block;
    while (data->size > 65536)
    {
        if (data->res->res)
        {
            mrtstr_lock_dec(*data->lock, data->mutex);
            mrtstr_lock_dec(data->res->lock, &data->res->mutex);

            mrstr_free(data);
            return;
        }

        for (i = 0; i < 65536; data->str++, i++)
        {
            block = mrtstr_memchr_load(data->str);
            if (mrtstr_memchr_cmp(block, data->chr))
            {
                data->res->res = MRTSTR_TRUE;

                mrtstr_lock_dec(*data->lock, data->mutex);
                mrtstr_lock_dec(data->res->lock, &data->res->mutex);

                mrstr_free(data);
                return;
            }
        }

        data->size -= 65536;
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
        if (mrtstr_memchr_cmp(block, data->chr))
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
