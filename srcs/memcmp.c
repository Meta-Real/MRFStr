/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#if MRFSTR_SIMD_SIZE == 64

typedef __m512i mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 64
#define MRFSTR_MEMCMP_SIMD_SHIFT 6

#define mrfstr_memcmp_loadu _mm512_loadu_si512
#define mrfstr_memcmp_cmpneq _mm512_cmpneq_epi64_mask

#elif MRFSTR_SIMD_SIZE == 32 && defined(__AVX2__)

typedef __m256i mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 32
#define MRFSTR_MEMCMP_SIMD_SHIFT 5

#define mrfstr_memcmp_loadu _mm256_loadu_si256
#define mrfstr_memcmp_cmpneq(x, y) (~_mm256_movemask_epi8(_mm256_cmpeq_epi64((x), (y))))

#elif defined(__SSE2__)

typedef __m128i mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 16
#define MRFSTR_MEMCMP_SIMD_SHIFT 4

#define mrfstr_memcmp_loadu _mm_loadu_si128
#define mrfstr_memcmp_cmpneq(x, y) (_mm_movemask_epi8(_mm_cmpeq_epi32((x), (y))) != 0xffff)

#else
#define MRFSTR_MEMCMP_NOSIMD

typedef uint64_t mrfstr_memcmp_simd_t;
#define MRFSTR_MEMCMP_SIMD_SIZE 8

#define mrfstr_memcmp_loadu(x) (*(x))
#define mrfstr_memcmp_cmpneq(x, y) ((x) != (y))

#endif

#ifndef MRFSTR_MEMCMP_NOSIMD
#define MRFSTR_MEMCMP_SIMD_MASK (MRFSTR_MEMCMP_SIMD_SIZE - 1)
#endif

#define MRFSTR_MEMCMP_SLIMIT (0x100 * MRFSTR_MEMCMP_SIMD_SIZE)

#define mrfstr_memcmp_sub(x, y)                   \
    do                                            \
    {                                             \
        block1 = mrfstr_memcmp_loadu(x);          \
        block2 = mrfstr_memcmp_loadu(y);          \
        if (mrfstr_memcmp_cmpneq(block1, block2)) \
        {                                         \
            res = MRFSTR_FALSE;                   \
            goto ret;                             \
        }                                         \
    } while (0)

#if MRFSTR_THREADING

#define MRFSTR_MEMCMP_TCHK (MRFSTR_MEMCMP_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_MEMCMP_TLIMIT (0x10000 * MRFSTR_MEMCMP_TCHK)

#define mrfstr_memcmp_tsub(x, y)                  \
    do                                            \
    {                                             \
        block1 = mrfstr_memcmp_loadu(x);          \
        block2 = mrfstr_memcmp_loadu(y);          \
        if (mrfstr_memcmp_cmpneq(block1, block2)) \
        {                                         \
            *data->res = MRFSTR_FALSE;            \
                                                  \
            mrstr_free(data);                     \
            return MRFSTR_TFUNC_RET;              \
        }                                         \
    } while (0)

struct __MRFSTR_MEMCMP_T
{
    mrfstr_memcmp_simd_t *str1;
    mrfstr_memcmp_simd_t *str2;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_MEMCMP_T *mrfstr_memcmp_t;

#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_memcmp_threaded(void *args);
#elif defined(_WIN32)
DWORD WINAPI mrfstr_memcmp_threaded(LPVOID args);
#endif

#endif

mrfstr_bool_t mrfstr_memcmp(mrfstr_data_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
    if (size < MRFSTR_MEMCMP_SLIMIT)
        return !memcmp(str1, str2, (size_t)size);

#ifndef MRFSTR_MEMCMP_NOSIMD
    mrfstr_byte_t align = (uintptr_t)str1 & MRFSTR_MEMCMP_SIMD_MASK;
    if (align)
    {
        align = MRFSTR_MEMCMP_SIMD_SIZE - align;
        if (memcmp(str1, str2, align))
            return MRFSTR_FALSE;

        str1 += align;
        str2 += align;
        size -= align;
    }
#endif

#if !defined(MRFSTR_MEMCMP_NOSIMD) || MRFSTR_THREADING
    mrfstr_memcmp_simd_t *s1block = (mrfstr_memcmp_simd_t*)str1;
    mrfstr_memcmp_simd_t *s2block = (mrfstr_memcmp_simd_t*)str2;

    mrfstr_short_t rem;
#endif

#if MRFSTR_THREADING
    if (size < MRFSTR_MEMCMP_TLIMIT)
    {
single:
#endif
#ifdef MRFSTR_MEMCMP_NOSIMD
        return !memcmp(str1, str2, size);
#else
        rem = size & MRFSTR_MEMCMP_SIMD_MASK;
        size >>= MRFSTR_MEMCMP_SIMD_SHIFT;

        mrfstr_memcmp_simd_t block1, block2;
        for (; size; s1block++, s2block++, size--)
        {
            block1 = mrfstr_memcmp_loadu(s1block);
            block2 = mrfstr_memcmp_loadu(s2block);
            if (mrfstr_memcmp_cmpneq(block1, block2))
                return MRFSTR_FALSE;
        }

        return !memcmp(s1block, s2block, rem);
#endif
#if MRFSTR_THREADING
    }

    rem = size % MRFSTR_MEMCMP_TCHK;
    size /= MRFSTR_MEMCMP_TCHK;

    volatile mrfstr_bool_t res = MRFSTR_TRUE;

    mrfstr_thread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_byte_t i;
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

        mrfstr_create_thread(mrfstr_memcmp_threaded)
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

    mrfstr_close_threads;
    return res;

rem:
    size *= MRFSTR_THREAD_COUNT - i;

    mrfstr_memcmp_simd_t block1, block2;
    mrfstr_long_t j;
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
#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_memcmp_threaded(void *args)
#elif defined(_WIN32)
DWORD WINAPI mrfstr_memcmp_threaded(LPVOID args)
#endif
{
    mrfstr_memcmp_t data = (mrfstr_memcmp_t)args;

    mrfstr_long_t i;
    mrfstr_memcmp_simd_t block1, block2;
    while (data->size >= 0x10000)
    {
        if (!*data->res)
        {
            mrstr_free(data);
            return MRFSTR_TFUNC_RET;
        }

        for (i = 0; i < 0x10000; data->str1++, data->str2++, i++)
            mrfstr_memcmp_tsub(data->str1, data->str2);

        data->size -= 0x10000;
    }

    if (!*data->res)
    {
        mrstr_free(data);
        return MRFSTR_TFUNC_RET;
    }

    for (; data->size; data->str1++, data->str2++, data->size--)
        mrfstr_memcmp_tsub(data->str1, data->str2);

    mrstr_free(data);
    return MRFSTR_TFUNC_RET;
}
#endif
