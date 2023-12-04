/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifdef __AVX512F__

typedef __m512i mrtstr_memset_simd_t;
#define MRTSTR_MEMSET_SIMD_SIZE 64
#define MRTSTR_MEMSET_SIMD_SHIFT 6

#define mrtstr_memset_set _mm512_set1_epi8
#define mrtstr_memset_store _mm512_stream_si512

#elif defined(__AVX__)

typedef __m256i mrtstr_memset_simd_t;
#define MRTSTR_MEMSET_SIMD_SIZE 32
#define MRTSTR_MEMSET_SIMD_SHIFT 5

#define mrtstr_memset_set _mm256_set1_epi8
#define mrtstr_memset_store _mm256_stream_si256

#elif defined(__SSE2__)

typedef __m128i mrtstr_memset_simd_t;
#define MRTSTR_MEMSET_SIMD_SIZE 16
#define MRTSTR_MEMSET_SIMD_SHIFT 4

#define mrtstr_memset_set _mm_set1_epi8
#define mrtstr_memset_store _mm_stream_si128

#else
#define MRTSTR_MEMSET_NOSIMD

typedef mrtstr_chr_t mrtstr_memset_simd_t;
#define MRTSTR_MEMSET_SIMD_SIZE 1

#define mrtstr_memset_set(x) x

#endif

#define MRTSTR_MEMSET_SIMD_MASK (MRTSTR_MEMSET_SIMD_SIZE - 1)

#define MRTSTR_MEMSET_TCHK (MRTSTR_MEMSET_SIMD_SIZE * MRTSTR_THREAD_COUNT)
#define MRTSTR_MEMSET_TLIMIT (65536 * MRTSTR_MEMSET_TCHK - 1)

struct __MRTSTR_MEMSET_T
{
    mrtstr_memset_simd_t *str;
    mrtstr_memset_simd_t block;
    mrtstr_size_t size;

    mrtstr_lock_t *lock;
};
typedef struct __MRTSTR_MEMSET_T *mrtstr_memset_t;

void mrtstr_memset_threaded(void *args);

void mrtstr_memset(mrtstr_t str, mrtstr_chr_t chr, mrtstr_size_t size)
{
    mrtstr_memset_simd_t *rblock = (mrtstr_memset_simd_t*)str->data;
    mrtstr_memset_simd_t block = mrtstr_memset_set(chr);

    if (size <= MRTSTR_MEMSET_TLIMIT)
    {
#ifdef MRTSTR_MEMSET_NOSIMD
        memset(rblock, chr, size);
        rblock[size] = '\0';
#else
        mrtstr_size_t rem = size & MRTSTR_MEMSET_SIMD_MASK;
        size >>= MRTSTR_MEMSET_SIMD_SHIFT;

        for (; size; rblock++, size--)
            mrtstr_memset_store(rblock, block);

        mrtstr_data_t rptr = (mrtstr_data_t)rblock;
        memset(rptr, chr, rem);
        rptr[rem] = '\0';
#endif
        return;
    }

    mrtstr_size_t rem = size % MRTSTR_MEMSET_TCHK;
    size /= MRTSTR_MEMSET_TCHK;

    mrtstr_size_t j;
    mrtstr_bit_t i;
    mrtstr_memset_t data;
    for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRTSTR_MEMSET_T));
        data->block = block;
        data->size = size;
        data->lock = str->lock + i;

        while (!mrtstr_threads.free_threads && data->size > 65536)
        {
            for (j = 0; j < 65536; rblock++, j++)
                mrtstr_memset_store(rblock, block);

            data->size -= 65536;
        }

        if (data->size <= 65536)
        {
            for (; data->size; rblock++, data->size--)
                mrtstr_memset_store(rblock, block);

            mrstr_free(data);
        }
        else
        {
            data->str = rblock;
            rblock += data->size;

            str->lock[i] = 1;
            mrtstr_load_threads(mrtstr_memset_threaded, data);
        }
    }

    mrtstr_data_t rptr = (mrtstr_data_t)rblock;
    memset(rptr, chr, rem);
    rptr[rem] = '\0';

    str->forced = MRTSTR_TRUE;
}

void mrtstr_memset_threaded(void *args)
{
    mrtstr_memset_t data = (mrtstr_memset_t)args;

#ifdef MRTSTR_MEMSET_NOSIMD
    memset(data->res, data->block, data->size);
#else
    for (; data->size; data->str++, data->size--)
        mrtstr_memset_store(data->str, data->block);
#endif

    *data->lock = 0;
    mrstr_free(data);
}
