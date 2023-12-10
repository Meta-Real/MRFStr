/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifdef __AVX512F__

typedef __m512i mrtstr_memcpy_simd_t;
#define MRTSTR_MEMCPY_SIMD_SIZE 64
#define MRTSTR_MEMCPY_SIMD_SHIFT 6

#define mrtstr_memcpy_define mrtstr_memcpy_simd_t \
    block1, block2, block3, block4

#define mrtstr_memcpy_proc(x, y)          \
    do                                    \
    {                                     \
        block1 = _mm512_loadu_si512(y++); \
        block2 = _mm512_loadu_si512(y++); \
        block3 = _mm512_loadu_si512(y++); \
        block4 = _mm512_loadu_si512(y++); \
        _mm512_stream_si512(x++, block1); \
        _mm512_stream_si512(x++, block2); \
        _mm512_stream_si512(x++, block3); \
        _mm512_stream_si512(x++, block4); \
    } while (0)

#define mrtstr_memcpy_sub(x, y, s)            \
    do                                        \
    {                                         \
        for (; s >= 4; s -= 4)                \
            mrtstr_memcpy_proc(x, y);         \
                                              \
        while (s--)                           \
        {                                     \
            block1 = _mm512_loadu_si512(y++); \
            _mm512_stream_si512(x++, block1); \
        }                                     \
    } while (0)

#define mrtstr_memcpy_waitsub                                     \
    while (!mrtstr_threads.free_threads && data->size >= 0x10000) \
    {                                                             \
        for (j = 0; j < 0x10000; j += 4)                          \
            mrtstr_memcpy_proc(sblock, dblock);                   \
                                                                  \
        data->size -= 0x10000;                                    \
    }

#elif defined(__AVX__)

typedef __m256i mrtstr_memcpy_simd_t;
#define MRTSTR_MEMCPY_SIMD_SIZE 32
#define MRTSTR_MEMCPY_SIMD_SHIFT 5

#define mrtstr_memcpy_define mrtstr_memcpy_simd_t \
    block1, block2, block3, block4, \
    block5, block6, block7, block8

#define mrtstr_memcpy_proc(x, y)          \
    do                                    \
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
    } while (0)

#define mrtstr_memcpy_sub(x, y, s)            \
    do                                        \
    {                                         \
        for (; s >= 8; s -= 8)                \
            mrtstr_memcpy_proc(x, y);         \
                                              \
        while (s--)                           \
        {                                     \
            block1 = _mm256_loadu_si256(y++); \
            _mm256_stream_si256(x++, block1); \
        }                                     \
    } while (0)

#define mrtstr_memcpy_waitsub                                     \
    while (!mrtstr_threads.free_threads && data->size >= 0x10000) \
    {                                                             \
        for (j = 0; j < 0x10000; j += 8)                          \
            mrtstr_memcpy_proc(sblock, dblock);                   \
                                                                  \
        data->size -= 0x10000;                                    \
    }

#elif defined(__SSE2__)

typedef __m128i mrtstr_memcpy_simd_t;
#define MRTSTR_MEMCPY_SIMD_SIZE 16
#define MRTSTR_MEMCPY_SIMD_SHIFT 4

#define mrtstr_memcpy_define mrtstr_memcpy_simd_t \
    block01, block02, block03, block04, \
    block05, block06, block07, block08, \
    block09, block10, block11, block12, \
    block13, block14, block15, block16

#define mrtstr_memcpy_proc(x, y)        \
    do                                  \
    {                                   \
        block01 = _mm_loadu_si128(y++); \
        block02 = _mm_loadu_si128(y++); \
        block03 = _mm_loadu_si128(y++); \
        block04 = _mm_loadu_si128(y++); \
        block05 = _mm_loadu_si128(y++); \
        block06 = _mm_loadu_si128(y++); \
        block07 = _mm_loadu_si128(y++); \
        block08 = _mm_loadu_si128(y++); \
        block09 = _mm_loadu_si128(y++); \
        block10 = _mm_loadu_si128(y++); \
        block11 = _mm_loadu_si128(y++); \
        block12 = _mm_loadu_si128(y++); \
        block13 = _mm_loadu_si128(y++); \
        block14 = _mm_loadu_si128(y++); \
        block15 = _mm_loadu_si128(y++); \
        block16 = _mm_loadu_si128(y++); \
        _mm_stream_si128(x++, block01); \
        _mm_stream_si128(x++, block02); \
        _mm_stream_si128(x++, block03); \
        _mm_stream_si128(x++, block04); \
        _mm_stream_si128(x++, block05); \
        _mm_stream_si128(x++, block06); \
        _mm_stream_si128(x++, block07); \
        _mm_stream_si128(x++, block08); \
        _mm_stream_si128(x++, block09); \
        _mm_stream_si128(x++, block10); \
        _mm_stream_si128(x++, block11); \
        _mm_stream_si128(x++, block12); \
        _mm_stream_si128(x++, block13); \
        _mm_stream_si128(x++, block14); \
        _mm_stream_si128(x++, block15); \
        _mm_stream_si128(x++, block16); \
    } while (0)

