/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <immintrin.h>
#include <pthread.h>
#include <alloc.h>
#include <string.h>

struct __MRFSTR_CONTAIN_T
{
    __m512i *str;
    __m512i chr;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_CONTAIN_T *mrfstr_contain_t;

void *mrfstr_contain_threaded(void *args);

mrfstr_bool_t mrfstr_contain_chr(mrfstr_ct str, mrfstr_chr_t chr)
{
    if (!str->size)
        return MRFSTR_FALSE;

    __m512i *sblock = (__m512i*)str->data;
    __m512i cblock = _mm512_set1_epi8(chr);

    mrfstr_size_t size = str->size;
    if (size <= MRFSTR_THREAD_LIMIT)
    {
        mrfstr_size_t rem = size & 63;
        size >>= 6;

        for (; size; sblock++, size--)
            if (_mm512_cmpeq_epi8_mask(*sblock, cblock))
                return MRFSTR_TRUE;

        return memchr((mrfstr_data_ct)sblock, chr, rem) != NULL;
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    volatile mrfstr_bool_t res = MRFSTR_FALSE;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_contain_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRFSTR_CONTAIN_T));
        data->str = sblock;
        data->chr = cblock;
        data->size = size;
        data->res = &res;

        sblock += size;
        pthread_create(threads + i, NULL, mrfstr_contain_threaded, data);
    }

    mrfstr_bool_t rres = memchr((mrfstr_data_t)sblock, chr, rem) != NULL;
    if (rres)
        res = MRFSTR_TRUE;

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    return res;
}

void *mrfstr_contain_threaded(void *args)
{
    mrfstr_contain_t data = (mrfstr_contain_t)args;

    mrfstr_size_t i;
    while (data->size > 65536)
    {
        if (*data->res)
        {
            __mrstr_free_una(data);
            return NULL;
        }

        for (i = 0; i < 65536; data->str++, i++)
            if (_mm512_cmpeq_epi8_mask(*data->str, data->chr))
            {
                *data->res = MRFSTR_TRUE;

                __mrstr_free_una(data);
                return NULL;
            }

        data->size -= 65536;
    }

    if (*data->res)
    {
        __mrstr_free_una(data);
        return NULL;
    }

    for (; data->size; data->str++, data->size--)
        if (_mm512_cmpeq_epi8_mask(*data->str, data->chr))
        {
            *data->res = MRFSTR_TRUE;

            __mrstr_free_una(data);
            return NULL;
        }

    __mrstr_free_una(data);
    return NULL;
}
