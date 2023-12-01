/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifdef __AVX512F__

typedef __m512i mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 64
#define MRFSTR_MEMCMP_SIMD_SHIFT 6

#define mrfstr_memcmp_load _mm512_loadu_si512
#define mrfstr_memcmp_cmp _mm512_cmpneq_epi64_mask

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 32
#define MRFSTR_MEMCMP_SIMD_SHIFT 5

#define mrfstr_memcmp_load _mm256_loadu_si256
#define mrfstr_memcmp_cmp(x, y) (~_mm256_movemask_epi8(_mm256_cmpeq_epi64(x, y)))

#elif defined(__SSE2__)

typedef __m128i mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 16
#define MRFSTR_MEMCMP_SIMD_SHIFT 4

#define mrfstr_memcmp_load _mm_loadu_si128

#ifdef __SSE4_1__
#define mrfstr_memcmp_cmp(x, y) (~_mm_movemask_epi8(_mm_cmpeq_epi64(x, y)))
#else
#define mrfstr_memcmp_cmp(x, y) (~_mm_movemask_epi8(_mm_cmpeq_epi32(x, y)))
#endif

#else
#define MRFSTR_MEMCMP_NOSIMD

typedef unsigned long long mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 8

#define mrfstr_memcmp_load(x) *x
#define mrfstr_memcmp_cmp(x, y) x != y

#endif

#define MRFSTR_MEMCMP_SIMD_MASK (MRFSTR_MEMCMP_SIMD_SIZE - 1)

#if MRFSTR_THREADING
#include <pthread.h>

#define MRFSTR_MEMCMP_TLIMIT (65536 * MRFSTR_MEMCMP_SIMD_SIZE * MRFSTR_THREAD_COUNT - 1)
#define MRFSTR_MEMCMP_TCHK (MRFSTR_MEMCMP_SIMD_SIZE * MRFSTR_THREAD_COUNT)

struct __MRFSTR_MEMCMP_T
{
    mrfstr_memcmp_simd_t *str1;
    mrfstr_memcmp_simd_t *str2;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_MEMCMP_T *mrfstr_memcmp_t;

void *mrfstr_memcmp_threaded(void *args);
#endif

mrfstr_bool_t mrfstr_memcmp(mrfstr_data_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
    mrfstr_memcmp_simd_t *s1block = (mrfstr_memcmp_simd_t*)str1;
    mrfstr_memcmp_simd_t *s2block = (mrfstr_memcmp_simd_t*)str2;

#if MRFSTR_THREADING
    if (size <= MRFSTR_MEMCMP_TLIMIT)
    {
#endif
#ifdef MRFSTR_MEMCMP_NOSIMD
        return !memcmp(str1, str2, size);
#else
        mrfstr_size_t rem = size & MRFSTR_MEMCMP_SIMD_MASK;
        size >>= MRFSTR_MEMCMP_SIMD_MASK;

        mrfstr_memcmp_simd_t block1, block2;
        for (; size; s1block++, s2block++, size--)
        {
            block1 = mrfstr_memcmp_load(s1block);
            block2 = mrfstr_memcmp_load(s2block);
            if (mrfstr_memcmp_cmp(block1, block2))
                return MRFSTR_FALSE;
        }

        return !memcmp((mrfstr_data_ct)s1block, (mrfstr_data_ct)s2block, rem);
#endif
#if MRFSTR_THREADING
    }

    mrfstr_size_t rem = size % MRFSTR_MEMCMP_TCHK;
    size /= MRFSTR_MEMCMP_TCHK;

    volatile mrfstr_bool_t res = MRFSTR_TRUE;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_memcmp_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_MEMCMP_T));
        data->str1 = s1block;
        data->str2 = s2block;
        data->size = size;
        data->res = &res;

        s1block += size;
        s2block += size;

        pthread_create(threads + i, NULL, mrfstr_memcmp_threaded, data);
    }

    mrfstr_bool_t rres = !memcmp((mrfstr_data_t)s2block, (mrfstr_data_t)s1block, rem);
    if (!rres)
        res = MRFSTR_FALSE;

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    return res;
#endif
}

#if MRFSTR_THREADING
void *mrfstr_memcmp_threaded(void *args)
{
    mrfstr_memcmp_t data = (mrfstr_memcmp_t)args;

    mrfstr_size_t i;
    mrfstr_memcmp_simd_t block1, block2;
    while (data->size > 65536)
    {
        if (!*data->res)
        {
            mrstr_free(data);
            return NULL;
        }

        for (i = 0; i < 65536; data->str1++, data->str2++, i++)
        {
            block1 = mrfstr_memcmp_load(data->str1);
            block2 = mrfstr_memcmp_load(data->str2);
            if (mrfstr_memcmp_cmp(block1, block2))
            {
                *data->res = MRFSTR_FALSE;

                mrstr_free(data);
                return NULL;
            }
        }

        data->size -= 65536;
    }

    if (!*data->res)
    {
        mrstr_free(data);
        return NULL;
    }

    for (; data->size; data->str1++, data->str2++, data->size--)
    {
        block1 = mrfstr_memcmp_load(data->str1);
        block2 = mrfstr_memcmp_load(data->str2);
        if (mrfstr_memcmp_cmp(block1, block2))
        {
            *data->res = MRFSTR_FALSE;

            mrstr_free(data);
            return NULL;
        }
    }

    mrstr_free(data);
    return NULL;
}
#endif