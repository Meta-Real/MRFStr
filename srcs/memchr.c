/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifdef _WIN32
#if !MRFSTR_THREADING
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <intrin.h>
#endif

#if MRFSTR_SIMD_SIZE == 64

typedef __m512i mrfstr_memchr_simd_t;
#define MRFSTR_MEMCHR_SIMD_SIZE 64
#define MRFSTR_MEMCHR_SIMD_SHIFT 6

#define mrfstr_memchr_set(x, y) ((x) = _mm512_set1_epi8(y))
#define mrfstr_memchr_loadu _mm512_loadu_si512
#define mrfstr_memchr_cmpeq _mm512_cmpeq_epi8_mask

#define mrfstr_memchr2_cmpeq(r, x, y) ((r) = mrfstr_memchr_cmpeq((x), (y)))

#elif MRFSTR_SIMD_SIZE == 32 && defined(__AVX2__)

typedef __m256i mrfstr_memchr_simd_t;
#define MRFSTR_MEMCHR_SIMD_SIZE 32
#define MRFSTR_MEMCHR_SIMD_SHIFT 5

#define mrfstr_memchr_set(x, y) ((x) = _mm256_set1_epi8(y))
#define mrfstr_memchr_loadu _mm256_loadu_si256
#define mrfstr_memchr_cmpeq(x, y) (_mm256_movemask_epi8(_mm256_cmpeq_epi8((x), (y))))

#define mrfstr_memchr2_cmpeq(r, x, y) ((r) = mrfstr_memchr_cmpeq((x), (y)))

#elif defined(__SSE2__)

typedef __m128i mrfstr_memchr_simd_t;
#define MRFSTR_MEMCHR_SIMD_SIZE 16
#define MRFSTR_MEMCHR_SIMD_SHIFT 4

#define mrfstr_memchr_set(x, y) ((x) = _mm_set1_epi8(y))
#define mrfstr_memchr_loadu _mm_loadu_si128
#define mrfstr_memchr_cmpeq(x, y) (_mm_movemask_epi8(_mm_cmpeq_epi8((x), (y))))

#define mrfstr_memchr2_cmpeq(r, x, y) ((r) = mrfstr_memchr_cmpeq((x), (y)))

#else
#define MRFSTR_MEMCHR_NOSIMD

typedef uint64_t mrfstr_memchr_simd_t;
#define MRFSTR_MEMCHR_SIMD_SIZE 8
#define MRFSTR_MEMCHR_SIMD_SHIFT 3

#define mrfstr_memchr_set(x, y) \
    do                          \
    {                           \
        (x) = (y) | ((y) << 8); \
        (x) |= ((x) << 16);     \
        (x) |= ((x) << 32);     \
    } while (0)

#define mrfstr_memchr_loadu(x) (*(x))
#define mrfstr_memchr_cmpeq(r, x, y)                                        \
    do                                                                      \
    {                                                                       \
        (x) ^= (y);                                                         \
        (r) = ((x) - 0x1010101010101010ULL) & ~(x) & 0x8080808080808080ULL; \
    } while (0)

#define mrfstr_memchr2_cmpeq mrfstr_memchr_cmpeq

#endif

#define MRFSTR_MEMCHR_SIMD_MASK (MRFSTR_MEMCHR_SIMD_SIZE - 1)
#define MRFSTR_MEMCHR_SLIMIT (0x100 * MRFSTR_MEMCHR_SIMD_SIZE)

#if MRFSTR_THREADING

#define MRFSTR_MEMCHR_TCHK (MRFSTR_MEMCHR_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_MEMCHR_TLIMIT (0x10000 * MRFSTR_MEMCHR_TCHK)

#pragma pack(push, 1)
struct __MRFSTR_MEMCHR_T
{
    mrfstr_memchr_simd_t *str;
    mrfstr_memchr_simd_t chr;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_MEMCHR_T *mrfstr_memchr_t;
#pragma pack(pop)

#pragma pack(push, 1)
struct __MRFSTR_MEMCHR2_T
{
    mrfstr_memchr_simd_t *str;
    mrfstr_memchr_simd_t chr;
    mrfstr_size_t size;
    mrfstr_idx_t start;

    volatile mrfstr_idx_t *res;
    mrfstr_mutex_p mutex;
};
typedef struct __MRFSTR_MEMCHR2_T *mrfstr_memchr2_t;
#pragma pack(pop)

#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_memchr_threaded(void *args);
void *mrfstr_memchr2_threaded(void *args);
#elif defined(_WIN32)
DWORD WINAPI mrfstr_memchr_threaded(LPVOID args);
DWORD WINAPI mrfstr_memchr2_threaded(LPVOID args);
#endif

#endif

#ifndef __MRFSTR_DEBUG__
inline mrfstr_byte_t bsf(mrfstr_size_t bits)
#else
mrfstr_byte_t bsf(mrfstr_size_t bits)
#endif
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(bits);
#elif defined(_MSC_VER)
    mrfstr_byte_t idx;
    BitScanForward64((DWORD*)&idx, bits);
    return idx;
#else
    mrfstr_byte_t n = 1;

