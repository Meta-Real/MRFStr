/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <immintrin.h>
#include <string.h>

struct __MRTSTR_SET_T
{
    __m512i *src;
    __m512i *dst;
    mrtstr_size_t size;

    mrtstr_lock_t *slock;
    pthread_mutex_t *smutex;

    mrtstr_lock_t *dlock;
};
typedef struct __MRTSTR_SET_T *mrtstr_set_t;

struct __MRTSTR_SET_STR_T
{
    __m512i *src;
    __m512i *dst;
    mrtstr_size_t size;

    mrtstr_lock_t *lock;
};
typedef struct __MRTSTR_SET_STR_T *mrtstr_set_str_t;

void mrtstr_set_threaded(void *args);
void mrtstr_set_str_threaded(void *args);

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

    if (size <= MRTSTR_THREAD_LIMIT)
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

    mrtstr_size_t rem = size % MRTSTR_THREAD_CHUNK;
    size /= MRTSTR_THREAD_CHUNK;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_set_t data;
    if (mrtstr_locked(src) && src->forced)
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            for (; src->lock[i];);

            data = __mrstr_alloc_una(sizeof(struct __MRTSTR_SET_T));
            data->size = size;
            data->slock = src->lock + i;
            data->smutex = &src->mutex;
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

                src->lock[i] = 1;
                dst->lock[i] = 1;
                mrtstr_load_threads(mrtstr_set_threaded, data);
            }
        }
    else
        for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        {
            data = __mrstr_alloc_una(sizeof(struct __MRTSTR_SET_T));
            data->size = size;
            data->slock = src->lock + i;
            data->smutex = &src->mutex;
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

                mrtstr_lock_inc(src->lock[i], &src->mutex);
                dst->lock[i] = 1;
                mrtstr_load_threads(mrtstr_set_threaded, data);
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

    if (size <= MRTSTR_THREAD_LIMIT)
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

    mrtstr_size_t rem = size % MRTSTR_THREAD_CHUNK;
    size /= MRTSTR_THREAD_CHUNK;

    mrtstr_size_t j;
    mrtstr_bit_t i;
    mrtstr_set_str_t data;
    for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRTSTR_SET_STR_T));
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

            dst->lock[i] = 1;
            mrtstr_load_threads(mrtstr_set_str_threaded, data);
        }
    }

    memcpy((mrtstr_data_t)sblock, (mrtstr_data_t)dblock, rem);
    dst->forced = MRTSTR_TRUE;
}

void mrtstr_set_nstr(mrtstr_t dst, mrtstr_data_ct src, mrtstr_size_t size)
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

    if (size <= MRTSTR_THREAD_LIMIT)
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
        return;
    }

    mrtstr_size_t rem = size % MRTSTR_THREAD_CHUNK;
    size /= MRTSTR_THREAD_CHUNK;

    mrtstr_size_t j;
    mrtstr_bit_t i;
    mrtstr_set_str_t data;
    for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRTSTR_SET_STR_T));
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

            dst->lock[i] = 1;
            mrtstr_load_threads(mrtstr_set_str_threaded, data);
        }
    }

    mrtstr_data_t dptr = (mrtstr_data_t)dblock;
    memcpy(dptr, (mrtstr_data_t)sblock, rem);

    dst->forced = MRTSTR_TRUE;
}

void mrtstr_set_threaded(void *args)
{
    mrtstr_set_t data = (mrtstr_set_t)args;

    __m512i block;
    for (; data->size; data->src++, data->dst++, data->size--)
    {
        block = _mm512_stream_load_si512(data->src);
        _mm512_stream_si512(data->dst, block);
    }

    mrtstr_lock_dec(*data->slock, data->smutex);
    *data->dlock = 0;

    __mrstr_free_una(data);
}

void mrtstr_set_str_threaded(void *args)
{
    mrtstr_set_str_t data = (mrtstr_set_str_t)args;

    __m512i block;
    for (; data->size; data->src++, data->dst++, data->size--)
    {
        block = _mm512_stream_load_si512(data->src);
        _mm512_stream_si512(data->dst, block);
    }

    *data->lock = 0;

    __mrstr_free_una(data);
}
