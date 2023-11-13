/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <immintrin.h>
#include <string.h>

struct __MRTSTR_MEMCPY_T
{
    __m512i *src;
    __m512i *dst;
    mrtstr_size_t size;

    mrtstr_lock_t *slock;
    mrtstr_lock_t *dlock;
};
typedef struct __MRTSTR_MEMCPY_T *mrtstr_memcpy_t;

struct __MRTSTR_MEMCPY2_T
{
    __m512i *src;
    __m512i *dst;
    mrtstr_size_t size;

    mrtstr_lock_t *lock;
};
typedef struct __MRTSTR_MEMCPY2_T *mrtstr_memcpy2_t;

void mrtstr_memcpy_threaded(void *args);
void mrtstr_memcpy2_threaded(void *args);

void mrtstr_set(mrtstr_t dst, mrtstr_ct src)
{
    for (; mrtstr_locked(dst););

    if (!src->size)
    {
        if (!dst->size)
            return;

        *dst->data = '\0';
        dst->size = 0;
        return;
    }

    mrtstr_size_t size = src->size + 1;
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

    if (size <= 25166207)
    {
        mrtstr_size_t rem = size & 63;
        size >>= 6;

        for (; size; sblock++, dblock++, size--)
        {
            block = _mm512_stream_load_si512(sblock);
            _mm512_stream_si512(dblock, block);
        }

        memcpy((mrtstr_data_t)dblock, (mrtstr_data_t)sblock, rem);
        return;
    }

    mrtstr_size_t rem = size % 384;
    size /= 384;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_memcpy_t data;
    if (mrtstr_locked(src) && src->forced)
        for (i = 0; i < 6; i++)
        {
            for (; src->lock[i];);

            data = __mrstr_alloc_una(sizeof(struct __MRTSTR_MEMCPY_T));
            data->size = size;
            data->slock = src->lock + i;
            data->dlock = dst->lock + i;

            while (!mrtstr_threads.free_threads && data->size > 65536)
            {
                for (j = 0; j < 65536; sblock++, dblock++, j++)
                {
                    block = _mm512_stream_load_si512(sblock);
                    _mm512_stream_si512(dblock, block);
                }

                data->size -= 65536;
            }

            if (data->size <= 65536)
            {
                for (; data->size; sblock++, dblock++, data->size--)
                {
                    block = _mm512_stream_load_si512(sblock);
                    _mm512_stream_si512(dblock, block);
                }

                __mrstr_free(data);
            }
            else
            {
                data->src = sblock;
                data->dst = dblock;

                sblock += data->size;
                dblock += data->size;

                src->lock[i]++;
                dst->lock[i]++;
                mrtstr_load_threads(mrtstr_memcpy_threaded, data);
            }
        }
    else
        for (i = 0; i < 6; i++)
        {
            data = __mrstr_alloc_una(sizeof(struct __MRTSTR_MEMCPY_T));
            data->size = size;
            data->slock = src->lock + i;
            data->dlock = dst->lock + i;

            while (!mrtstr_threads.free_threads && data->size > 65536)
            {
                for (j = 0; j < 65536; sblock++, dblock++, j++)
                {
                    block = _mm512_stream_load_si512(sblock);
                    _mm512_stream_si512(dblock, block);
                }

                data->size -= 65536;
            }

            if (data->size <= 65536)
            {
                for (; data->size; sblock++, dblock++, data->size--)
                {
                    block = _mm512_stream_load_si512(sblock);
                    _mm512_stream_si512(dblock, block);
                }

                __mrstr_free_una(data);
            }
            else
            {
                data->src = sblock;
                data->dst = dblock;

                sblock += data->size;
                dblock += data->size;

                src->lock[i]++;
                dst->lock[i]++;
                mrtstr_load_threads(mrtstr_memcpy_threaded, data);
            }
        }

    memcpy((mrtstr_data_t)sblock, (mrtstr_data_ct)dblock, rem);
    dst->size = src->size;
    src->forced = MRTSTR_FALSE;
    dst->forced = MRTSTR_TRUE;
}

