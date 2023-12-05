/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
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

#define mrfstr_rev_perm(x, y) x = _mm512_permutexvar_epi8(revidx, _mm512_stream_load_si512(y))
#define mrfstr_rev_permu(x, y) x = _mm512_permutexvar_epi8(revidx, _mm512_loadu_si512(y))
#define mrfstr_rev_store _mm512_stream_si512
#define mrfstr_rev_storeu _mm512_storeu_si512

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

#define mrfstr_rev_perm(x, y)                                          \
    do                                                                 \
    {                                                                  \
        x = _mm512_shuffle_epi8(_mm512_stream_load_si512(y), revidx1); \
        x = _mm512_permutexvar_epi64(revidx2, x);                      \
    } while (0)
#define mrfstr_rev_permu(x, y)                                   \
    do                                                           \
    {                                                            \
        x = _mm512_shuffle_epi8(_mm512_loadu_si512(y), revidx1); \
        x = _mm512_permutexvar_epi64(revidx2, x);                \
    } while (0)

#define mrfstr_rev_store _mm512_stream_si512
#define mrfstr_rev_storeu _mm512_storeu_si512

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 64

#define mrfstr_init_revidx                      \
    const __m256i revidx = _mm256_set_epi64x(   \
        0x0001020304050607, 0x08090a0b0c0d0e0f, \
        0x0001020304050607, 0x08090a0b0c0d0e0f)

#define mrfstr_rev_perm(x, y)                                         \
    do                                                                \
    {                                                                 \
        x = _mm256_shuffle_epi8(_mm256_stream_load_si256(y), revidx); \
        x = _mm256_permute2x128_si256(x, x, 1);                       \
    } while (0)
#define mrfstr_rev_permu(x, y)                                  \
    do                                                          \
    {                                                           \
        x = _mm256_shuffle_epi8(_mm256_loadu_si256(y), revidx); \
        x = _mm256_permute2x128_si256(x, x, 1);                 \
    } while (0)

#define mrfstr_rev_store _mm256_store_si256
#define mrfstr_rev_storeu _mm256_storeu_si256

#elif defined(__SSE2__) && defined(__SSSE3__)

typedef __m128i mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 32

#define mrfstr_init_revidx                \
    const __m128i revidx = _mm_set_epi32( \
        0x00010203, 0x04050607,           \
        0x08090a0b, 0x0c0d0e0f)

#define mrfstr_rev_perm(x, y) x = _mm_shuffle_epi8(_mm_stream_load_si128(y), revidx)
#define mrfstr_rev_permu(x, y) x = _mm_shuffle_epi8(_mm_loadu_si128(y), revidx)

#define mrfstr_rev_store _mm_store_si128
#define mrfstr_rev_storeu _mm_storeu_si128

#else

typedef unsigned long long mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 16

#define mrfstr_init_revidx

#define mrfstr_rev_perm(x, y) x = _bswap64(*y)
#define mrfstr_rev_permu mrfstr_rev_perm

#define mrfstr_rev_store(x, y) *x = y
#define mrfstr_rev_storeu mrfstr_rev_store

#endif

#define MRFSTR_REV2_SIMD_SIZE (MRFSTR_REV_SIMD_SIZE >> 1)

#if MRFSTR_THREADING
#include <pthread.h>

#define MRFSTR_REV_TCHK (MRFSTR_REV_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_REV_TLIMIT (65536 * MRFSTR_REV_TCHK - 1)

#define MRFSTR_REV2_TCHK (MRFSTR_REV2_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_REV2_TLIMIT (65536 * MRFSTR_REV2_TCHK - 1)

