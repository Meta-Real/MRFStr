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

#define mrfstr_memcpy_sub(x, y, s)            \
    do                                        \
    {                                         \
        mrfstr_memcpy_simd_t                  \
            block1, block2, block3, block4;   \
                                              \
        for (; s >= 4; s -= 4)                \
        {                                     \
            block1 = _mm512_loadu_si512(y++); \
            block2 = _mm512_loadu_si512(y++); \
            block3 = _mm512_loadu_si512(y++); \
            block4 = _mm512_loadu_si512(y++); \
            _mm512_stream_si512(x++, block1); \
            _mm512_stream_si512(x++, block2); \
            _mm512_stream_si512(x++, block3); \
            _mm512_stream_si512(x++, block4); \
        }                                     \
                                              \
        while (s--)                           \
        {                                     \
            block1 = _mm512_loadu_si512(y++); \
            _mm512_stream_si512(x++, block1); \
        }                                     \
    } while (0)

#elif defined(__AVX__)

typedef __m256i mrfstr_memcpy_simd_t;
#define MRFSTR_MEMCPY_SIMD_SIZE 32
#define MRFSTR_MEMCPY_SIMD_SHIFT 5

#define mrfstr_memcpy_sub(x, y, s)            \
    do                                        \
    {                                         \
        mrfstr_memcpy_simd_t                  \
            block1, block2, block3, block4,   \
            block5, block6, block7, block8;   \
                                              \
        for (; s >= 8; s -= 8)                \
        {                                     \
            block1 = _mm256_loadu_si256(y++); \
            block2 = _mm256_loadu_si256(y++); \
            block3 = _mm256_loadu_si256(y++); \
            block4 = _mm256_loadu_si256(y++); \
            block5 = _mm256_loadu_si256(y++); \
            block6 = _mm256_loadu_si256(y++); \
            block7 = _mm256_loadu_si256(y++); \
            block8 = _mm256_loadu_si256(y++); \
            _mm256_stream_si256(x++, block1); \
            _mm256_stream_si256(x++, block2); \
            _mm256_stream_si256(x++, block3); \
            _mm256_stream_si256(x++, block4); \
            _mm256_stream_si256(x++, block5); \
            _mm256_stream_si256(x++, block6); \
            _mm256_stream_si256(x++, block7); \
            _mm256_stream_si256(x++, block8); \
        }                                     \
                                              \
        while (s--)                           \
        {                                     \
            block1 = _mm256_loadu_si256(y++); \
            _mm256_store_si256(x++, block1);  \
        }                                     \
    } while (0)

#elif defined(__SSE2__)

typedef __m128i mrfstr_memcpy_simd_t;
#define MRFSTR_MEMCPY_SIMD_SIZE 16
#define MRFSTR_MEMCPY_SIMD_SHIFT 4

#define mrfstr_memcpy_sub(x, y, s)              \
    do                                          \
    {                                           \
        mrfstr_memcpy_simd_t                    \
            block01, block02, block03, block04, \
            block05, block06, block07, block08, \
            block09, block10, block11, block12, \
            block13, block14, block15, block16; \
                                                \
        for (; s >= 16; s -= 16)                \
        {                                       \
            block01 = _mm_loadu_si128(y++);     \
            block02 = _mm_loadu_si128(y++);     \
            block03 = _mm_loadu_si128(y++);     \
            block04 = _mm_loadu_si128(y++);     \
            block05 = _mm_loadu_si128(y++);     \
            block06 = _mm_loadu_si128(y++);     \
            block07 = _mm_loadu_si128(y++);     \
            block08 = _mm_loadu_si128(y++);     \
            block09 = _mm_loadu_si128(y++);     \
            block10 = _mm_loadu_si128(y++);     \
            block11 = _mm_loadu_si128(y++);     \
            block12 = _mm_loadu_si128(y++);     \
            block13 = _mm_loadu_si128(y++);     \
            block14 = _mm_loadu_si128(y++);     \
            block15 = _mm_loadu_si128(y++);     \
            block16 = _mm_loadu_si128(y++);     \
            _mm_stream_si128(x++, block01);     \
            _mm_stream_si128(x++, block02);     \
            _mm_stream_si128(x++, block03);     \
            _mm_stream_si128(x++, block04);     \
            _mm_stream_si128(x++, block05);     \
            _mm_stream_si128(x++, block06);     \
            _mm_stream_si128(x++, block07);     \
            _mm_stream_si128(x++, block08);     \
            _mm_stream_si128(x++, block09);     \
            _mm_stream_si128(x++, block10);     \
            _mm_stream_si128(x++, block11);     \
            _mm_stream_si128(x++, block12);     \
            _mm_stream_si128(x++, block13);     \
            _mm_stream_si128(x++, block14);     \
            _mm_stream_si128(x++, block15);     \
            _mm_stream_si128(x++, block16);     \
        }                                       \
                                                \
        while (s--)                             \
        {                                       \
            block01 = _mm_loadu_si128(y++);     \
            _mm_store_si128(x++, block01);      \
        }                                       \
    } while (0)

#else
#define MRFSTR_MEMCPY_NOSIMD

typedef mrfstr_chr_t mrfstr_memcpy_simd_t;
#define MRFSTR_MEMCPY_SIMD_SIZE 1

#endif

#ifdef MRFSTR_MEMCPY_NOSIMD
#define MRFSTR_MEMCPY_SLIMIT (0x800 - 1)
#else
#define MRFSTR_MEMCPY_SIMD_MASK (MRFSTR_MEMCPY_SIMD_SIZE - 1)
#define MRFSTR_MEMCPY_SLIMIT (0x100 * MRFSTR_MEMCPY_SIMD_SIZE - 1)
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
    if (size <= MRFSTR_MEMCPY_SLIMIT)
    {
        memcpy(dst, src, size);
        return;
    }

#ifndef MRFSTR_MEMCPY_NOSIMD
    mrfstr_bit_t align = (uintptr_t)dst & MRFSTR_MEMCPY_SIMD_MASK;
    if (align)
    {
        align = MRFSTR_MEMCPY_SIMD_SIZE - align;
        memcpy(dst, src, align);

        dst += align;
        src += align;
        size -= align;
    }
#endif

#if !defined(MRFSTR_MEMCPY_NOSIMD) || defined(MRFSTR_THREADING)
    mrfstr_memcpy_simd_t *dblock = (mrfstr_memcpy_simd_t*)dst;
    mrfstr_memcpy_simd_t *sblock = (mrfstr_memcpy_simd_t*)src;
#endif

#if MRFSTR_THREADING
    if (size <= MRFSTR_MEMCPY_TLIMIT)
    {
#endif
#ifdef MRFSTR_MEMCPY_NOSIMD
        memcpy(dst, src, size);
#else
        mrfstr_bit_t rem = size & MRFSTR_MEMCPY_SIMD_MASK;
        size >>= MRFSTR_MEMCPY_SIMD_SHIFT;

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
    mrfstr_memcpy_sub(data->dst, data->src, data->size);
#endif

    mrstr_free(data);
    return NULL;
}
#endif
