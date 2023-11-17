/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <immintrin.h>
#include <pthread.h>
#include <alloc.h>
#include <string.h>
#include <stdio.h>

struct __MRFSTR_REPEAT_CHR_T
{
    __m512i *res;
    __m512i block;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_REPEAT_CHR_T *mrfstr_repeat_chr_t;

void *mrfstr_repeat_chr_threaded(void *args);

void mrfstr_repeat_chr(mrfstr_t res, mrfstr_chr_t chr, mrfstr_size_t count)
{
    if (!count)
    {
        if (!res->size)
            return;

        *res->data = '\0';
        res->size = 0;
        return;
    }

    res->size = count;
    if (res->alloc <= count)
    {
        if (res->alloc)
            __mrstr_free(res->data);

        res->alloc = count + 1;
        res->data = __mrstr_alloc(res->alloc);
    }

    __m512i *rblock = (__m512i*)res->data;
    __m512i block = _mm512_set1_epi8(chr);

    if (count <= MRFSTR_THREAD_LIMIT)
    {
        mrfstr_size_t rem = count & 63;
        count >>= 6;

        for (; count; rblock++, count--)
            _mm512_stream_si512(rblock, block);

        mrfstr_data_t rptr = (mrfstr_data_t)rblock;
        memset(rptr, chr, rem);
        rptr[rem] = '\0';
        return;
    }

    mrfstr_size_t rem = count % MRFSTR_THREAD_CHUNK;
    count /= MRFSTR_THREAD_CHUNK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_repeat_chr_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRFSTR_REPEAT_CHR_T));
        data->res = rblock;
        data->block = block;
        data->size = count;

        rblock += count;

        pthread_create(threads + i, NULL, mrfstr_repeat_chr_threaded, data);
    }

    mrfstr_data_t rptr = (mrfstr_data_t)rblock;
    memset(rptr, chr, rem);
    rptr[rem] = '\0';

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);
}

void *mrfstr_repeat_chr_threaded(void *args)
{
    mrfstr_repeat_chr_t data = (mrfstr_repeat_chr_t)args;

    for (; data->size; data->res++, data->size--)
        _mm512_stream_si512(data->res, data->block);

    __mrstr_free_una(data);
    return NULL;
}
