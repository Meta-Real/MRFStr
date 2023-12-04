/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifdef __AVX512F__

typedef __m512i mrfstr_memset_simd_t;
#define MRFSTR_MEMSET_SIMD_SIZE 64
#define MRFSTR_MEMSET_SIMD_SHIFT 6

#define mrfstr_memset_set _mm512_set1_epi8
#define mrfstr_memset_store _mm512_stream_si512

#elif defined(__AVX__)

typedef __m256i mrfstr_memset_simd_t;
#define MRFSTR_MEMSET_SIMD_SIZE 32
#define MRFSTR_MEMSET_SIMD_SHIFT 5

#define mrfstr_memset_set _mm256_set1_epi8
#define mrfstr_memset_store _mm256_stream_si256

#elif defined(__SSE2__)

typedef __m128i mrfstr_memset_simd_t;
#define MRFSTR_MEMSET_SIMD_SIZE 16
#define MRFSTR_MEMSET_SIMD_SHIFT 4

#define mrfstr_memset_set _mm_set1_epi8
#define mrfstr_memset_store _mm_stream_si128

#else
#define MRFSTR_MEMSET_NOSIMD

typedef mrfstr_chr_t mrfstr_memset_simd_t;
#define MRFSTR_MEMSET_SIMD_SIZE 1

#define mrfstr_memset_set(x) x

#endif

#define MRFSTR_MEMSET_SIMD_MASK (MRFSTR_MEMSET_SIMD_SIZE - 1)

#if MRFSTR_THREADING
#include <pthread.h>

#define MRFSTR_MEMSET_TCHK (MRFSTR_MEMSET_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_MEMSET_TLIMIT (65536 * MRFSTR_MEMSET_TCHK - 1)

struct __MRFSTR_MEMSET_T
{
    mrfstr_memset_simd_t *res;
    mrfstr_memset_simd_t block;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_MEMSET_T *mrfstr_memset_t;

void *mrfstr_memset_threaded(void *args);
#endif

void mrfstr_memset(mrfstr_data_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_memset_simd_t *rblock = (mrfstr_memset_simd_t*)res;
    mrfstr_memset_simd_t block = mrfstr_memset_set(chr);

#if MRFSTR_THREADING
    if (size <= MRFSTR_MEMSET_TLIMIT)
    {
#endif
#ifdef MRFSTR_REPCHR_NOSIMD
        memset(rblock, block, size);
        rblock[size] = '\0';
#else
        mrfstr_size_t rem = size & MRFSTR_MEMSET_SIMD_MASK;
        size >>= MRFSTR_MEMSET_SIMD_SHIFT;

        for (; size; rblock++, size--)
            mrfstr_memset_store(rblock, block);

        mrfstr_data_t rptr = (mrfstr_data_t)rblock;
        memset(rptr, chr, rem);
        rptr[rem] = '\0';
#endif
#if MRFSTR_THREADING
        return;
    }

    mrfstr_size_t rem = size % MRFSTR_MEMSET_TCHK;
    size /= MRFSTR_MEMSET_TCHK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_memset_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_MEMSET_T));
        data->res = rblock;
        data->block = block;
        data->size = size;

        rblock += size;

        pthread_create(threads + i, NULL, mrfstr_memset_threaded, data);
    }

    mrfstr_data_t rptr = (mrfstr_data_t)rblock;
    memset(rptr, chr, rem);
    rptr[rem] = '\0';

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);
#endif
}

#if MRFSTR_THREADING
void *mrfstr_memset_threaded(void *args)
{
    mrfstr_memset_t data = (mrfstr_memset_t)args;

#ifdef MRFSTR_MEMSET_NOSIMD
    memset(data->res, data->block, data->size);
#else
    for (; data->size; data->res++, data->size--)
        mrfstr_memset_store(data->res, data->block);
#endif

    mrstr_free(data);
    return NULL;
}
#endif
