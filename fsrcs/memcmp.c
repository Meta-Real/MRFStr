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

#define mrfstr_memcmp_load _mm512_load_si512
#define mrfstr_memcmp_loadu _mm512_loadu_si512
#define mrfstr_memcmp_cmp _mm512_cmpneq_epi64_mask

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 32
#define MRFSTR_MEMCMP_SIMD_SHIFT 5

#define mrfstr_memcmp_load _mm256_load_si256
#define mrfstr_memcmp_loadu _mm256_loadu_si256
#define mrfstr_memcmp_cmp(x, y) (~_mm256_movemask_epi8(_mm256_cmpeq_epi64(x, y)))

#elif defined(__SSE2__)

typedef __m128i mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 16
#define MRFSTR_MEMCMP_SIMD_SHIFT 4

#define mrfstr_memcmp_load _mm_load_si128
#define mrfstr_memcmp_loadu _mm_loadu_si128
#define mrfstr_memcmp_cmp(x, y) (_mm_movemask_epi8(_mm_cmpeq_epi32(x, y)) != 0xffff)

#else
#define MRFSTR_MEMCMP_NOSIMD

typedef unsigned long long mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 8

#define mrfstr_memcmp_load(x) *x
#define mrfstr_memcmp_loadu mrfstr_memcmp_load
#define mrfstr_memcmp_cmp(x, y) x != y

#endif

#ifndef MRFSTR_MEMCMP_NOSIMD
#define MRFSTR_MEMCMP_SIMD_MASK (MRFSTR_MEMCMP_SIMD_SIZE - 1)
#endif

#define mrfstr_memcmp_sub(x, y)                \
    do                                         \
    {                                          \
        block1 = mrfstr_memcmp_load(x);        \
        block2 = mrfstr_memcmp_loadu(y);       \
        if (mrfstr_memcmp_cmp(block1, block2)) \
        {                                      \
            res = MRFSTR_FALSE;                \
            goto ret;                          \
        }                                      \
    } while (0)

#if MRFSTR_THREADING
#include <pthread.h>

#define MRFSTR_MEMCMP_TCHK (MRFSTR_MEMCMP_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_MEMCMP_TLIMIT (0x10000 * MRFSTR_MEMCMP_TCHK - 1)

#define mrfstr_memcmp_tsub(x, y)               \
    do                                         \
    {                                          \
        block1 = mrfstr_memcmp_load(x);        \
        block2 = mrfstr_memcmp_loadu(y);       \
        if (mrfstr_memcmp_cmp(block1, block2)) \
        {                                      \
            *data->res = MRFSTR_FALSE;         \
                                               \
            mrstr_free(data);                  \
            return NULL;                       \
        }                                      \
    } while (0)

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
#if !defined(MRFSTR_MEMCMP_NOSIMD) || MRFSTR_THREADING
    mrfstr_memcmp_simd_t *s1block = (mrfstr_memcmp_simd_t*)str1;
    mrfstr_memcmp_simd_t *s2block = (mrfstr_memcmp_simd_t*)str2;
#endif

#if MRFSTR_THREADING
    if (size <= MRFSTR_MEMCMP_TLIMIT)
    {
single:
#endif
#ifdef MRFSTR_MEMCMP_NOSIMD
        return !memcmp(str1, str2, size);
#else
        mrfstr_size_t rem = size & MRFSTR_MEMCMP_SIMD_MASK;
        size >>= MRFSTR_MEMCMP_SIMD_SHIFT;

        mrfstr_memcmp_simd_t block1, block2;
        for (; size; s1block++, s2block++, size--)
        {
            block1 = mrfstr_memcmp_load(s1block);
            block2 = mrfstr_memcmp_loadu(s2block);
            if (mrfstr_memcmp_cmp(block1, block2))
                return MRFSTR_FALSE;
        }

        return !memcmp(s1block, s2block, rem);
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
        if (!data)
        {
            if (!i)
                goto single;
            goto rem;
        }

        data->str1 = s1block;
        data->str2 = s2block;
        data->size = size;
        data->res = &res;

        s1block += size;
        s2block += size;

        if (pthread_create(threads + i, NULL, mrfstr_memcmp_threaded, data))
        {
            s1block -= size;
            s2block -= size;
            mrstr_free(data);

            if (!i)
                goto single;
            goto rem;
        }
    }

ret:
    if (res && memcmp(s2block, s1block, rem))
        res = MRFSTR_FALSE;

    while (i)
        pthread_join(threads[--i], NULL);
    return res;

rem:
    size *= MRFSTR_THREAD_COUNT - i;

    mrfstr_memcmp_simd_t block1, block2;
    mrfstr_size_t j;
    while (size >= 0x10000)
    {
        if (!res)
            goto ret;

        for (j = 0; j < 0x10000; s1block++, s2block++, j++)
            mrfstr_memcmp_sub(s1block, s2block);

        size -= 0x10000;
    }

    if (!res)
        goto ret;

    for (; size; s1block++, s2block++, size--)
        mrfstr_memcmp_sub(s1block, s2block);

    goto ret;
#endif
}

#if MRFSTR_THREADING
void *mrfstr_memcmp_threaded(void *args)
{
    mrfstr_memcmp_t data = (mrfstr_memcmp_t)args;

    mrfstr_size_t i;
    mrfstr_memcmp_simd_t block1, block2;
    while (data->size >= 0x10000)
    {
        if (!*data->res)
        {
            mrstr_free(data);
            return NULL;
        }

        for (i = 0; i < 0x10000; data->str1++, data->str2++, i++)
            mrfstr_memcmp_tsub(data->str1, data->str2);

        data->size -= 0x10000;
    }

    if (!*data->res)
    {
        mrstr_free(data);
        return NULL;
    }

    for (; data->size; data->str1++, data->str2++, data->size--)
        mrfstr_memcmp_tsub(data->str1, data->str2);

    mrstr_free(data);
    return NULL;
}
#endif