struct __MRFSTR_REV_T
{
    mrfstr_rev_simd_t *left;
    mrfstr_rev_simd_t *right;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_REV_T *mrfstr_rev_t;

void *mrfstr_rev_threaded(void *args);
void *mrfstr_rev2_threaded(void *args);
#endif

mrfstr_res_enum_t mrfstr_reverse(mrfstr_t res, mrfstr_ct str)
{
    if (res->alloc && res->data == str->data)
    {
        if (res->size <= 1)
            return MRFSTR_RES_NOERROR;

        if (res->size < MRFSTR_REV_SIMD_SIZE)
        {
            strrev(res->data);
            return MRFSTR_RES_NOERROR;
        }

        mrfstr_rev_simd_t *lblock = (mrfstr_rev_simd_t*)res->data;
        mrfstr_rev_simd_t *rblock = (mrfstr_rev_simd_t*)(res->data + res->size);

#if MRFSTR_THREADING
        if (res->size <= MRFSTR_REV_TLIMIT)
        {
#endif
            mrfstr_init_revidx;

            mrfstr_rev_simd_t left, right;
            for (; --rblock > lblock; lblock++)
            {
                mrfstr_rev_perm(left, lblock);
                mrfstr_rev_permu(right, rblock);

                mrfstr_rev_store(lblock, right);
                mrfstr_rev_storeu(rblock, left);
            }

            rblock++;
            if (lblock == rblock)
                return MRFSTR_RES_NOERROR;

            mrfstr_data_t lptr = (mrfstr_data_t)lblock;
            mrfstr_data_t rptr = (mrfstr_data_t)rblock;

            mrfstr_chr_t chr;
            for (; lptr < rptr; lptr++)
            {
                chr = *lptr;
                *lptr = *--rptr;
                *rptr = chr;
            }

            return MRFSTR_RES_NOERROR;
#if MRFSTR_THREADING
        }

        mrfstr_size_t size = res->size / MRFSTR_REV_TCHK;

        pthread_t threads[MRFSTR_THREAD_COUNT];
        mrfstr_bit_t i;
        mrfstr_rev_t data;
        for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        {
            data = mrstr_alloc(sizeof(struct __MRFSTR_REV_T));
            if (!data)
                goto rem;

            data->left = lblock;
            data->right = rblock;
            data->size = size;

            lblock += size;
            rblock -= size;

            if (pthread_create(threads + i, NULL, mrfstr_rev_threaded, data))
            {
                lblock -= size;
                rblock += size;

                mrstr_free(data);
                goto rem;
            }
        }

ret:
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

        while (i)
            pthread_join(threads[--i], NULL);
        return MRFSTR_RES_NOERROR;

rem:
        size *= MRFSTR_THREAD_COUNT - i;
        mrfstr_init_revidx;

        mrfstr_rev_simd_t left, right;
        for (; --rblock > lblock; lblock++)
        {
            mrfstr_rev_perm(left, lblock);
            mrfstr_rev_permu(right, rblock);

            mrfstr_rev_store(lblock, right);
            mrfstr_rev_storeu(rblock, left);
        }

        rblock++;
        goto ret;
#endif
    }

    res->size = str->size;
    if (!str->size)
        return MRFSTR_RES_NOERROR;

    if (res->alloc <= str->size)
    {
        if (res->alloc && res->data != str->data)
            mrstr_aligned_free(res->data);

        res->alloc = str->size + 1;
        res->data = mrstr_aligned_alloc(res->alloc, MRFSTR_SIMD_SIZE);
        if (!res->data)
            return MRFSTR_RES_MEM_ERROR;
    }

    if (str->size < MRFSTR_REV2_SIMD_SIZE)
    {
        mrfstr_data_t rptr = res->data;
        mrfstr_data_t sptr = str->data + str->size;

        for (; sptr > str->data; rptr++)
            *rptr = *--sptr;

        res->data[res->size] = '\0';
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_rev_simd_t *rblock = (mrfstr_rev_simd_t*)res->data;
    mrfstr_rev_simd_t *sblock = (mrfstr_rev_simd_t*)(str->data + str->size);

#if MRFSTR_THREADING
    if (str->size <= MRFSTR_REV2_TLIMIT)
    {
#endif
        mrfstr_init_revidx;

        mrfstr_rev_simd_t block;
        for (; --sblock > (mrfstr_rev_simd_t*)str->data; rblock++)
        {
            mrfstr_rev_permu(block, sblock);
            mrfstr_rev_store(rblock, block);
        }

        sblock++;
        res->data[res->size] = '\0';
        if ((mrfstr_data_t)sblock == str->data)
            return MRFSTR_RES_NOERROR;

        mrfstr_data_t rptr = (mrfstr_data_t)rblock;
        mrfstr_data_t sptr = (mrfstr_data_t)sblock;

        for (; sptr > str->data; rptr++)
            *rptr = *--sptr;

        return MRFSTR_RES_NOERROR;
#if MRFSTR_THREADING
    }

    mrfstr_size_t size = res->size / MRFSTR_REV2_TCHK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_rev_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_REV_T));
        if (!data)
            goto rem2;

        data->left = rblock;
        data->right = sblock;
        data->size = size;

        rblock += size;
        sblock -= size;

        if (pthread_create(threads + i, NULL, mrfstr_rev2_threaded, data))
        {
            rblock -= size;
            sblock += size;

            mrstr_free(data);
            goto rem2;
        }
    }

ret2:
    if (rblock != sblock)
    {
        mrfstr_data_t rptr = (mrfstr_data_t)rblock;
        mrfstr_data_t sptr = (mrfstr_data_t)sblock;

        for (; sptr > str->data; rptr++)
            *rptr = *--sptr;
    }

    res->data[res->size] = '\0';

    while (i)
        pthread_join(threads[--i], NULL);
    return MRFSTR_RES_NOERROR;

rem2:
    mrfstr_init_revidx;

    mrfstr_rev_simd_t block;
    for (; --sblock > (mrfstr_rev_simd_t*)str->data; rblock++)
    {
        mrfstr_rev_permu(block, sblock);
        mrfstr_rev_store(rblock, block);
    }

    sblock++;
    goto ret2;
#endif
}

#if MRFSTR_THREADING
void *mrfstr_rev_threaded(void *args)
{
    mrfstr_rev_t data = (mrfstr_rev_t)args;
    mrfstr_init_revidx;

    mrfstr_rev_simd_t left, right;
    for (; data->size; data->size--, data->left++)
    {
        data->right--;

        mrfstr_rev_perm(left, data->left);
        mrfstr_rev_permu(right, data->right);

        mrfstr_rev_store(data->left, right);
        mrfstr_rev_storeu(data->right, left);
    }

    mrstr_free(data);
    return NULL;
}

void *mrfstr_rev2_threaded(void *args)
{
    mrfstr_rev_t data = (mrfstr_rev_t)args;
    mrfstr_init_revidx;

    mrfstr_rev_simd_t block;
    for (; data->size; data->size--, data->left++)
    {
        mrfstr_rev_permu(block, --data->right);
        mrfstr_rev_store(data->left, block);
    }

    mrstr_free(data);
    return NULL;
}
#endif
