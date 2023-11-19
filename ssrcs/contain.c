/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#if MRFSTR_THREADING
#include <pthread.h>

struct __MRFSTR_CONTAIN_CHR_T
{
    mrfstr_simd_block_t *str;
    mrfstr_simd_block_t chr;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_CONTAIN_CHR_T *mrfstr_contain_chr_t;

void *mrfstr_contain_chr_threaded(void *args);
#endif

mrfstr_bool_t mrfstr_contain_chr(mrfstr_ct str, mrfstr_chr_t chr)
{
    if (!str->size)
        return MRFSTR_FALSE;

    mrfstr_simd_block_t *sblock = (mrfstr_simd_block_t*)str->data;
    mrfstr_simd_block_t cblock = mrfstr_simd_set1_func(chr);

    mrfstr_size_t size = str->size;

#if MRFSTR_THREADING
    if (size <= MRFSTR_THREAD_LIMIT)
    {
#endif
        mrfstr_size_t rem = size & MRFSTR_SIMD_CHAR_MASK;
        size >>= MRFSTR_SIMD_CHAR_SHIFT;

        for (; size; sblock++, size--)
            if (mrfstr_simd_cmpeq_mask_func(*sblock, cblock))
                return MRFSTR_TRUE;

        return memchr((mrfstr_data_ct)sblock, chr, rem) != NULL;
#if MRFSTR_THREADING
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    volatile mrfstr_bool_t res = MRFSTR_FALSE;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_contain_chr_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRFSTR_CONTAIN_CHR_T));
        data->str = sblock;
        data->chr = cblock;
        data->size = size;
        data->res = &res;

        sblock += size;
        pthread_create(threads + i, NULL, mrfstr_contain_chr_threaded, data);
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
void *mrfstr_contain_chr_threaded(void *args)
{
    mrfstr_contain_chr_t data = (mrfstr_contain_chr_t)args;

    mrfstr_size_t i;
    while (data->size > 65536)
    {
        if (*data->res)
        {
            __mrstr_free_una(data);
            return NULL;
        }

        for (i = 0; i < 65536; data->str++, i++)
            if (mrfstr_simd_cmpeq_mask_func(*data->str, data->chr))
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
        if (mrfstr_simd_cmpeq_mask_func(*data->str, data->chr))
        {
            *data->res = MRFSTR_TRUE;

            __mrstr_free_una(data);
            return NULL;
        }

    __mrstr_free_una(data);
    return NULL;
}
#endif