    if (!(bits & 0x0000FFFF))
    {
        n += 16;
        bits >>= 16;
    }
    if (!(bits & 0x000000FF))
    {
        n += 8;
        bits >>= 8;
    }
    if (!(bits & 0x0000000F))
    {
        n += 4;
        bits >>= 4;
    }
    if (!(bits & 0x00000003))
    {
        n += 2;
        bits >>= 2;
    }

    return n - (bits & 1);
#endif
}

mrfstr_bool_t mrfstr_memchr(mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (size < MRFSTR_MEMCHR_SLIMIT)
        return !memchr(str, chr, (size_t)size);

#ifndef MRFSTR_MEMCHR_NOSIMD
    mrfstr_byte_t align = (uintptr_t)str & MRFSTR_MEMCHR_SIMD_MASK;
    if (align)
    {
        align = MRFSTR_MEMCHR_SIMD_SIZE - align;
        if (memchr(str, chr, align))
            return MRFSTR_TRUE;

        str += align;
        size -= align;
    }
#endif

    mrfstr_memchr_simd_t *sblock = (mrfstr_memchr_simd_t *)str;
    mrfstr_memchr_simd_t cblock;
    mrfstr_memchr_set(cblock, chr);

    mrfstr_short_t rem;
#if MRFSTR_THREADING
    if (size < MRFSTR_MEMCHR_TLIMIT)
    {
single:
#endif
        rem = size & MRFSTR_MEMCHR_SIMD_MASK;
        size >>= MRFSTR_MEMCHR_SIMD_SHIFT;

        mrfstr_memchr_simd_t block;
#ifdef MRFSTR_MEMCHR_NOSIMD
        mrstr_size_t cres;
#endif
        for (; size; sblock++, size--)
        {
            block = mrfstr_memchr_loadu(sblock);

#ifdef MRFSTR_MEMCHR_NOSIMD
            mrfstr_memchr_cmpeq(cres, block, cblock);
            if (cres)
#else
            if (mrfstr_memchr_cmpeq(block, cblock))
#endif
                return MRFSTR_TRUE;
        }

        return memchr(sblock, chr, rem) != NULL;
#if MRFSTR_THREADING
    }

    rem = size % MRFSTR_MEMCHR_TCHK;
    size /= MRFSTR_MEMCHR_TCHK;

    volatile mrfstr_bool_t res = MRFSTR_FALSE;

    mrfstr_thread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_byte_t i;
    mrfstr_memchr_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_MEMCHR_T));
        if (!data)
        {
            if (!i)
                goto single;
            goto rem;
        }

        data->str = sblock;
        data->chr = cblock;
        data->size = size;
        data->res = &res;

        sblock += size;

        mrfstr_create_thread(mrfstr_memchr_threaded)
        {
            sblock -= size;
            mrstr_free(data);

            if (!i)
                goto single;
            goto rem;
        }
    }

    if (!res && memchr(sblock, chr, rem))
        res = MRFSTR_TRUE;

ret:
    mrfstr_close_threads;
    return res;

rem:
    size *= MRFSTR_THREAD_COUNT - i;

    mrfstr_long_t j;
    mrfstr_memchr_simd_t block;
#ifdef MRFSTR_MEMCHR_NOSIMD
    mrfstr_bool_t cres;
#endif
    while (size >= 0x10000)
    {
        if (res)
            goto ret;

        for (j = 0; j < 0x10000; sblock++, j++)
        {
            block = mrfstr_memchr_loadu(sblock);

#ifdef MRFSTR_MEMCHR_NOSIMD
            mrfstr_memchr_cmpeq(cres, block, cblock);
            if (cres)
#else
            if (mrfstr_memchr_cmpeq(block, cblock))
#endif
            {
                res = MRFSTR_TRUE;
                goto ret;
            }
        }

        size -= 0x10000;
    }

    if (res)
        goto ret;

    for (; size; sblock++, size--)
    {
        block = mrfstr_memchr_loadu(sblock);

#ifdef MRFSTR_MEMCHR_NOSIMD
        mrfstr_memchr_cmpeq(cres, block, cblock);
        if (cres)
#else
        if (mrfstr_memchr_cmpeq(block, cblock))
#endif
        {
            res = MRFSTR_TRUE;
            goto ret;
        }
    }

    if (memchr(sblock, chr, rem))
        res = MRFSTR_TRUE;

    goto ret;
