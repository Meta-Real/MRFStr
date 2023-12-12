/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>

#if defined(__AVX512F__) && defined(__AVX512BW__)

typedef __m512i mrfstr_repl_simd_t;
#define MRFSTR_REPL_SIMD_SIZE 64
#define MRFSTR_REPL_SIMD_SHIFT 6

#define mrfstr_repl_set _mm512_set1_epi8
#define mrfstr_repl_load _mm512_load_si512
#define mrfstr_repl_cmp _mm512_cmpeq_epi8_mask
#define mrfstr_repl_store _mm512_mask_storeu_epi8

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrfstr_repl_simd_t;
#define MRFSTR_REPL_SIMD_SIZE 32
#define MRFSTR_REPL_SIMD_SHIFT 5

#define mrfstr_repl_set _mm256_set1_epi8
#define mrfstr_repl_load _mm256_load_si256
#define mrfstr_repl_cmp(x, y) _mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y))
#define mrfstr_repl_store _mm256_mask_storeu_epi8

#elif defined(__SSE2__)

typedef __m128i mrfstr_repl_simd_t;
#define MRFSTR_REPL_SIMD_SIZE 16
#define MRFSTR_REPL_SIMD_SHIFT 4

#define mrfstr_repl_set _mm_set1_epi8
#define mrfstr_repl_load _mm_load_si128
#define mrfstr_repl_cmp(x, y) _mm_movemask_epi8(_mm_cmpeq_epi8(x, y))
#define mrfstr_repl_store _mm_mask_storeu_epi8

#else
#error 64-bit Replace Not Implemented Yet
#endif

#define MRFSTR_REPL_SIMD_MASK (MRFSTR_REPL_SIMD_SIZE - 1)

#define mrfstr_repl_sub(r, o, n, s)            \
    do                                         \
    {                                          \
        mrfstr_repl_simd_t block;              \
        mrfstr_size_t rres;                    \
        for (; s; r++, s--)                    \
        {                                      \
            block = mrfstr_repl_load(r);       \
            rres = mrfstr_repl_cmp(block, o);  \
            if (rres)                          \
                mrfstr_repl_store(r, rres, n); \
        }                                      \
    } while (0)

#define mrfstr_repl_rem                             \
    do                                              \
    {                                               \
        mrfstr_data_t rptr = (mrfstr_data_t)rblock; \
        for (; rem; rptr++, rem--)                  \
            if (*rptr == old)                       \
                *rptr = new;                        \
    } while (0)

#if MRFSTR_THREADING
#include <pthread.h>

#define MRFSTR_REPL_TCHK (MRFSTR_REPL_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_REPL_TLIMIT (0x10000 * MRFSTR_REPL_TCHK - 1)

struct __MRFSTR_REPL_T
{
    mrfstr_repl_simd_t *res;
    mrfstr_repl_simd_t old;
    mrfstr_repl_simd_t new;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_REPL_T *mrfstr_repl_t;

void *mrfstr_repl_threaded(void *args);
#endif

mrfstr_res_enum_t mrfstr_replace(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new)
{
    if (!str->size)
    {
        res->size = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (res == str)
    {
        mrfstr_repl_simd_t *rblock = (mrfstr_repl_simd_t*)res->data;
        mrfstr_repl_simd_t oblock = mrfstr_repl_set(old);
        mrfstr_repl_simd_t nblock = mrfstr_repl_set(new);

#if MRFSTR_THREADING
        if (res->size <= MRFSTR_REPL_TLIMIT)
        {
#endif
            mrfstr_bit_t rem = res->size & MRFSTR_REPL_SIMD_MASK;
            mrfstr_size_t size = res->size >> MRFSTR_REPL_SIMD_SHIFT;

            mrfstr_repl_sub(rblock, oblock, nblock, size);
            mrfstr_repl_rem;

            return MRFSTR_RES_NOERROR;
#if MRFSTR_THREADING
        }

        mrfstr_size_t rem = res->size % MRFSTR_REPL_TCHK;
        mrfstr_size_t size = res->size / MRFSTR_REPL_TCHK;

        pthread_t threads[MRFSTR_THREAD_COUNT];
        mrfstr_bit_t i;
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
            if (pthread_create(threads + i, NULL, mrfstr_repl_threaded, data))
            {
                rblock -= size;

                mrstr_free(data);
                goto ret;
            }
        }

ret:
        mrfstr_repl_rem;

        while (i--)
            pthread_join(threads[i], NULL);
        return MRFSTR_RES_NOERROR;

rem:
        size *= MRFSTR_THREAD_COUNT - i;
        mrfstr_repl_sub(rblock, oblock, nblock, size);
        goto ret;
#endif
    }

    // For later support
    return MRFSTR_RES_NOERROR;
}

#if MRFSTR_THREADING
void *mrfstr_repl_threaded(void *args)
{
    mrfstr_repl_t data = (mrfstr_repl_t)args;
    mrfstr_repl_sub(data->res, data->old, data->new, data->size);

    mrstr_free(data);
    return NULL;
}
#endif
