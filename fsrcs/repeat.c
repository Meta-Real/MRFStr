/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#if MRFSTR_THREADING
#include <pthread.h>

struct __MRFSTR_REPEAT_CHR_T
{
    mrfstr_simd_block_t *res;
    mrfstr_simd_block_t block;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_REPEAT_CHR_T *mrfstr_repeat_chr_t;

void *mrfstr_repeat_chr_threaded(void *args);
#endif

void mrfstr_repeat_chr(mrfstr_t res, mrfstr_chr_t chr, mrfstr_size_t count)
{
    if (!count)
    {
        if (!res->size)
            return;

        res->size = 0;
        return;
    }

    res->size = count;
    if (res->alloc <= count)
    {
        if (res->alloc)
            mrstr_free(res->data);

        res->alloc = count + 1;
        res->data = mrstr_alloc(res->alloc);
    }

    mrfstr_simd_block_t *rblock = (mrfstr_simd_block_t*)res->data;
    mrfstr_simd_block_t block = mrfstr_simd_set1_func(chr);

#if MRFSTR_THREADING
    if (count <= MRFSTR_THREAD_LIMIT)
    {
#endif
        mrfstr_size_t rem = count & MRFSTR_SIMD_MASK;
        count >>= MRFSTR_SIMD_SHIFT;

        for (; count; rblock++, count--)
            mrfstr_simd_stream_func(rblock, block);

        mrfstr_data_t rptr = (mrfstr_data_t)rblock;
        memset(rptr, chr, rem);
        rptr[rem] = '\0';
#if MRFSTR_THREADING
        return;
    }

    mrfstr_size_t rem = count % MRFSTR_THREAD_CHUNK;
    count /= MRFSTR_THREAD_CHUNK;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_repeat_chr_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_REPEAT_CHR_T));
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
#endif
}

#if MRFSTR_THREADING
void *mrfstr_repeat_chr_threaded(void *args)
{
    mrfstr_repeat_chr_t data = (mrfstr_repeat_chr_t)args;

    for (; data->size; data->res++, data->size--)
        mrfstr_simd_stream_func(data->res, data->block);

    mrstr_free(data);
    return NULL;
}
#endif