#define mrtstr_memcpy_sub(x, y, s)          \
    do                                      \
    {                                       \
        for (; s >= 16; s -= 16)            \
            mrtstr_memcpy_proc(x, y);       \
                                            \
        while (s--)                         \
        {                                   \
            block01 = _mm_loadu_si128(y++); \
            _mm_stream_si128(x++, block01); \
        }                                   \
    } while (0)

#define mrtstr_memcpy_waitsub                                     \
    while (!mrtstr_threads.free_threads && data->size >= 0x10000) \
    {                                                             \
        for (j = 0; j < 0x10000; j += 16)                         \
            mrtstr_memcpy_proc(sblock, dblock);                   \
                                                                  \
        data->size -= 0x10000;                                    \
    }

#else
#define MRTSTR_MEMCPY_NOSIMD

typedef mrtstr_chr_t mrtstr_memcpy_simd_t;
#define MRTSTR_MEMCPY_SIMD_SIZE 1

#define mrtstr_memcpy_sub memcpy
#define mrtstr_memcpy_waitsub                                     \
    while (!mrtstr_threads.free_threads && data->size >= 0x10000) \
    {                                                             \
        memcpy(sblock, dblock, 0x10000);                          \
        sblock += 0x10000;                                        \
        dblock += 0x10000;                                        \
        data->size -= 0x10000;                                    \
    }

#endif

#ifndef MRTSTR_MEMCPY_NOSIMD
#define MRTSTR_MEMCPY_SIMD_MASK (MRTSTR_MEMCPY_SIMD_SIZE - 1)
#endif

#define MRTSTR_MEMCPY_TCHK (MRTSTR_MEMCPY_SIMD_SIZE * MRTSTR_THREAD_COUNT)
#define MRTSTR_MEMCPY_TLIMIT (0x10000 * MRTSTR_MEMCPY_TCHK - 1)

struct __MRTSTR_MEMCPY_T
{
    mrtstr_memcpy_simd_t *src;
    mrtstr_memcpy_simd_t *dst;
    mrtstr_size_t size;

    mrtstr_lock_t *slock;
    pthread_mutex_t *smutex;

    mrtstr_lock_t *dlock;
};
typedef struct __MRTSTR_MEMCPY_T *mrtstr_memcpy_t;

struct __MRTSTR_MEMCPY2_T
{
    mrtstr_memcpy_simd_t *src;
    mrtstr_memcpy_simd_t *dst;
    mrtstr_size_t size;

    mrtstr_lock_t *lock;
};
typedef struct __MRTSTR_MEMCPY2_T *mrtstr_memcpy2_t;

void mrtstr_memcpy_threaded(void *args);
void mrtstr_memcpy2_threaded(void *args);

void mrtstr_memcpy(mrtstr_t dst, mrtstr_ct src, mrtstr_size_t size)
{
    mrtstr_memcpy_simd_t *dblock = (mrtstr_memcpy_simd_t*)dst->data;
    mrtstr_memcpy_simd_t *sblock = (mrtstr_memcpy_simd_t*)src->data;

    if (size <= MRTSTR_MEMCPY_TLIMIT)
    {
#ifdef MRTSTR_MEMCPY_NOSIMD
        memcpy(dst->data, src->data, size);
#else
        mrtstr_size_t rem = size & MRTSTR_MEMCPY_SIMD_MASK;
        size >>= MRTSTR_MEMCPY_SIMD_SHIFT;

        mrtstr_memcpy_define;
        mrtstr_memcpy_sub(dblock, sblock, size);
        memcpy(dblock, sblock, rem);
#endif
        return;
    }

    mrtstr_size_t rem = size % MRTSTR_MEMCPY_TCHK;
    size /= MRTSTR_MEMCPY_TCHK;

    mrtstr_bit_t i;
#ifndef MRTSTR_MEMCPY_NOSIMD
    mrtstr_size_t j;
    mrtstr_memcpy_define;
#endif
    mrtstr_memcpy_t data;
    if (mrtstr_locked(src) && src->forced)
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            for (; src->lock[i];);

            data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCPY_T));
            if (!data)
                goto rem;

            data->size = size;
            data->slock = src->lock + i;
            data->smutex = &src->mutex;
            data->dlock = dst->lock + i;

            mrtstr_memcpy_waitsub;

            if (data->size <= 0x10000)
            {
                mrtstr_memcpy_sub(dblock, sblock, data->size);
                mrstr_free(data);
            }
            else
            {
                data->src = sblock;
                data->dst = dblock;

                sblock += data->size;
                dblock += data->size;

                src->lock[i] = 1;
                dst->lock[i] = 1;
                mrtstr_load_threads(mrtstr_memcpy_threaded, data);
            }
        }
    else
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCPY_T));
            if (!data)
                goto rem2;

            data->size = size;
            data->slock = src->lock + i;
            data->smutex = &src->mutex;
            data->dlock = dst->lock + i;

            mrtstr_memcpy_waitsub;

            if (data->size < 0x10000)
            {
                mrtstr_memcpy_sub(dblock, sblock, data->size);
                mrstr_free(data);
            }
            else
            {
                data->src = sblock;
                data->dst = dblock;

                sblock += data->size;
                dblock += data->size;

                mrtstr_lock_inc(src->lock[i], &src->mutex);
                dst->lock[i] = 1;
                mrtstr_load_threads(mrtstr_memcpy_threaded, data);
            }
        }

