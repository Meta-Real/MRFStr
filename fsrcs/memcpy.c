/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifdef __AVX512F__

typedef __m512i mrfstr_memcpy_simd_t;
#define MRFSTR_MEMCPY_SIMD_SIZE 64
#define MRFSTR_MEMCPY_SIMD_SHIFT 6

#define mrfstr_memcpy_load _mm512_loadu_si512
#define mrfstr_memcpy_store _mm512_stream_si512

#elif defined(__AVX__)

typedef __m256i mrfstr_memcpy_simd_t;
#define MRFSTR_MEMCPY_SIMD_SIZE 32
#define MRFSTR_MEMCPY_SIMD_SHIFT 5

#define mrfstr_memcpy_load _mm256_loadu_si256
#define mrfstr_memcpy_store _mm256_stream_si256

#elif defined(__SSE2__)

typedef __m128i mrfstr_memcpy_simd_t;
#define MRFSTR_MEMCPY_SIMD_SIZE 16
#define MRFSTR_MEMCPY_SIMD_SHIFT 4

#define mrfstr_memcpy_load _mm_loadu_si128
#define mrfstr_memcpy_store _mm_stream_si128

#else
#define MRFSTR_MEMCPY_NOSIMD

typedef mrfstr_chr_t mrfstr_memcpy_simd_t;
#define MRFSTR_MEMCPY_SIMD_SIZE 1

#endif

#ifndef MRFSTR_MEMCPY_NOSIMD
#define MRFSTR_MEMCPY_SIMD_MASK (MRFSTR_MEMCPY_SIMD_SIZE - 1)

#define mrfstr_memcpy_sub(x, y, s)         \
    do                                     \
    {                                      \
        for (; s; x++, y++, s--)           \
        {                                  \
            block = mrfstr_memcpy_load(y); \
            mrfstr_memcpy_store(x, block); \
        }                                  \
    } while (0)

#endif

#if MRFSTR_THREADING
#include <pthread.h>

#define MRFSTR_MEMCPY_TCHK (MRFSTR_MEMCPY_SIMD_SIZE * MRFSTR_THREAD_COUNT)

#ifdef MRFSTR_MEMCPY_NOSIMD
#define MRFSTR_MEMCPY_TLIMIT (0x80000 * MRFSTR_THREAD_COUNT - 1)
#else
#define MRFSTR_MEMCPY_TLIMIT (0x10000 * MRFSTR_MEMCPY_TCHK - 1)
#endif

struct __MRFSTR_MEMCPY_T
{
    mrfstr_memcpy_simd_t *src;
    mrfstr_memcpy_simd_t *dst;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_MEMCPY_T *mrfstr_memcpy_t;

void *mrfstr_memcpy_threaded(void *args);
#endif

void mrfstr_memcpy(mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    mrfstr_memcpy_simd_t *dblock = (mrfstr_memcpy_simd_t*)dst;
    mrfstr_memcpy_simd_t *sblock = (mrfstr_memcpy_simd_t*)src;

#if MRFSTR_THREADING
    if (size <= MRFSTR_MEMCPY_TLIMIT)
    {
#endif
#ifdef MRFSTR_MEMCPY_NOSIMD
        memcpy(dst, src, size);
#else
        mrfstr_size_t rem = size & MRFSTR_MEMCPY_SIMD_MASK;
        size >>= MRFSTR_MEMCPY_SIMD_SHIFT;

        mrfstr_memcpy_simd_t block;
        mrfstr_memcpy_sub(dblock, sblock, size);
        memcpy(dblock, sblock, rem);
#endif
#if MRFSTR_THREADING
        return;
    }

    mrfstr_size_t rem = size % MRFSTR_MEMCPY_TCHK;
    size /= MRFSTR_MEMCPY_TCHK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_memcpy_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_MEMCPY_T));
        if (!data)
            goto rem;

        data->src = sblock;
        data->dst = dblock;
        data->size = size;

        sblock += size;
        dblock += size;

        if (pthread_create(threads + i, NULL, mrfstr_memcpy_threaded, data))
        {
            sblock -= size;
            dblock -= size;

            mrstr_free(data);
            goto rem;
        }
    }

    memcpy(dblock, sblock, rem);

ret:
    while (i)
        pthread_join(threads[--i], NULL);
    return;

rem:
    size *= MRFSTR_THREAD_COUNT - i;

#ifdef MRFSTR_MEMCPY_NOSIMD
    memcpy(dblock, sblock, size + rem);
#else
    mrfstr_memcpy_simd_t block;
    mrfstr_memcpy_sub(dblock, sblock, size);
    memcpy(dblock, sblock, rem);
#endif
    goto ret;
#endif
}

#if MRFSTR_THREADING
void *mrfstr_memcpy_threaded(void *args)
{
    mrfstr_memcpy_t data = args;

#ifdef MRFSTR_MEMCPY_NOSIMD
    memcpy(data->dst, data->src, data->size);
#else
    mrfstr_memcpy_simd_t block;
    mrfstr_memcpy_sub(data->dst, data->src, data->size);
#endif

    mrstr_free(data);
    return NULL;
}
#endif
