/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

struct __MRTSTR_REPEAT_CHR_T
{
    mrtstr_simd_block_t *res;
    mrtstr_simd_block_t block;
    mrtstr_size_t size;

    mrtstr_lock_t *lock;
};
typedef struct __MRTSTR_REPEAT_CHR_T *mrtstr_repeat_chr_t;

void mrtstr_repeat_chr_threaded(void *args);

void mrtstr_repeat_chr(mrtstr_t res, mrtstr_chr_t chr, mrtstr_size_t count)
{
    for (; mrtstr_locked(res););

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

    mrtstr_simd_block_t *rblock = (mrtstr_simd_block_t*)res->data;
    mrtstr_simd_block_t block = mrtstr_simd_set1_func(chr);

    if (count <= MRTSTR_THREAD_LIMIT)
    {
        mrtstr_size_t rem = count & MRTSTR_SIMD_MASK;
        count >>= MRTSTR_SIMD_SHIFT;

        for (; count; rblock++, count--)
            mrtstr_simd_stream_func(rblock, block);

        mrtstr_data_t rptr = (mrtstr_data_t)rblock;
        memset(rptr, rem, chr);
        rptr[rem] = '\0';
        return;
    }

    mrtstr_size_t rem = count % MRTSTR_THREAD_CHUNK;
    count /= MRTSTR_THREAD_CHUNK;

    mrtstr_size_t j;
    mrtstr_bit_t i;
    mrtstr_repeat_chr_t data;
    for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRTSTR_REPEAT_CHR_T));
        data->block = block;
        data->size = count;
        data->lock = res->lock + i;

        while (!mrtstr_threads.free_threads && data->size > 65536)
        {
            for (j = 0; j < 65536; rblock++, j++)
                mrtstr_simd_stream_func(rblock, block);

            data->size -= 65536;
        }

        if (data->size <= 65536)
        {
            for (; data->size; rblock++, data->size--)
                mrtstr_simd_stream_func(rblock, block);

            __mrstr_free_una(data);
        }
        else
        {
            data->res = rblock;
            rblock += data->size;

            res->lock[i] = 1;
            mrtstr_load_threads(mrtstr_repeat_chr_threaded, data);
        }
    }

    mrtstr_data_t rptr = (mrtstr_data_t)rblock;
    memset(rptr, chr, rem);
    rptr[rem] = '\0';

    res->forced = MRTSTR_TRUE;
}

void mrtstr_repeat_chr_threaded(void *args)
{
    mrtstr_repeat_chr_t data = (mrtstr_repeat_chr_t)args;

    for (; data->size; data->res++, data->size--)
        mrtstr_simd_stream_func(data->res, data->block);

    *data->lock = 0;
    __mrstr_free_una(data);
}
