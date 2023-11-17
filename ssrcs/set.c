/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <immintrin.h>
#include <pthread.h>
#include <alloc.h>
#include <string.h>

struct __MRFSTR_SET_T
{
    __m512i *src;
    __m512i *dst;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_SET_T *mrfstr_set_t;

void *mrfstr_set_threaded(void *args);

void mrfstr_set(mrfstr_t dst, mrfstr_ct src)
{
    if (!src->size)
    {
        if (!dst->size)
            return;

        *dst->data = '\0';
        dst->size = 0;
        return;
    }

    mrfstr_size_t size = src->size + 1;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            __mrstr_free(dst->data);

        dst->alloc = size;
        dst->data = __mrstr_alloc(dst->alloc);
    }

    __m512i *sblock = (__m512i*)src->data;
    __m512i *dblock = (__m512i*)dst->data;
    __m512i block;

    if (size <= MRFSTR_THREAD_LIMIT)
    {
        mrfstr_size_t rem = size & 63;
        size >>= 6;

        for (; size; sblock++, dblock++, size--)
        {
            block = _mm512_stream_load_si512(sblock);
            _mm512_stream_si512(dblock, block);
        }

        memcpy((mrfstr_data_t)dblock, (mrfstr_data_t)sblock, rem);
        return;
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_set_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRFSTR_SET_T));
        data->src = sblock;
        data->dst = dblock;
        data->size = size;

        sblock += size;
        dblock += size;

        pthread_create(threads + i, NULL, mrfstr_set_threaded, data);
    }

    memcpy((mrfstr_data_t)dblock, (mrfstr_data_t)sblock, rem);
    dst->size = src->size;

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);
}

void mrfstr_set_str(mrfstr_t dst, mrfstr_data_ct src)
{
    mrfstr_size_t size = strlen(src);
    if (!size)
    {
        if (!dst->size)
            return;

        *dst->data = '\0';
        dst->size = 0;
        return;
    }

    dst->size = size++;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            __mrstr_free(dst->data);

        dst->alloc = size;
        dst->data = __mrstr_alloc(dst->alloc);
    }

    __m512i *sblock = (__m512i*)src;
    __m512i *dblock = (__m512i*)dst->data;
    __m512i block;

    if (size <= MRFSTR_THREAD_LIMIT)
    {
        mrfstr_size_t rem = size & 63;
        size >>= 6;

        for (; size; sblock++, dblock++, size--)
        {
            block = _mm512_stream_load_si512(sblock);
            _mm512_stream_si512(dblock, block);
        }

        memcpy((mrfstr_data_t)dblock, (mrfstr_data_t)sblock, rem);
        return;
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_set_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRFSTR_SET_T));
        data->src = sblock;
        data->dst = dblock;
        data->size = size;

        sblock += size;
        dblock += size;

        pthread_create(threads + i, NULL, mrfstr_set_threaded, data);
    }

    memcpy((mrfstr_data_t)dblock, (mrfstr_data_t)sblock, rem);

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);
}

void mrfstr_set_nstr(mrfstr_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    if (!size)
    {
        if (!dst->size)
            return;

        *dst->data = '\0';
        dst->size = 0;
        return;
    }

    dst->size = size++;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            __mrstr_free(dst->data);

        dst->alloc = size;
        dst->data = __mrstr_alloc(dst->alloc);
    }

    __m512i *sblock = (__m512i*)src;
    __m512i *dblock = (__m512i*)dst->data;
    __m512i block;

    if (size <= MRFSTR_THREAD_LIMIT)
    {
        mrfstr_size_t rem = size & 63;
        size >>= 6;

        for (; size; sblock++, dblock++, size--)
        {
            block = _mm512_stream_load_si512(sblock);
            _mm512_stream_si512(dblock, block);
        }

        mrfstr_data_t dptr = (mrfstr_data_t)dblock;
        memcpy(dptr, (mrfstr_data_t)sblock, rem);
        return;
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_set_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRFSTR_SET_T));
        data->src = sblock;
        data->dst = dblock;
        data->size = size;

        sblock += size;
        dblock += size;

        pthread_create(threads + i, NULL, mrfstr_set_threaded, data);
    }

    mrfstr_data_t dptr = (mrfstr_data_t)dblock;
    memcpy(dptr, (mrfstr_data_t)sblock, rem);

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);
}

void *mrfstr_set_threaded(void *args)
{
    mrfstr_set_t data = (mrfstr_set_t)args;

    __m512i block;
    for (; data->size; data->src++, data->dst++, data->size--)
    {
        block = _mm512_stream_load_si512(data->src);
        _mm512_stream_si512(data->dst, block);
    }

    __mrstr_free_una(data);
    return NULL;
}