ret:
    memcpy(sblock, dblock, rem);

    src->forced = MRTSTR_FALSE;
    dst->forced = MRTSTR_TRUE;
    return;

rem:
#ifdef MRTSTR_MEMCPY_NOSIMD
    memcpy(dblock, sblock, size);
#else
    mrtstr_memcpy_sub(dblock, sblock, size);
#endif

    for (; ++i < MRTSTR_THREAD_COUNT;)
    {
        for (; src->lock[i];);
#ifdef MRTSTR_MEMCPY_NOSIMD
        memcpy(dblock, sblock, size);
#else
        mrtstr_memcpy_sub(dblock, sblock, size);
#endif
    }

    goto ret;

rem2:
    size *= MRTSTR_THREAD_COUNT - i;

#ifdef MRTSTR_MEMCPY_NOSIMD
    memcpy(dblock, sblock, size);
#else
    mrtstr_memcpy_sub(dblock, sblock, size);
#endif

    goto ret;
}

void mrtstr_memcpy2(mrtstr_t dst, mrtstr_data_ct src, mrtstr_size_t size)
{
    mrtstr_memcpy_simd_t *dblock = (mrtstr_memcpy_simd_t*)dst->data;
    mrtstr_memcpy_simd_t *sblock = (mrtstr_memcpy_simd_t*)src;

    if (size <= MRTSTR_MEMCPY_TLIMIT)
    {
#ifdef MRTSTR_MEMCPY_NOSIMD
        memcpy(dst->data, src, size);
#else
        mrtstr_size_t rem = size & MRTSTR_MEMCPY_SIMD_MASK;
        size >>= MRTSTR_MEMCPY_SIMD_SHIFT;

        mrtstr_memcpy_define;
        mrtstr_memcpy_sub(dblock, sblock, size);
        memcpy(dblock, sblock, rem);
#endif
        return;
    }

    mrtstr_size_t rem = size % MRTSTR_MEMCPY_TCHK;
    size /= MRTSTR_MEMCPY_TCHK;

    mrtstr_bit_t i;
#ifndef MRTSTR_MEMCPY_NOSIMD
    mrtstr_size_t j;
    mrtstr_memcpy_define;
#endif
    mrtstr_memcpy2_t data;
    for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRTSTR_MEMCPY2_T));
        if (!data)
            goto rem;

        data->size = size;
        data->lock = dst->lock + i;

        mrtstr_memcpy_waitsub;

        if (data->size < 0x10000)
        {
            mrtstr_memcpy_sub(dblock, sblock, data->size);
            mrstr_free(data);
        }
        else
        {
            data->src = sblock;
            data->dst = dblock;

            sblock += data->size;
            dblock += data->size;

            dst->lock[i] = 1;
            mrtstr_load_threads(mrtstr_memcpy2_threaded, data);
        }
    }

ret:
    memcpy(sblock, dblock, rem);

    dst->forced = MRTSTR_TRUE;
    return;

rem:
    size *= MRTSTR_THREAD_COUNT - i;

#ifdef MRTSTR_MEMCPY_NOSIMD
    memcpy(dblock, sblock, size);
#else
    mrtstr_memcpy_sub(dblock, sblock, size);
#endif

    goto ret;
}

void mrtstr_memcpy_threaded(void *args)
{
    mrtstr_memcpy_t data = (mrtstr_memcpy_t)args;

#ifdef MRTSTR_MEMCPY_NOSIMD
    memcpy(data->dst, data->src, data->size);
#else
    mrtstr_memcpy_define;
    mrtstr_memcpy_sub(data->dst, data->src, data->size);
#endif

    mrtstr_lock_dec(*data->slock, data->smutex);
    *data->dlock = 0;
    mrstr_free(data);
}

void mrtstr_memcpy2_threaded(void *args)
{
    mrtstr_memcpy2_t data = (mrtstr_memcpy2_t)args;

#ifdef MRTSTR_MEMCPY_NOSIMD
    memcpy(data->dst, data->src, data->size);
#else
    mrtstr_memcpy_define;
    mrtstr_memcpy_sub(data->dst, data->src, data->size);
#endif

    *data->lock = 0;
    mrstr_free(data);
}
