/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#if MRFSTR_SIMD_SIZE == 64 && defined(__AVX512VBMI__)

typedef __m512i mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 128

#define mrfstr_init_revidx                      \
    const __m512i revidx = _mm512_set_epi64(    \
        0x0001020304050607, 0x08090a0b0c0d0e0f, \
        0x1011121314151617, 0x18191a1b1c1d1e1f, \
        0x2021222324252627, 0x28292a2b2c2d2e2f, \
        0x3031323334353637, 0x38393a3b3c3d3e3f)

#define mrfstr_rev_perm(x, y) \
    ((x) = _mm512_permutexvar_epi8(revidx, _mm512_loadu_si512(y)))
#define mrfstr_rev_storeu _mm512_storeu_si512

#elif MRFSTR_SIMD_SIZE == 64 && defined(__AVX512BW__)

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

#define mrfstr_rev_perm(x, y)                                      \
    do                                                             \
    {                                                              \
        (x) = _mm512_shuffle_epi8(_mm512_loadu_si512(y), revidx1); \
        (x) = _mm512_permutexvar_epi64(revidx2, (x));              \
    } while (0)

#define mrfstr_rev_storeu _mm512_storeu_si512

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 64

#define mrfstr_init_revidx                      \
    const __m256i revidx = _mm256_set_epi64x(   \
        0x0001020304050607, 0x08090a0b0c0d0e0f, \
        0x0001020304050607, 0x08090a0b0c0d0e0f)

#define mrfstr_rev_perm(x, y)                                     \
    do                                                            \
    {                                                             \
        (x) = _mm256_shuffle_epi8(_mm256_loadu_si256(y), revidx); \
        (x) = _mm256_permute2x128_si256((x), (x), 1);             \
    } while (0)

#define mrfstr_rev_storeu _mm256_storeu_si256

#elif defined(__SSE2__) && defined(__SSSE3__)

typedef __m128i mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 32

#define mrfstr_init_revidx                \
    const __m128i revidx = _mm_set_epi32( \
        0x00010203, 0x04050607,           \
        0x08090a0b, 0x0c0d0e0f)

#define mrfstr_rev_perm(x, y) ((x) = _mm_shuffle_epi8(_mm_loadu_si128(y), revidx))
#define mrfstr_rev_storeu _mm_storeu_si128

#else

typedef unsigned long long mrfstr_rev_simd_t;
#define MRFSTR_REV_SIMD_SIZE 16

#define mrfstr_init_revidx

#if defined(unix) || defined(__unix) || defined(__unix__)
#define mrfstr_rev_perm(x, y) ((x) = _bswap64(*(y)))
#elif defined(_WIN32)
#include <stdlib.h>
#define mrfstr_rev_perm(x, y) ((x) = _byteswap_uint64(*(y)))
#endif

#define mrfstr_rev_storeu(x, y) (*(x) = (y))

#endif

#define MRFSTR_REV_SLIMIT (0x100 * MRFSTR_REV_SIMD_SIZE)

#define MRFSTR_REV2_SIMD_SIZE (MRFSTR_REV_SIMD_SIZE >> 1)
#define MRFSTR_REV2_SLIMIT (0x100 * MRFSTR_REV2_SIMD_SIZE)

#if MRFSTR_THREADING

#define MRFSTR_REV_TCHK (MRFSTR_REV_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_REV_TLIMIT (0x10000 * MRFSTR_REV_TCHK)

#define MRFSTR_REV2_TCHK (MRFSTR_REV2_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_REV2_TLIMIT (0x10000 * MRFSTR_REV2_TCHK)

