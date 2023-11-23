/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#if MRFSTR_THREADING
#include <pthread.h>

struct __MRFSTR_MEMCPY_T
{
    mrfstr_simd_block_t *src;
    mrfstr_simd_block_t *dst;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_MEMCPY_T *mrfstr_memcpy_t;

void *mrfstr_memcpy_threaded(void *args);
#endif

void mrfstr_memcpy(mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    mrfstr_simd_block_t *sblock = (mrfstr_simd_block_t*)src;
    mrfstr_simd_block_t *dblock = (mrfstr_simd_block_t*)dst;

#if MRFSTR_THREADING
    if (size <= MRFSTR_THREAD_LIMIT)
    {
#endif
        mrfstr_size_t rem = size & MRFSTR_SIMD_MASK;
        size >>= MRFSTR_SIMD_SHIFT;

        for (; size; sblock++, dblock++, size--)
            mrfstr_simd_store_func(dblock, *sblock);

        mrfstr_data_t dptr = (mrfstr_data_t)dblock;
        memcpy(dptr, (mrfstr_data_t)sblock, rem);
#if MRFSTR_THREADING
        return;
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_memcpy_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_MEMCPY_T));
        data->src = sblock;
        data->dst = dblock;
        data->size = size;

        sblock += size;
        dblock += size;

        pthread_create(threads + i, NULL, mrfstr_memcpy_threaded, data);
    }

    mrfstr_data_t dptr = (mrfstr_data_t)dblock;
    memcpy(dptr, (mrfstr_data_t)sblock, rem);

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);
#endif
}

#if MRFSTR_THREADING
void *mrfstr_memcpy_threaded(void *args)
{
    mrfstr_memcpy_t data = args;

    for (; data->size; data->src++, data->dst++, data->size--)
        mrfstr_simd_stream_func(data->dst, *data->src);

    mrstr_free(data);
    return NULL;
}
#endif
