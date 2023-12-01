/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <pthread.h>
#include <alloc.h>
#include <string.h>

#if defined(__AVX512F__) && defined(__AVX512VBMI__)

typedef __m512i mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 128

#define mrfstr_init_revidx                   \
    const __m512i revidx = _mm512_set_epi64( \
    0x0001020304050607, 0x08090a0b0c0d0e0f,  \
    0x1011121314151617, 0x18191a1b1c1d1e1f,  \
    0x2021222324252627, 0x28292a2b2c2d2e2f,  \
    0x3031323334353637, 0x38393a3b3c3d3e3f)

#define mrfstr_rev_inst(x, y) x = _mm512_permutexvar_epi8(revidx, _mm512_stream_load_si512(y))
#define mrfstr_rev_store(x, y) _mm512_stream_si512(x, y)

#elif defined(__AVX512F__) && defined(__AVX512BW__)

typedef __m512i mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 128

#define mrfstr_init_revidx                                           \
    const __m512i revidx1 = _mm512_set_epi64(                        \
        0x0001020304050607, 0x08090a0b0c0d0e0f,                      \
        0x0001020304050607, 0x08090a0b0c0d0e0f,                      \
        0x0001020304050607, 0x08090a0b0c0d0e0f,                      \
        0x0001020304050607, 0x08090a0b0c0d0e0f);                     \
                                                                     \
    const __m512i revidx2 = _mm512_set_epi64(1, 0, 3, 2, 5, 4, 7, 6) \

#define mrfstr_rev_inst(x, y)                                          \
    do                                                                 \
    {                                                                  \
        x = _mm512_shuffle_epi8(_mm512_stream_load_si512(y), revidx1); \
        x = _mm512_permutexvar_epi64(revidx2, x);                      \
    } while (0)
#define mrfstr_rev_store(x, y) _mm512_stream_si512(x, y)

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 64

#define mrfstr_init_revidx                      \
    const __m256i revidx = _mm256_set_epi64x(   \
        0x0001020304050607, 0x08090a0b0c0d0e0f, \
        0x0001020304050607, 0x08090a0b0c0d0e0f)

#define mrfstr_rev_inst(x, y)                                         \
    do                                                                \
    {                                                                 \
        x = _mm256_shuffle_epi8(_mm256_stream_load_si256(y), revidx); \
        x = _mm256_permute2x128_si256(x, x, 1);                       \
    } while (0)
#define mrfstr_rev_store(x, y) _mm256_stream_si256(x, y)

#elif defined(__SSE2__) && defined(__SSSE3__)

typedef __m128i mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 32

#define mrfstr_init_revidx                \
    const __m128i revidx = _mm_set_epi32( \
        0x00010203, 0x04050607,           \
        0x08090a0b, 0x0c0d0e0f)

#define mrfstr_rev_inst(x, y) x = _mm_shuffle_epi8(_mm_stream_load_si128(y), revidx)
#define mrfstr_rev_store(x, y) _mm_stream_store_si128(x, y)

#else

typedef unsigned long long mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 16

#define mrfstr_init_revidx

#define mrfstr_rev_inst(x, y) x = _bswap64(*y)
#define mrfstr_rev_store(x, y) *x = y

#endif

#define MRFSTR_REV_TLIMIT (65536 * MRFSTR_REV_SIMD_SIZE * MRFSTR_THREAD_COUNT - 1)
#define MRFSTR_REV_TCHK (MRFSTR_REV_SIMD_SIZE * MRFSTR_THREAD_COUNT)

struct __MRFSTR_REV_T
{
    mrfstr_rev_simd_t *left;
    mrfstr_rev_simd_t *right;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_REV_T *mrfstr_rev_t;

void *mrfstr_rev_threaded(void *args);

void mrfstr_reverse(mrfstr_t res, mrfstr_ct str)
{
    if (res == str)
    {
        if (res->size <= 1)
            return;

        if (res->size < MRFSTR_REV_SIMD_SIZE)
        {
            strrev(res->data);
            return;
        }

        mrfstr_rev_simd_t *lblock = (mrfstr_rev_simd_t*)res->data;
        mrfstr_rev_simd_t *rblock = (mrfstr_rev_simd_t*)(res->data + res->size);

        if (res->size <= MRFSTR_REV_TLIMIT)
        {
            mrfstr_init_revidx;

            mrfstr_rev_simd_t left, right;
            for (; rblock - lblock >= 2; lblock++)
            {
                rblock--;

                mrfstr_rev_inst(left, lblock);
                mrfstr_rev_inst(right, rblock);

                mrfstr_rev_store(lblock, right);
                mrfstr_rev_store(rblock, left);
            }

            if (lblock == rblock)
                return;

            mrfstr_data_t lptr = (mrfstr_data_t)lblock;
            mrfstr_data_t rptr = (mrfstr_data_t)rblock;

            mrfstr_chr_t chr;
            for (; lptr < rptr; lptr++)
            {
                chr = *lptr;
                *lptr = *--rptr;
                *rptr = chr;
            }

            return;
        }

        mrfstr_size_t size = res->size / MRFSTR_REV_TCHK;

        pthread_t threads[MRFSTR_THREAD_COUNT];
        mrfstr_bit_t i;
        mrfstr_rev_t data;
        for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        {
            data = mrstr_alloc(sizeof(struct __MRFSTR_REV_T));
            data->left = lblock;
            data->right = rblock;
            data->size = size;

            lblock += size;
            rblock -= size;

            pthread_create(threads + i, NULL, mrfstr_rev_threaded, data);
        }

        if (lblock != rblock)
        {
            mrfstr_data_t lptr = (mrfstr_data_t)lblock;
            mrfstr_data_t rptr = (mrfstr_data_t)rblock;

            mrfstr_chr_t chr;
            for (; lptr < rptr; lptr++)
            {
                chr = *lptr;
                *lptr = *--rptr;
                *rptr = chr;
            }
        }

        for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
            pthread_join(threads[i], NULL);

        return;
    }
}

void *mrfstr_rev_threaded(void *args)
{
    mrfstr_rev_t data = (mrfstr_rev_t)args;

    mrfstr_init_revidx;

    mrfstr_rev_simd_t left, right;
    for (; data->size; data->size--, data->left++)
    {
        data->right--;

        mrfstr_rev_inst(left, data->left);
        mrfstr_rev_inst(right, data->right);

        mrfstr_rev_store(data->left, right);
        mrfstr_rev_store(data->right, left);
    }

    mrstr_free(data);
    return NULL;
}
