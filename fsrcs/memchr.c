/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifdef __AVX512F__

typedef __m512i mrfstr_memchr_simd_t;
#define MRFSTR_MEMCHR_SIMD_SIZE 64
#define MRFSTR_MEMCHR_SIMD_SHIFT 6

#define mrfstr_memchr_set(x, y) x = _mm512_set1_epi8(y)
#define mrfstr_memchr_load _mm512_stream_load_si512
#define mrfstr_memchr_cmp _mm512_cmpeq_epi8_mask

#elif defined(__AVX__) && defined(__AVX2__)

typedef __m256i mrfstr_memchr_simd_t;
#define MRFSTR_MEMCHR_SIMD_SIZE 32
#define MRFSTR_MEMCHR_SIMD_SHIFT 5

#define mrfstr_memchr_set(x, y) x = _mm256_set1_epi8(y)
#define mrfstr_memchr_load _mm256_stream_load_si256
#define mrfstr_memchr_cmp(x, y) _mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y))

#elif defined(__SSE2__)

typedef __m128i mrfstr_memchr_simd_t;
#define MRFSTR_MEMCHR_SIMD_SIZE 16
#define MRFSTR_MEMCHR_SIMD_SHIFT 4

#define mrfstr_memchr_set(x, y) x = _mm_set1_epi8(y)
#define mrfstr_memchr_load _mm_stream_load_si128
#define mrfstr_memchr_cmp(x, y) _mm_movemask_epi8(_mm_cmpeq_epi8(x, y))

#else

typedef unsigned long long mrfstr_memchr_simd_t;
#define MRFSTR_MEMCHR_SIMD_SIZE 8
#define MRFSTR_MEMCHR_SIMD_SHIFT 3

#define mrfstr_memchr_set(x, y)                                    \
    do                                                             \
    {                                                              \
        x = y;                                                     \
        for (mrfstr_bit_t i = 1; i < MRFSTR_MEMCHR_SIMD_SIZE; i++) \
            x = x << 8 | y;                                        \
    } while (0)

#define mrfstr_memchr_load(x) *x
#define mrfstr_memchr_cmp(x, y) \
    ((x ^ y) - 0x01010101010101010ULL) & ~(x ^ y) & 0x8080808080808080LL

#endif

#define MRFSTR_MEMCHR_SIMD_MASK (MRFSTR_MEMCHR_SIMD_SIZE - 1)

#if MRFSTR_THREADING
#include <pthread.h>

#define MRFSTR_MEMCHR_TCHK (MRFSTR_MEMCHR_SIMD_SIZE * MRFSTR_THREAD_COUNT)
#define MRFSTR_MEMCHR_TLIMIT (65536 * MRFSTR_MEMCHR_TCHK - 1)

struct __MRFSTR_MEMCHR_T
{
    mrfstr_memchr_simd_t *str;
    mrfstr_memchr_simd_t chr;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_MEMCHR_T *mrfstr_memchr_t;

void *mrfstr_memchr_threaded(void *args);
#endif

mrfstr_bool_t mrfstr_memchr(mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_memchr_simd_t *sblock = (mrfstr_memchr_simd_t*)str;
    mrfstr_memchr_simd_t cblock;
    mrfstr_memchr_set(cblock, chr);

#if MRFSTR_THREADING
    if (size <= MRFSTR_MEMCHR_TLIMIT)
    {
#endif
        mrfstr_size_t rem = size & MRFSTR_MEMCHR_SIMD_MASK;
        size >>= MRFSTR_MEMCHR_SIMD_SHIFT;

        mrfstr_memchr_simd_t block;
        for (; size; sblock++, size--)
        {
            block = mrfstr_memchr_load(sblock);
            if (mrfstr_memchr_cmp(block, cblock))
                return MRFSTR_TRUE;
        }

        return memchr((mrfstr_data_ct)sblock, chr, rem) != NULL;
#if MRFSTR_THREADING
    }

    mrfstr_size_t rem = size % MRFSTR_MEMCHR_TCHK;
    size /= MRFSTR_MEMCHR_TCHK;

    volatile mrfstr_bool_t res = MRFSTR_FALSE;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_memchr_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_MEMCHR_T));
        data->str = sblock;
        data->chr = cblock;
        data->size = size;
        data->res = &res;

        sblock += size;
        pthread_create(threads + i, NULL, mrfstr_memchr_threaded, data);
    }

    mrfstr_bool_t rres = memchr((mrfstr_data_t)sblock, chr, rem) != NULL;
    if (rres)
        res = MRFSTR_TRUE;

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    return res;
#endif
}

#if MRFSTR_THREADING
void *mrfstr_memchr_threaded(void *args)
{
    mrfstr_memchr_t data = (mrfstr_memchr_t)args;

    mrfstr_size_t i;
    mrfstr_memchr_simd_t block;
    while (data->size > 65536)
    {
        if (*data->res)
        {
            mrstr_free(data);
            return NULL;
        }

        for (i = 0; i < 65536; data->str++, i++)
        {
            block = mrfstr_memchr_load(data->str);
            if (mrfstr_memchr_cmp(block, data->chr))
            {
                *data->res = MRFSTR_TRUE;

                mrstr_free(data);
                return NULL;
            }
        }

        data->size -= 65536;
    }

    if (*data->res)
    {
        mrstr_free(data);
        return NULL;
    }

    for (; data->size; data->str++, data->size--)
    {
        block = mrfstr_memchr_load(data->str);
        if (mrfstr_memchr_cmp(block, data->chr))
        {
            *data->res = MRFSTR_TRUE;

            mrstr_free(data);
            return NULL;
        }
    }

    mrstr_free(data);
    return NULL;
}
#endif
