/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#if MRFSTR_SIMD_SIZE == 64

#define mrfstr_memcpy_sub(x, y, s)             \
    do                                         \
    {                                          \
        mrfstr_simd_t block;                   \
        for (; (s); (s)--)                     \
        {                                      \
            block = _mm512_loadu_si512((y)++); \
            _mm512_stream_si512((x)++, block); \
        }                                      \
    } while (0)

#elif MRFSTR_SIMD_SIZE == 32

#define mrfstr_memcpy_sub(x, y, s)              \
    do                                          \
    {                                           \
        mrfstr_simd_t block1, block2;           \
        for (; s >= 2; s -= 2)                  \
        {                                       \
            block1 = _mm256_loadu_si256((y)++); \
            block2 = _mm256_loadu_si256((y)++); \
            _mm256_stream_si256((x)++, block1); \
            _mm256_stream_si256((x)++, block2); \
        }                                       \
                                                \
        if (s)                                  \
        {                                       \
            block1 = _mm256_loadu_si256((y)++); \
            _mm256_store_si256((x)++, block1);  \
        }                                       \
    } while (0)

#elif MRFSTR_SIMD_SIZE == 16

#define mrfstr_memcpy_sub(x, y, s)                     \
    do                                                 \
    {                                                  \
        mrfstr_simd_t  block1, block2, block3, block4; \
        for (; s >= 4; s -= 4)                         \
        {                                              \
            block1 = _mm_loadu_si128((y)++);           \
            block2 = _mm_loadu_si128((y)++);           \
            block3 = _mm_loadu_si128((y)++);           \
            block4 = _mm_loadu_si128((y)++);           \
            _mm_stream_si128((x)++, block1);           \
            _mm_stream_si128((x)++, block2);           \
            _mm_stream_si128((x)++, block3);           \
            _mm_stream_si128((x)++, block4);           \
        }                                              \
                                                       \
        while (s--)                                    \
        {                                              \
            block1 = _mm_loadu_si128((y)++);           \
            _mm_store_si128((x)++, block1);            \
        }                                              \
    } while (0)

#endif

#if MRFSTR_THREADING

#pragma pack(push, 1)
struct __MRFSTR_MEMCPY_T
{
    mrfstr_simd_t *src;
    mrfstr_simd_t *dst;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_MEMCPY_T *mrfstr_memcpy_t;
#pragma pack(pop)

#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_memcpy_threaded(void *args);
#elif defined(_WIN32)
DWORD WINAPI mrfstr_memcpy_threaded(LPVOID args);
#endif

#endif

void mrfstr_memcpy(mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    if (size < MRFSTR_SIMD_SLIMIT)
    {
        memcpy(dst, src, (size_t)size);
        return;
    }

#ifndef MRFSTR_NOSIMD
    mrfstr_byte_t align = (uintptr_t)dst & MRFSTR_SIMD_MASK;
    if (align)
    {
        align = MRFSTR_SIMD_SIZE - align;
        memcpy(dst, src, align);

        dst += align;
        src += align;
        size -= align;
    }
#endif

#if !defined(MRFSTR_NOSIMD) || MRFSTR_THREADING
    mrfstr_simd_t *dblock = (mrfstr_simd_t*)dst;
    mrfstr_simd_t *sblock = (mrfstr_simd_t*)src;
#endif

#if MRFSTR_THREADING
    if (size < MRFSTR_SIMD_TLIMIT)
    {
#endif
#ifdef MRFSTR_NOSIMD
        memcpy(dst, src, size);
#else
        mrfstr_byte_t rem = size & MRFSTR_SIMD_MASK;
        size >>= MRFSTR_SIMD_SHIFT;

        mrfstr_memcpy_sub(dblock, sblock, size);
        memcpy(dblock, sblock, rem);
#endif
#if MRFSTR_THREADING
        return;
    }

#ifdef MRFSTR_NOSIMD
    mrfstr_byte_t rem = size % MRFSTR_THREAD_COUNT;
    size /= MRFSTR_THREAD_COUNT;
#else
    mrfstr_short_t rem = size % MRFSTR_SIMD_TCHK;
    size /= MRFSTR_SIMD_TCHK;
#endif

    mrfstr_thread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_byte_t i;
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

        mrfstr_create_thread(mrfstr_memcpy_threaded)
        {
            sblock -= size;
            dblock -= size;

            mrstr_free(data);
            goto rem;
        }
    }

    memcpy(dblock, sblock, rem);

ret:
    mrfstr_close_threads;
    return;

rem:
    size *= MRFSTR_THREAD_COUNT - i;

#ifdef MRFSTR_NOSIMD
    memcpy(dblock, sblock, size + rem);
#else
    mrfstr_memcpy_sub(dblock, sblock, size);
    memcpy(dblock, sblock, rem);
#endif
    goto ret;
#endif
}

#if MRFSTR_THREADING
#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_memcpy_threaded(void *args)
#elif defined(_WIN32)
DWORD WINAPI mrfstr_memcpy_threaded(LPVOID args)
#endif
{
    mrfstr_memcpy_t data = args;

#ifdef MRFSTR_NOSIMD
    memcpy(data->dst, data->src, data->size);
#else
    mrfstr_memcpy_sub(data->dst, data->src, data->size);
#endif

    mrstr_free(data);
    return MRFSTR_TFUNC_RET;
}
#endif