#endif
}

mrfstr_idx_t mrfstr_memchr2(mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (size < MRFSTR_MEMCHR_SLIMIT)
    {
        mrfstr_data_t ptr = memchr(str, chr, (size_t)size);
        return ptr ? (mrfstr_idx_t)(ptr - str) : MRFSTR_INVIDX;
    }

#ifndef MRFSTR_MEMCHR_NOSIMD
    mrfstr_byte_t align = (uintptr_t)str & MRFSTR_MEMCHR_SIMD_MASK;
    if (align)
    {
        align = MRFSTR_MEMCHR_SIMD_SIZE - align;
        mrfstr_data_t ptr = memchr(str, chr, align);
        if (ptr)
            return (mrfstr_idx_t)(ptr - str);

        str += align;
        size -= align;
    }
#endif

    mrfstr_memchr_simd_t *sblock = (mrfstr_memchr_simd_t *)str;
    mrfstr_memchr_simd_t cblock;
    mrfstr_memchr_set(cblock, chr);

    mrfstr_short_t rem;
#if MRFSTR_THREADING
    if (size < MRFSTR_MEMCHR_TLIMIT)
    {
single:
#endif
        rem = size & MRFSTR_MEMCHR_SIMD_MASK;
        size >>= MRFSTR_MEMCHR_SIMD_SHIFT;

        mrfstr_memchr_simd_t block;
        mrfstr_size_t cres;
        for (; size; sblock++, size--)
        {
            block = mrfstr_memchr_loadu(sblock);

            mrfstr_memchr2_cmpeq(cres, block, cblock);
            if (cres)
                return (mrfstr_idx_t)(uintptr_t)sblock - (mrfstr_idx_t)(uintptr_t)str + bsf(cres);
        }

        mrfstr_data_t ptr = memchr(sblock, chr, rem);
        return ptr ? (mrfstr_idx_t)(ptr - str) : MRFSTR_INVIDX;
#if MRFSTR_THREADING
    }

    rem = size % MRFSTR_MEMCHR_TCHK;
    size /= MRFSTR_MEMCHR_TCHK;

    volatile mrfstr_idx_t res = MRFSTR_INVIDX;

    mrfstr_mutex_t mutex;
    mrfstr_create_mutex(mutex)
        goto single;

    mrfstr_thread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_byte_t i;
    mrfstr_memchr2_t data;
    mrfstr_data_t ptr;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_MEMCHR2_T));
        if (!data)
        {
            if (!i)
                goto single;
            goto rem;
        }

        data->str = sblock;
        data->chr = cblock;
        data->size = size;
        data->start = (mrfstr_idx_t)(uintptr_t)sblock - (mrfstr_idx_t)(uintptr_t)str;
        data->res = &res;
        data->mutex = MRFSTR_CAST_MUTEX(mutex);

        sblock += size;
        mrfstr_create_thread(mrfstr_memchr2_threaded)
        {
            sblock -= size;
            mrstr_free(data);

            if (!i)
                goto single;
            goto rem;
        }
    }

ret:
    ptr = memchr(sblock, chr, rem);
    if (ptr)
    {
        mrfstr_lock_mutex(MRFSTR_CAST_MUTEX(mutex));
        if (res == MRFSTR_INVIDX)
            res = (mrfstr_idx_t)(ptr - str);
        mrfstr_unlock_mutex(MRFSTR_CAST_MUTEX(mutex));
    }

    mrfstr_close_threads;
    mrfstr_close_mutex(mutex);
    return res;

rem:
    size *= MRFSTR_THREAD_COUNT - i;

    mrfstr_long_t j;
    mrfstr_memchr_simd_t block;
    mrfstr_size_t cres;
    while (size >= 0x10000)
    {
        if (res != MRFSTR_INVIDX)
            goto ret;

        for (j = 0; j < 0x10000; sblock++, j++)
        {
            block = mrfstr_memchr_loadu(sblock);

            mrfstr_memchr2_cmpeq(cres, block, cblock);
            if (cres)
            {
                mrfstr_lock_mutex(MRFSTR_CAST_MUTEX(mutex));
                if (res != MRFSTR_INVIDX)
                    res = (mrfstr_idx_t)(uintptr_t)sblock - (mrfstr_idx_t)(uintptr_t)str + bsf(cres);
                mrfstr_unlock_mutex(MRFSTR_CAST_MUTEX(mutex));
                goto ret;
            }
        }

        size -= 0x10000;
    }

    if (res != MRFSTR_INVIDX)
        goto ret;

    for (; size; sblock++, size--)
    {
        block = mrfstr_memchr_loadu(sblock);

        mrfstr_memchr2_cmpeq(cres, block, cblock);
        if (cres)
        {
            mrfstr_lock_mutex(MRFSTR_CAST_MUTEX(mutex));
            if (res != MRFSTR_INVIDX)
                res = (mrfstr_idx_t)(uintptr_t)sblock - (mrfstr_idx_t)(uintptr_t)str + bsf(cres);
            mrfstr_unlock_mutex(MRFSTR_CAST_MUTEX(mutex));
            goto ret;
        }
    }

    goto ret;