void mrtstr_set_str(mrtstr_t dst, mrtstr_data_ct src)
{
    for (; mrtstr_locked(dst););

    mrtstr_size_t size = strlen(src);
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

    if (size <= 25166207)
    {
        mrtstr_size_t rem = size & 63;
        size >>= 6;

        for (; size; sblock++, dblock++, size--)
        {
            block = _mm512_stream_load_si512(sblock);
            _mm512_stream_si512(dblock, block);
        }

        memcpy((mrtstr_data_t)dblock, (mrtstr_data_t)sblock, rem);
        return;
    }

    mrtstr_size_t rem = size % 384;
    size /= 384;

    mrtstr_size_t j;
    mrtstr_bit_t i;
    mrtstr_memcpy2_t data;
    for (i = 0; i < 6; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRTSTR_MEMCPY2_T));
        data->size = size;
        data->lock = dst->lock + i;

        while (!mrtstr_threads.free_threads && data->size > 65536)
        {
            for (j = 0; j < 65536; sblock++, dblock++, j++)
            {
                block = _mm512_stream_load_si512(sblock);
                _mm512_stream_si512(dblock, block);
            }

            data->size -= 65536;
        }

        if (data->size <= 65536)
        {
            for (; data->size; sblock++, dblock++, data->size--)
            {
                block = _mm512_stream_load_si512(sblock);
                _mm512_stream_si512(dblock, block);
            }

            __mrstr_free_una(data);
        }
        else
        {
            data->src = sblock;
            data->dst = dblock;

            sblock += data->size;
            dblock += data->size;

            dst->lock[i]++;
            mrtstr_load_threads(mrtstr_memcpy2_threaded, data);
        }
    }

    memcpy((mrtstr_data_t)sblock, (mrtstr_data_t)dblock, rem);
    dst->forced = MRTSTR_TRUE;
}

void mrtstr_nset_str(mrtstr_t dst, mrtstr_data_ct src, mrtstr_size_t size)
{
    for (; mrtstr_locked(dst););

    if (!size)
    {
        if (!dst->size)
            return;

        *dst->data = '\0';
        dst->size = 0;
        return;
    }

    if (dst->alloc <= size)
    {
        if (dst->alloc)
            __mrstr_free(dst->data);

        dst->alloc = size + 1;
        dst->data = __mrstr_alloc(dst->alloc);
    }

    dst->size = size;

    __m512i *sblock = (__m512i*)src;
    __m512i *dblock = (__m512i*)dst->data;
    __m512i block;

    if (size <= 25166207)
    {
        mrtstr_size_t rem = size & 63;
        size >>= 6;

        for (; size; sblock++, dblock++, size--)
        {
            block = _mm512_stream_load_si512(sblock);
            _mm512_stream_si512(dblock, block);
        }

        mrtstr_data_t dptr = (mrtstr_data_t)dblock;
        memcpy(dptr, (mrtstr_data_t)sblock, rem);
        dptr[rem] = '\0';
        return;
    }

    mrtstr_size_t rem = size % 384;
    size /= 384;

    mrtstr_size_t j;
    mrtstr_bit_t i;
    mrtstr_memcpy2_t data;
    for (i = 0; i < 6; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRTSTR_MEMCPY2_T));
        data->size = size;
        data->lock = dst->lock + i;

        while (!mrtstr_threads.free_threads && data->size > 65536)
        {
            for (j = 0; j < 65536; sblock++, dblock++, j++)
            {
                block = _mm512_stream_load_si512(sblock);
                _mm512_stream_si512(dblock, block);
            }

            data->size -= 65536;
        }

        if (data->size <= 65536)
        {
            for (; data->size; sblock++, dblock++, data->size--)
            {
                block = _mm512_stream_load_si512(sblock);
                _mm512_stream_si512(dblock, block);
            }

            __mrstr_free_una(data);
        }
        else
        {
            data->src = sblock;
            data->dst = dblock;

            sblock += data->size;
            dblock += data->size;

            dst->lock[i]++;
            mrtstr_load_threads(mrtstr_memcpy2_threaded, data);
        }
    }

    mrtstr_data_t dptr = (mrtstr_data_t)dblock;
    memcpy(dptr, (mrtstr_data_t)sblock, rem);
    dptr[rem] = '\0';
    dst->forced = MRTSTR_TRUE;
}

void mrtstr_memcpy_threaded(void *args)
{
    mrtstr_memcpy_t data = (mrtstr_memcpy_t)args;

    __m512i block;
    for (; data->size; data->src++, data->dst++, data->size--)
    {
        block = _mm512_stream_load_si512(data->src);
        _mm512_stream_si512(data->dst, block);
    }

    --*data->slock;
    --*data->dlock;

    __mrstr_free_una(data);
}

void mrtstr_memcpy2_threaded(void *args)
{
    mrtstr_memcpy2_t data = (mrtstr_memcpy2_t)args;

    __m512i block;
    for (; data->size; data->src++, data->dst++, data->size--)
    {
        block = _mm512_stream_load_si512(data->src);
        _mm512_stream_si512(data->dst, block);
    }

    --*data->lock;

    __mrstr_free_una(data);
}