struct __MRFSTR_REV_T
{
    mrfstr_rev_simd_t *left;
    mrfstr_rev_simd_t *right;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_REV_T *mrfstr_rev_t;

#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_rev_threaded(void *args);
void *mrfstr_rev2_threaded(void *args);
#elif defined(_WIN32)
DWORD WINAPI mrfstr_rev_threaded(LPVOID args);
DWORD WINAPI mrfstr_rev2_threaded(LPVOID args);
#endif

#endif

#define mrfstr_rev_chrrev(c, i) \
    do                          \
    {                           \
        mrfstr_chr_t chr;       \
        for (; (c); (i))        \
        {                       \
            chr = *lptr;        \
            *lptr = *--rptr;    \
            *rptr = chr;        \
        }                       \
    } while (0)

#define mrfstr_rev2_chrrev(c, i) \
    for (; (c); (i))             \
        *lptr = *--rptr

mrfstr_res_enum_t mrfstr_reverse(
    mrfstr_t res, mrfstr_ct str)
{
    if (res == str)
    {
        if (MRFSTR_SIZE(res) <= 1)
            return MRFSTR_RES_NOERROR;

        mrfstr_data_t lptr = MRFSTR_DATA(res);
        mrfstr_data_t rptr = lptr + MRFSTR_SIZE(res);

        if (MRFSTR_SIZE(res) < MRFSTR_REV_SLIMIT)
        {
            mrfstr_rev_chrrev(lptr < rptr, lptr++);
            return MRFSTR_RES_NOERROR;
        }

        mrfstr_byte_t align = (uintptr_t)lptr & MRFSTR_REV2_SIMD_SIZE;
        if (align)
        {
            align = MRFSTR_REV2_SIMD_SIZE - align;
            mrfstr_rev_chrrev(align, (lptr++, align--));
        }

        mrfstr_rev_simd_t *lblock = (mrfstr_rev_simd_t*)lptr;
        mrfstr_rev_simd_t *rblock = (mrfstr_rev_simd_t*)rptr;

#if MRFSTR_THREADING
        mrstr_size_t size = MRFSTR_SIZE(res) - (align << 1);
        if (size < MRFSTR_REV_TLIMIT)
        {
#endif
            mrfstr_init_revidx;

            mrfstr_rev_simd_t left, right;
            for (; --rblock > lblock; lblock++)
            {
                mrfstr_rev_perm(left, lblock);
                mrfstr_rev_perm(right, rblock);

                mrfstr_rev_storeu(lblock, right);
                mrfstr_rev_storeu(rblock, left);
            }

            if (lblock == ++rblock)
                return MRFSTR_RES_NOERROR;

            lptr = (mrfstr_data_t)lblock;
            rptr = (mrfstr_data_t)rblock;

            mrfstr_rev_chrrev(lptr < rptr, lptr++);
            return MRFSTR_RES_NOERROR;
#if MRFSTR_THREADING
        }

        size /= MRFSTR_REV_TCHK;

        mrfstr_thread_t threads[MRFSTR_THREAD_COUNT];
        mrfstr_byte_t i;
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

            mrfstr_create_thread(mrfstr_rev_threaded)
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
            lptr = (mrfstr_data_t)lblock;
            rptr = (mrfstr_data_t)rblock;
            mrfstr_rev_chrrev(lptr < rptr, lptr++);
        }

        mrfstr_close_threads;
        return MRFSTR_RES_NOERROR;

rem:
        size *= MRFSTR_THREAD_COUNT - i;
        mrfstr_init_revidx;

        mrfstr_rev_simd_t left, right;
        for (; --rblock > lblock; lblock++)
        {
            mrfstr_rev_perm(left, lblock);
            mrfstr_rev_perm(right, rblock);

            mrfstr_rev_storeu(lblock, right);
            mrfstr_rev_storeu(rblock, left);
        }

        rblock++;
        goto ret;
#endif
    }

    MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
    if (!MRFSTR_SIZE(res))
        return MRFSTR_RES_NOERROR;

    mrfstr_data_t lptr = MRFSTR_DATA(res);
    mrfstr_data_t rptr = MRFSTR_DATA(str) + MRFSTR_SIZE(res);