#endif
}

#if MRFSTR_THREADING

#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_memchr_threaded(void *args)
#elif defined(_WIN32)
DWORD WINAPI mrfstr_memchr_threaded(LPVOID args)
#endif
{
    mrfstr_memchr_t data = (mrfstr_memchr_t)args;

    mrfstr_long_t i;
    mrfstr_memchr_simd_t block;
#ifdef MRFSTR_MEMCHR_NOSIMD
    mrfstr_bool_t cres;
#endif
    while (data->size >= 0x10000)
    {
        if (*data->res)
        {
            mrstr_free(data);
            return MRFSTR_TFUNC_RET;
        }

        for (i = 0; i < 0x10000; data->str++, i++)
        {
            block = mrfstr_memchr_loadu(data->str);

#ifdef MRFSTR_MEMCHR_NOSIMD
            mrfstr_memchr_cmpeq(cres, block, data->chr);
            if (cres)
#else
            if (mrfstr_memchr_cmpeq(block, data->chr))
#endif
            {
                *data->res = MRFSTR_TRUE;

                mrstr_free(data);
                return MRFSTR_TFUNC_RET;
            }
        }

        data->size -= 0x10000;
    }

    if (*data->res)
    {
        mrstr_free(data);
        return MRFSTR_TFUNC_RET;
    }

    for (; data->size; data->str++, data->size--)
    {
        block = mrfstr_memchr_loadu(data->str);

#ifdef MRFSTR_MEMCHR_NOSIMD
        mrfstr_memchr_cmpeq(cres, block, data->chr);
        if (cres)
#else
        if (mrfstr_memchr_cmpeq(block, data->chr))
#endif
        {
            *data->res = MRFSTR_TRUE;

            mrstr_free(data);
            return MRFSTR_TFUNC_RET;
        }
    }

    mrstr_free(data);
    return MRFSTR_TFUNC_RET;
}

#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_memchr2_threaded(void *args)
#elif defined(_WIN32)
DWORD WINAPI mrfstr_memchr2_threaded(LPVOID args)
#endif
{
    mrfstr_memchr2_t data = (mrfstr_memchr2_t)args;
    mrfstr_data_t ptr = (mrfstr_data_t)data->str;

    mrfstr_size_t i, cres;
    mrfstr_memchr_simd_t block;
    while (data->size >= 0x10000)
    {
        if (*data->res < data->start)
        {
            mrstr_free(data);
            return MRFSTR_TFUNC_RET;
        }

        for (i = 0; i < 0x10000; data->str++, i++)
        {
            block = mrfstr_memchr_loadu(data->str);

            mrfstr_memchr2_cmpeq(cres, block, data->chr);
            if (cres)
            {
                mrfstr_lock_mutex(data->mutex);
                if (*data->res > data->start)
                    *data->res = (mrfstr_idx_t)(uintptr_t)data->str - (mrfstr_idx_t)(uintptr_t)ptr +
                        data->start + bsf(cres);
                mrfstr_unlock_mutex(data->mutex);

                mrstr_free(data);
                return MRFSTR_TFUNC_RET;
            }
        }

        data->size -= 0x10000;
    }

    if (*data->res < data->start)
    {
        mrstr_free(data);
        return MRFSTR_TFUNC_RET;
    }

    for (; data->size; data->str++, data->size--)
    {
        block = mrfstr_memchr_loadu(data->str);

        mrfstr_memchr2_cmpeq(cres, block, data->chr);
        if (cres)
        {
            mrfstr_lock_mutex(data->mutex);
            if (*data->res > data->start)
                *data->res = (mrfstr_idx_t)(uintptr_t)data->str - (mrfstr_idx_t)(uintptr_t)ptr +
                    data->start + bsf(cres);
            mrfstr_unlock_mutex(data->mutex);

            mrstr_free(data);
            return MRFSTR_TFUNC_RET;
        }
    }

    mrstr_free(data);
    return MRFSTR_TFUNC_RET;
}

#endif
