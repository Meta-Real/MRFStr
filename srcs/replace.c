/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>

#if MRFSTR_SIMD_SIZE == 64 && defined(__AVX512BW__)

typedef __m512i mrfstr_repl_simd_t;
#define MRFSTR_REPL_SIMD_SIZE 64
#define MRFSTR_REPL_SIMD_SHIFT 6

#define mrfstr_repl_set(x, y) ((x) = _mm512_set1_epi8(y))
#define mrfstr_repl_loadu _mm512_loadu_si512
#define mrfstr_repl_cmpeq _mm512_cmpeq_epi8_mask
#define mrfstr_repl_store _mm512_mask_storeu_epi8

#elif defined(__AVX__) && defined(__AVX2__)
#define MRFSTR_NOAVX512

typedef __m256i mrfstr_repl_simd_t;
#define MRFSTR_REPL_SIMD_SIZE 32
#define MRFSTR_REPL_SIMD_SHIFT 5

#define mrfstr_repl_set(x, y) ((x) = _mm256_set1_epi8(y))
#define mrfstr_repl_loadu _mm256_loadu_si256
#define mrfstr_repl_cmpeq _mm256_cmpeq_epi8
#define mrfstr_repl_store(r, x, y, c) _mm256_store_si256((r), _mm256_blendv_epi8((x), (y), (c)))

#elif defined(__SSE2__) && defined(__SSE4_1__)
#define MRFSTR_NOAVX512

typedef __m128i mrfstr_repl_simd_t;
#define MRFSTR_REPL_SIMD_SIZE 16
#define MRFSTR_REPL_SIMD_SHIFT 4

#define mrfstr_repl_set(x, y) ((x) = _mm_set1_epi8(y))
#define mrfstr_repl_loadu _mm_loadu_si128
#define mrfstr_repl_cmpeq _mm_cmpeq_epi8
#define mrfstr_repl_store(r, x, y, c) _mm_store_si128((r), _mm_blendv_epi8((x), (y), (c)))

#else

typedef uint64_t mrfstr_repl_simd_t;
#define MRFSTR_REPL_SIMD_SIZE 8
#define MRFSTR_REPL_SIMD_SHIFT 3

#define mrfstr_repl_set(x, y)   \
    do                          \
    {                           \
        (x) = (y) | ((y) << 8); \
        (x) |= ((x) << 16);     \
        (x) |= ((x) << 32);     \
    } while (0)

#define mrfstr_repl_loadu(x) (*(x))

#error 64-bit Replace Not Implemented Yet
#endif

#define MRFSTR_REPL_SIMD_MASK (MRFSTR_REPL_SIMD_SIZE - 1)
#define MRFSTR_REPL_SLIMIT (0x100 * MRFSTR_REPL_SIMD_SIZE)

#define mrfstr_repl_sub(r, o, n, s)             \
    do                                          \
    {                                           \
        mrfstr_repl_simd_t block;               \
        mrfstr_size_t cres;                     \
        for (; s; r++, s--)                     \
        {                                       \
            block = mrfstr_repl_loadu(r);       \
            cres = mrfstr_repl_cmpeq(block, o); \
            if (cres)                           \
                mrfstr_repl_store(r, cres, n);  \
        }                                       \
    } while (0)

#define mrfstr_repl_rem               \
    do                                \
    {                                 \
        rptr = (mrfstr_data_t)rblock; \
        for (; rem; rptr++, rem--)    \
            if (*rptr == old)         \
                *rptr = new;          \
    } while (0)

#if MRFSTR_THREADING

#define MRFSTR_REPL_TCHK (MRFSTR_REPL_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_REPL_TLIMIT (0x10000 * MRFSTR_REPL_TCHK)

