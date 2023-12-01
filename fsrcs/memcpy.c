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

typedef __m512i mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 64
#define MRFSTR_MEMCMP_SIMD_SHIFT 6

#define mrfstr_memcmp_load _mm256_loadu_si256
#define mrfstr_memcmp_cmp(x, y) _mm512_cmpneq_epi64_mask

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

#define MRFSTR_MEMCPY_SIMD_MASK (MRFSTR_MEMCPY_SIMD_SIZE - 1)

#if MRFSTR_THREADING
#include <pthread.h>

#define MRFSTR_MEMCPY_TLIMIT (65536 * MRFSTR_MEMCPY_SIMD_SIZE * MRFSTR_THREAD_COUNT - 1)
#define MRFSTR_MEMCPY_TCHK (MRFSTR_MEMCPY_SIMD_SIZE * MRFSTR_THREAD_COUNT)

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
    mrfstr_memcpy_simd_t *sblock = (mrfstr_memcpy_simd_t*)src;
    mrfstr_memcpy_simd_t *dblock = (mrfstr_memcpy_simd_t*)dst;

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
        for (; size; sblock++, dblock++, size--)
        {
            block = mrfstr_memcpy_load(sblock);
            mrfstr_memcpy_store(dblock, block);
        }

        mrfstr_data_t dptr = (mrfstr_data_t)dblock;
        memcpy(dptr, (mrfstr_data_t)sblock, rem);
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
        data->src = sblock;
        data->dst = dblock;
        data->size = size;

        sblock += size;
        dblock += size;

        pthread_create(threads + i, NULL, mrfstr_memcpy_threaded, data);
    }

    mrfstr_data_t dptr = (mrfstr_data_t)dblock;
    memcpy(dptr, (mrfstr_data_t)sblock, rem);

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);
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
    for (; data->size; data->src++, data->dst++, data->size--)
    {
        block = mrfstr_memcpy_load(data->src);
        mrfstr_memcpy_store(data->dst, block);
    }
#endif

    mrstr_free(data);
    return NULL;
}
#endif