    if (MRFSTR_SIZE(res) < MRFSTR_REV2_SLIMIT)
    {
        mrfstr_rev2_chrrev(rptr > MRFSTR_DATA(str), lptr++);
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_byte_t align = (uintptr_t)lptr & MRFSTR_REV2_SIMD_SIZE;
    if (align)
    {
        align = MRFSTR_REV2_SIMD_SIZE - align;
        mrfstr_rev2_chrrev(align, (lptr++, align--));
    }

    mrfstr_rev_simd_t *lblock = (mrfstr_rev_simd_t*)lptr;
    mrfstr_rev_simd_t *rblock = (mrfstr_rev_simd_t*)rptr;

#if MRFSTR_THREADING
    mrstr_size_t size = MRFSTR_SIZE(res) - align;
    if (size < MRFSTR_REV2_TLIMIT)
    {
#endif
        mrfstr_init_revidx;

        mrfstr_rev_simd_t block;
        for (; (mrfstr_data_t)--rblock >= MRFSTR_DATA(str); lblock++)
        {
            mrfstr_rev_perm(block, rblock);
            mrfstr_rev_storeu(lblock, block);
        }

        if ((mrfstr_data_t)++rblock == MRFSTR_DATA(str))
            return MRFSTR_RES_NOERROR;

        lptr = (mrfstr_data_t)lblock;
        rptr = (mrfstr_data_t)rblock;

        mrfstr_rev2_chrrev(rptr > MRFSTR_DATA(str), lptr++);
        return MRFSTR_RES_NOERROR;
#if MRFSTR_THREADING
    }

    size /= MRFSTR_REV2_TCHK;

    mrfstr_thread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_byte_t i;
    mrfstr_rev_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_REV_T));
        if (!data)
            goto rem2;

        data->left = lblock;
        data->right = rblock;
        data->size = size;

        lblock += size;
        rblock -= size;

        mrfstr_create_thread(mrfstr_rev2_threaded)
        {
            lblock -= size;
            rblock += size;

            mrstr_free(data);
            goto rem2;
        }
    }

ret2:
    if ((mrfstr_data_t)rblock != MRFSTR_DATA(str))
    {
        lptr = (mrfstr_data_t)lblock;
        rptr = (mrfstr_data_t)rblock;
        mrfstr_rev2_chrrev(rptr > MRFSTR_DATA(str), lptr++);
    }

    mrfstr_close_threads;
    return MRFSTR_RES_NOERROR;

rem2:
    do {} while (0);
    mrfstr_init_revidx;

    mrfstr_rev_simd_t block;
    for (; (mrfstr_data_t)--rblock >= MRFSTR_DATA(str); lblock++)
    {
        mrfstr_rev_perm(block, rblock);
        mrfstr_rev_storeu(lblock, block);
    }

    rblock++;
    goto ret2;
#endif
}

#if MRFSTR_THREADING
#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_rev_threaded(void *args)
#elif defined(_WIN32)
DWORD WINAPI mrfstr_rev_threaded(LPVOID args)
#endif
{
    mrfstr_rev_t data = (mrfstr_rev_t)args;
    mrfstr_init_revidx;

    mrfstr_rev_simd_t left, right;
    for (; data->size; data->size--, data->left++)
    {
        data->right--;

        mrfstr_rev_perm(left, data->left);
        mrfstr_rev_perm(right, data->right);

        mrfstr_rev_storeu(data->left, right);
        mrfstr_rev_storeu(data->right, left);
    }

    mrstr_free(data);
    return MRFSTR_TFUNC_RET;
}

#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_rev2_threaded(void *args)
#elif defined(_WIN32)
DWORD WINAPI mrfstr_rev2_threaded(LPVOID args)
#endif
{
    mrfstr_rev_t data = (mrfstr_rev_t)args;
    mrfstr_init_revidx;

    mrfstr_rev_simd_t block;
    for (; data->size; data->size--, data->left++)
    {
        mrfstr_rev_perm(block, --data->right);
        mrfstr_rev_storeu(data->left, block);
    }

    mrstr_free(data);
    return MRFSTR_TFUNC_RET;
}
#endif