struct __MRFSTR_REPL_T
{
    mrfstr_repl_simd_t *res;
    mrfstr_repl_simd_t old;
    mrfstr_repl_simd_t new;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_REPL_T *mrfstr_repl_t;

#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_repl_threaded(void *args);
#elif defined(_WIN32)
DWORD WINAPI mrfstr_repl_threaded(LPVOID args);
#endif

#endif

void mrfstr_replace(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new)
{
    if (MRFSTR_SIZE(str) <= MRFSTR_REPL_SLIMIT)
    {
        mrfstr_chr_t chr;
        mrfstr_short_t i;
        if (res == str)
        {
            for (i = 0; i < MRFSTR_SIZE(str); i++)
            {
                chr = MRFSTR_DATA(res)[i];
                if (chr == old)
                    chr = new;
            }

            return;
        }

        for (i = 0; i < MRFSTR_SIZE(str); i++)
        {
            chr = MRFSTR_DATA(str)[i];
            MRFSTR_DATA(res)[i] = chr == old ? new : chr;
        }

        return;
    }

    if (!MRFSTR_SIZE(str))
    {
        mrstr_free(MRFSTR_DATA(res));

        MRFSTR_DATA(res) = NULL;
        MRFSTR_SIZE(res) = 0;
        return;
    }

    if (res == str)
    {
        mrstr_size_t size = MRFSTR_SIZE(res);
        mrfstr_data_t rptr = MRFSTR_DATA(res);

        mrfstr_byte_t align = (uintptr_t)MRFSTR_DATA(res) & MRFSTR_REPL_SIMD_SIZE;
        if (align)
        {
            align = MRFSTR_REPL_SIMD_SIZE - align;
            size -= align;

            for (; align; rptr++, align--)
                if (*rptr == old)
                    *rptr = new;
        }

        mrfstr_repl_simd_t *rblock = (mrfstr_repl_simd_t*)rptr;

        mrfstr_repl_simd_t oblock;
        mrfstr_repl_set(oblock, old);

        mrfstr_repl_simd_t nblock;
        mrfstr_repl_set(nblock, old);

#if MRFSTR_THREADING
        if (MRFSTR_SIZE(res) < MRFSTR_REPL_TLIMIT)
        {
#endif
            mrfstr_byte_t rem = size & MRFSTR_REPL_SIMD_MASK;
            size >>= MRFSTR_REPL_SIMD_SHIFT;

            mrfstr_repl_simd_t block;
#ifdef MRFSTR_NOAVX512
            mrfstr_repl_simd_t cres;
#else
            mrfstr_size_t cres;
#endif
            for (; size; rblock++, size--)
            {
                block = mrfstr_repl_loadu(rblock);

                cres = mrfstr_repl_cmpeq(block, oblock);
#ifdef MRFSTR_NOAVX512
                mrfstr_repl_store(rblock, nblock, block, cres);
#else
                if (cres)
                    mrfstr_repl_store(rblock, cres, nblock);
#endif
            }

            mrfstr_repl_rem;
            return;
#if MRFSTR_THREADING
        }

        mrfstr_short_t rem = size % MRFSTR_REPL_TCHK;
        size /= MRFSTR_REPL_TCHK;

        mrfstr_thread_t threads[MRFSTR_THREAD_COUNT];
        mrfstr_byte_t i;
        mrfstr_repl_t data;
        for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        {
            data = mrstr_alloc(sizeof(struct __MRFSTR_REPL_T));
            if (!data)
                goto rem;

            data->res = rblock;
            data->old = oblock;
            data->new = nblock;
            data->size = size;

            rblock += size;
            mrfstr_create_thread(mrfstr_repl_threaded)
            {
                rblock -= size;

                mrstr_free(data);
                goto ret;
            }
        }

ret:
        mrfstr_repl_rem;

        mrfstr_close_threads;
        return;

rem:
        size *= MRFSTR_THREAD_COUNT - i;

        mrfstr_repl_simd_t block;
#ifdef MRFSTR_NOAVX512
        mrfstr_repl_simd_t cres;
#else
        mrfstr_size_t cres;
#endif
        for (; size; rblock++, size--)
        {
            block = mrfstr_repl_loadu(rblock);

            cres = mrfstr_repl_cmpeq(block, oblock);
#ifdef MRFSTR_NOAVX512
            mrfstr_repl_store(rblock, nblock, block, cres);
#else
            if (cres)
                mrfstr_repl_store(rblock, cres, nblock);
#endif
        }

        goto ret;
#endif
    }

    // For later support
}

#if MRFSTR_THREADING
#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_repl_threaded(void *args)
#elif defined(_WIN32)
DWORD WINAPI mrfstr_repl_threaded(LPVOID args)
#endif
{
    mrfstr_repl_t data = (mrfstr_repl_t)args;

    mrfstr_repl_simd_t block;
#ifdef MRFSTR_NOAVX512
        mrfstr_repl_simd_t cres;
#else
        mrfstr_size_t cres;
#endif
        for (; data->size; data->res++, data->size--)
        {
            block = mrfstr_repl_loadu(data->res);

            cres = mrfstr_repl_cmpeq(block, data->old);
#ifdef MRFSTR_NOAVX512
            mrfstr_repl_store(data->res, data->new, block, cres);
#else
            if (cres)
                mrfstr_repl_store(data->res, cres, data->new);
#endif
        }

    mrstr_free(data);
    return MRFSTR_TFUNC_RET;
}
#endif
