/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#if MRFSTR_THREADING
#include <pthread.h>

struct __MRFSTR_SET_T
{
    mrfstr_simd_block_t *src;
    mrfstr_simd_block_t *dst;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_SET_T *mrfstr_set_t;

void *mrfstr_set_threaded(void *args);
#endif

void mrfstr_set(mrfstr_t dst, mrfstr_ct src)
{
    if (!src->size)
    {
        if (!dst->size)
            return;

        dst->size = 0;
        return;
    }

    mrfstr_size_t size = src->size + 1;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_alloc(dst->alloc);
    }

    mrfstr_simd_block_t *sblock = (mrfstr_simd_block_t*)src->data;
    mrfstr_simd_block_t *dblock = (mrfstr_simd_block_t*)dst->data;

#if MRFSTR_THREADING
    if (size <= MRFSTR_THREAD_LIMIT)
    {
#endif
        mrfstr_size_t rem = size & MRFSTR_SIMD_MASK;
        size >>= MRFSTR_SIMD_SHIFT;

        for (; size; sblock++, dblock++, size--)
            mrfstr_simd_stream_func(dblock, *sblock);

        memcpy((mrfstr_data_t)dblock, (mrfstr_data_t)sblock, rem);
#if MRFSTR_THREADING
        return;
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_set_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_SET_T));
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
#endif
}

void mrfstr_set_str(mrfstr_t dst, mrfstr_data_ct src)
{
    mrfstr_size_t size = strlen(src);
    if (!size)
    {
        if (!dst->size)
            return;

        dst->size = 0;
        return;
    }

    dst->size = size++;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_alloc(dst->alloc);
    }

    mrfstr_simd_block_t *sblock = (mrfstr_simd_block_t*)src;
    mrfstr_simd_block_t *dblock = (mrfstr_simd_block_t*)dst->data;

#if MRFSTR_THREADING
    if (size <= MRFSTR_THREAD_LIMIT)
    {
#endif
        mrfstr_size_t rem = size & MRFSTR_SIMD_MASK;
        size >>= MRFSTR_SIMD_SHIFT;

        for (; size; sblock++, dblock++, size--)
            mrfstr_simd_stream_func(dblock, *sblock);

        memcpy((mrfstr_data_t)dblock, (mrfstr_data_t)sblock, rem);
#if MRFSTR_THREADING
        return;
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_set_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_SET_T));
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
#endif
}

void mrfstr_set_nstr(mrfstr_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    if (!size)
    {
        if (!dst->size)
            return;

        dst->size = 0;
        return;
    }

    dst->size = size++;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_alloc(dst->alloc);
    }

    mrfstr_simd_block_t *sblock = (mrfstr_simd_block_t*)src;
    mrfstr_simd_block_t *dblock = (mrfstr_simd_block_t*)dst->data;

#if MRFSTR_THREADING
    if (size <= MRFSTR_THREAD_LIMIT)
    {
#endif
        mrfstr_size_t rem = size & MRFSTR_SIMD_MASK;
        size >>= MRFSTR_SIMD_SHIFT;

        for (; size; sblock++, dblock++, size--)
            mrfstr_simd_stream_func(dblock, *sblock);

        mrfstr_data_t dptr = (mrfstr_data_t)dblock;
        memcpy(dptr, (mrfstr_data_t)sblock, rem);
#if MRFSTR_THREADING
        return;
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_set_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_SET_T));
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
#endif
}

#if MRFSTR_THREADING
void *mrfstr_set_threaded(void *args)
{
    mrfstr_set_t data = (mrfstr_set_t)args;

    for (; data->size; data->src++, data->dst++, data->size--)
        mrfstr_simd_stream_func(data->dst, *data->src);

    mrstr_free(data);
    return NULL;
}
#endif
