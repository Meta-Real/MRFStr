/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

struct __MRTSTR_CONTAIN_CHR_T
{
    mrtstr_simd_block_t *str;
    mrtstr_simd_block_t chr;
    mrtstr_size_t size;

    mrtstr_lock_t *lock;
    pthread_mutex_t *mutex;

    mrtstr_bres_t *res;
};
typedef struct __MRTSTR_CONTAIN_CHR_T *mrtstr_contain_chr_t;

void mrtstr_contain_chr_threaded(void *args);

void mrtstr_contain_chr(mrtstr_bres_t *res, mrtstr_ct str, mrtstr_chr_t chr)
{
    res->lock = 0;
    pthread_mutex_init(&res->mutex, NULL);

    if (!str->size)
    {
        res->res = MRTSTR_FALSE;
        return;
    }

    mrtstr_simd_block_t *sblock = (mrtstr_simd_block_t*)str->data;
    mrtstr_simd_block_t block = mrtstr_simd_set1_func(chr);

    mrtstr_size_t size = str->size;
    if (size <= MRTSTR_THREAD_LIMIT)
    {
        mrtstr_size_t rem = size & MRTSTR_SIMD_MASK;
        size >>= MRTSTR_SIMD_SHIFT;

        for (; size; sblock++, size--)
            if (mrtstr_simd_cmpeq_mask_func(*sblock, block))
            {
                res->res = MRTSTR_TRUE;
                return;
            }

        res->res = memchr((mrtstr_data_ct)sblock, chr, rem) != NULL;
        return;
    }

    res->res = MRTSTR_FALSE;

    mrtstr_size_t rem = size % MRTSTR_THREAD_CHUNK;
    size /= MRTSTR_THREAD_CHUNK;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_contain_chr_t data;
    if (mrtstr_locked(str) && str->forced)
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            for (; str->lock[i];);

            data = mrstr_alloc(sizeof(struct __MRTSTR_CONTAIN_CHR_T));
            data->chr = block;
            data->size = size;
            data->lock = str->lock + i;
            data->mutex = &str->mutex;
            data->res = res;

            while (!mrtstr_threads.free_threads && data->size > 65536)
            {
                for (j = 0; j < 65536; sblock++, j++)
                    if (mrtstr_simd_cmpeq_mask_func(*sblock, block))
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        return;
                    }

                data->size -= 65536;
            }

            if (data->size <= 65536)
            {
                for (; data->size; sblock++, data->size--)
                    if (mrtstr_simd_cmpeq_mask_func(*sblock, block))
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        return;
                    }

                mrstr_free(data);
            }
            else
            {
                data->str = sblock;
                sblock += data->size;

                str->lock[i] = 1;
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_contain_chr_threaded, data);
            }
        }
    else
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            data = mrstr_alloc(sizeof(struct __MRTSTR_CONTAIN_CHR_T));
            data->chr = block;
            data->size = size;
            data->lock = str->lock + i;
            data->mutex = &str->mutex;
            data->res = res;

            while (!mrtstr_threads.free_threads && data->size > 65536)
            {
                for (j = 0; j < 65536; sblock++, j++)
                    if (mrtstr_simd_cmpeq_mask_func(*sblock, block))
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        return;
                    }

                data->size -= 65536;
            }

            if (data->size <= 65536)
            {
                for (; data->size; sblock++, data->size--)
                    if (mrtstr_simd_cmpeq_mask_func(*sblock, block))
                    {
                        res->res = MRTSTR_TRUE;

                        mrstr_free(data);
                        return;
                    }

                mrstr_free(data);
            }
            else
            {
                data->str = sblock;
                sblock += data->size;

                mrtstr_lock_inc(str->lock[i], &str->mutex);
                mrtstr_lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_contain_chr_threaded, data);
            }
        }

    mrtstr_bool_t rres = memchr((mrtstr_data_ct)sblock, chr, rem) != NULL;
    if (!rres)
        res->res = MRTSTR_FALSE;

    str->forced = MRTSTR_FALSE;
}

void mrtstr_contain_chr_threaded(void *args)
{
    mrtstr_contain_chr_t data = (mrtstr_contain_chr_t)args;

    mrtstr_size_t i;
    while (data->size > 65536)
    {
        if (data->res->res)
        {
            mrtstr_lock_dec(*data->lock, data->mutex);
            mrtstr_lock_dec(data->res->lock, &data->res->mutex);

            mrstr_free(data);
            return;
        }

        for (i = 0; i < 65536; data->str++, i++)
            if (mrtstr_simd_cmpeq_mask_func(*data->str, data->chr))
            {
                data->res->res = MRTSTR_TRUE;

                mrtstr_lock_dec(*data->lock, data->mutex);
                mrtstr_lock_dec(data->res->lock, &data->res->mutex);

                mrstr_free(data);
                return;
            }

        data->size -= 65536;
    }

    if (data->res->res)
    {
        mrtstr_lock_dec(*data->lock, data->mutex);
        mrtstr_lock_dec(data->res->lock, &data->res->mutex);

        mrstr_free(data);
        return;
    }

    for (; data->size; data->str++, data->size--)
        if (mrtstr_simd_cmpeq_mask_func(*data->str, data->chr))
        {
            data->res->res = MRTSTR_TRUE;

            mrtstr_lock_dec(*data->lock, data->mutex);
            mrtstr_lock_dec(data->res->lock, &data->res->mutex);

            mrstr_free(data);
            return;
        }

    mrtstr_lock_dec(*data->lock, data->mutex);
    mrtstr_lock_dec(data->res->lock, &data->res->mutex);

    mrstr_free(data);
}
