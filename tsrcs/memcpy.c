/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifdef __AVX512F__

typedef __m512i mrtstr_memcpy_simd_t;
#define MRTSTR_MEMCPY_SIMD_SIZE 64
#define MRTSTR_MEMCPY_SIMD_SHIFT 6

#define mrtstr_memcpy_load _mm512_stream_load_si512
#define mrtstr_memcpy_loadu _mm512_loadu_si512
#define mrtstr_memcpy_store _mm512_stream_si512

#elif defined(__AVX__)

typedef __m256i mrtstr_memcpy_simd_t;
#define MRTSTR_MEMCPY_SIMD_SIZE 32
#define MRTSTR_MEMCPY_SIMD_SHIFT 5

#define mrtstr_memcpy_load _mm256_stream_load_si256
#define mrtstr_memcpy_loadu _mm256_loadu_si256
#define mrtstr_memcpy_store _mm256_stream_si256

#elif defined(__SSE2__)

typedef __m128i mrtstr_memcpy_simd_t;
#define MRTSTR_MEMCPY_SIMD_SIZE 16
#define MRTSTR_MEMCPY_SIMD_SHIFT 4

#define mrtstr_memcpy_load _mm_stream_load_si128
#define mrtstr_memcpy_loadu _mm_loadu_si128
#define mrtstr_memcpy_store _mm_stream_si128

#else
#define MRTSTR_MEMCPY_NOSIMD

typedef unsigned long long mrtstr_memcpy_simd_t;
#define MRTSTR_MEMCPY_SIMD_SIZE 8
#define MRTSTR_MEMCPY_SIMD_SHIFT 3

#define mrtstr_memcpy_load(x) *x
#define mrtstr_memcpy_loadu mrtstr_memcpy_load
#define mrtstr_memcpy_store(x, y) *x = y

#endif

#define MRTSTR_MEMCPY_SIMD_MASK (MRTSTR_MEMCPY_SIMD_SIZE - 1)
#define MRTSTR_MEMCPY_TCHK (MRTSTR_MEMCPY_SIMD_SIZE * MRTSTR_THREAD_COUNT)
#define MRTSTR_MEMCPY_TLIMIT (65536 * MRTSTR_MEMCPY_TCHK - 1)

#define mrtstr_memcpy_sub(x, y, s)         \
    do                                     \
    {                                      \
        for (; s; x++, y++, s--)           \
        {                                  \
            block = mrtstr_memcpy_load(y); \
            mrtstr_memcpy_store(x, block); \
        }                                  \
    } while (0)

#define mrtstr_memcpy2_sub(x, y, s)         \
    do                                      \
    {                                       \
        for (; s; x++, y++, s--)            \
        {                                   \
            block = mrtstr_memcpy_loadu(y); \
            mrtstr_memcpy_store(x, block);  \
        }                                   \
    } while (0)

#define mrtstr_memcpy_data                                    \
    do                                                        \
    {                                                         \
        data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCPY_T)); \
        data->size = size;                                    \
        data->slock = src->lock + i;                          \
        data->smutex = &src->mutex;                           \
        data->dlock = dst->lock + i;                          \
    } while (0)

#define mrtstr_wait_process                                        \
    do                                                             \
    {                                                              \
        while (!mrtstr_threads.free_threads && data->size > 65536) \
        {                                                          \
            for (j = 0; j < 65536; sblock++, dblock++, j++)        \
            {                                                      \
                block = mrtstr_memcpy_load(sblock);                \
                mrtstr_memcpy_store(dblock, block);                \
            }                                                      \
                                                                   \
            data->size -= 65536;                                   \
        }                                                          \
    } while (0)

struct __MRTSTR_MEMCPY_T
{
    mrtstr_memcpy_simd_t *src;
    mrtstr_memcpy_simd_t *dst;
    mrtstr_size_t size;

    mrtstr_lock_t *slock;
    pthread_mutex_t *smutex;

    mrtstr_lock_t *dlock;
};
typedef struct __MRTSTR_MEMCPY_T *mrtstr_memcpy_t;

struct __MRTSTR_MEMCPY2_T
{
    mrtstr_memcpy_simd_t *src;
    mrtstr_memcpy_simd_t *dst;
    mrtstr_size_t size;

    mrtstr_lock_t *lock;
};
typedef struct __MRTSTR_MEMCPY2_T *mrtstr_memcpy2_t;

void mrtstr_memcpy_threaded(void *args);
void mrtstr_memcpy2_threaded(void *args);

void mrtstr_memcpy(mrtstr_t dst, mrtstr_ct src, mrtstr_size_t size)
{
    mrtstr_memcpy_simd_t *sblock = (mrtstr_memcpy_simd_t*)src->data;
    mrtstr_memcpy_simd_t *dblock = (mrtstr_memcpy_simd_t*)dst->data;

    if (size <= MRTSTR_MEMCPY_TLIMIT)
    {
#ifdef MRTSTR_MEMCPY_NOSIMD
        memcpy(dst->data, src->data, size);
#else
        mrtstr_size_t rem = size & MRTSTR_MEMCPY_SIMD_MASK;
        size >>= MRTSTR_MEMCPY_SIMD_SHIFT;

        mrtstr_memcpy_simd_t block;
        mrtstr_memcpy_sub(dblock, sblock, size);
        memcpy(dblock, sblock, rem);
#endif
        return;
    }

    mrtstr_size_t rem = size % MRTSTR_MEMCPY_TCHK;
    size /= MRTSTR_MEMCPY_TCHK;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_memcpy_simd_t block;
    mrtstr_memcpy_t data;
    if (mrtstr_locked(src) && src->forced)
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            for (; src->lock[i];);
            mrtstr_memcpy_data;
            mrtstr_wait_process;

            if (data->size <= 65536)
            {
                mrtstr_memcpy_sub(dblock, sblock, data->size);
                mrstr_free(data);
            }
            else
            {
                data->src = sblock;
                data->dst = dblock;

                sblock += data->size;
                dblock += data->size;

                src->lock[i] = 1;
                dst->lock[i] = 1;
                mrtstr_load_threads(mrtstr_memcpy_threaded, data);
            }
        }
    else
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            mrtstr_memcpy_data;
            mrtstr_wait_process;

            if (data->size <= 65536)
            {
                mrtstr_memcpy_sub(dblock, sblock, data->size);
                mrstr_free(data);
            }
            else
            {
                data->src = sblock;
                data->dst = dblock;

                sblock += data->size;
                dblock += data->size;

                mrtstr_lock_inc(src->lock[i], &src->mutex);
                dst->lock[i] = 1;
                mrtstr_load_threads(mrtstr_memcpy_threaded, data);
            }
        }

    memcpy(sblock, dblock, rem);

    src->forced = MRTSTR_FALSE;
    dst->forced = MRTSTR_TRUE;
}

void mrtstr_memcpy2(mrtstr_t dst, mrtstr_data_ct src, mrtstr_size_t size)
{
    mrtstr_memcpy_simd_t *sblock = (mrtstr_memcpy_simd_t*)src;
    mrtstr_memcpy_simd_t *dblock = (mrtstr_memcpy_simd_t*)dst->data;

    if (size <= MRTSTR_MEMCPY_TLIMIT)
    {
#ifdef MRTSTR_MEMCPY_NOSIMD
        memcpy(dst->data, src->data, size);
#else
        mrtstr_size_t rem = size & MRTSTR_MEMCPY_SIMD_MASK;
        size >>= MRTSTR_MEMCPY_SIMD_SHIFT;

        mrtstr_memcpy_simd_t block;
        mrtstr_memcpy2_sub(dblock, sblock, size);
        memcpy(dblock, sblock, rem);
#endif
        return;
    }

    mrtstr_size_t rem = size % MRTSTR_MEMCPY_TCHK;
    size /= MRTSTR_MEMCPY_TCHK;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_memcpy_simd_t block;
    mrtstr_memcpy2_t data;
    for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCPY2_T));
        data->size = size;
        data->lock = dst->lock + i;

        mrtstr_wait_process;

        if (data->size <= 65536)
        {
            mrtstr_memcpy2_sub(dblock, sblock, data->size);
            mrstr_free(data);
        }
        else
        {
            data->src = sblock;
            data->dst = dblock;

            sblock += data->size;
            dblock += data->size;

            dst->lock[i] = 1;
            mrtstr_load_threads(mrtstr_memcpy2_threaded, data);
        }
    }

    memcpy(sblock, dblock, rem);
    dst->forced = MRTSTR_TRUE;
}

void mrtstr_memcpy_threaded(void *args)
{
    mrtstr_memcpy_t data = (mrtstr_memcpy_t)args;

#ifdef MRTSTR_MEMCPY_NOSIMD
    memcpy(data->dst, data->src, data->size);
#else
    mrtstr_memcpy_simd_t block;
    mrtstr_memcpy_sub(data->dst, data->src, data->size);
#endif

    mrtstr_lock_dec(*data->slock, data->smutex);
    *data->dlock = 0;

    mrstr_free(data);
}

void mrtstr_memcpy2_threaded(void *args)
{
    mrtstr_memcpy2_t data = (mrtstr_memcpy2_t)args;

#ifdef MRTSTR_MEMCPY_NOSIMD
    memcpy(data->dst, data->src, data->size);
#else
    mrtstr_memcpy_simd_t block;
    mrtstr_memcpy2_sub(data->dst, data->src, data->size);
#endif

    *data->lock = 0;
    mrstr_free(data);
}
