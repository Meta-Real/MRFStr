/*
MIT License

Copyright (c) 2023 MetaReal

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/

#include <mrfstr-intern.h>
#include <string.h>

#define mrfstr_findchr_rem    \
    for (; rem; rem--, str++) \
        if (chr == *str)      \
            return (mrfstr_short_t)(str - base)

#pragma pack(push, 1)
struct __MRFSTR_FINDCHR_T
{
    mrfstr_data_ct str;
    mrfstr_chr_t chr;
    mrfstr_size_t size;
    mrfstr_idx_t start;

    volatile mrfstr_idx_t *res;
    mrfstr_mutex_p mutex;
};
#pragma pack(pop)
typedef struct __MRFSTR_FINDCHR_T *mrfstr_findchr_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_findchr_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_findchr_threaded(
    LPVOID args);
#endif

mrfstr_idx_t __mrfstr_findchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        mrfstr_data_t ptr = memchr(str, chr, size);
        if (ptr)
            return (mrfstr_idx_t)(ptr - str);
        return MRFSTR_INVIDX;
    }

    mrfstr_data_ct base = str;
    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_byte_t rem = (uintptr_t)str % _mrfstr_config.nsearch_size;
        if (rem)
        {
            rem = _mrfstr_config.nsearch_size - rem;
            size -= rem;
            mrfstr_findchr_rem;
        }

        rem = size % _mrfstr_config.nsearch_size;
        size -= rem;

        mrfstr_idx_t idx = _mrfstr_config.nfindchr_sub(
            str, chr, size / _mrfstr_config.nsearch_size);
        if (idx != MRFSTR_INVIDX)
            return idx + (mrfstr_idx_t)(uintptr_t)(str - base);
        str += size;

        mrfstr_findchr_rem;
        return MRFSTR_INVIDX;
    }

    mrfstr_size_t tsize = _mrfstr_config.tlimit >> 1;
    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * tsize)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / tsize);

    mrfstr_short_t rem = (uintptr_t)str % _mrfstr_config.tsearch_size;
    if (rem)
    {
        rem = _mrfstr_config.tsearch_size - rem;
        size -= rem;
        mrfstr_findchr_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.tsearch_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.tsearch_size;

    volatile mrfstr_idx_t res = MRFSTR_INVIDX;

    mrfstr_mutex_t mutex;
    mrfstr_create_mutex(mutex)
    {
        mrfstr_idx_t idx = _mrfstr_config.nfindchr_sub(
            str, chr, size * tcount);
        if (idx != MRFSTR_INVIDX)
            return idx + (mrfstr_idx_t)(uintptr_t)(str - base);
        str += inc * tcount;

        mrfstr_findchr_rem;
        return MRFSTR_INVIDX;
    }

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_findchr_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_FINDCHR_T));
            if (!data)
            {
                if (!i)
                {
                    mrfstr_idx_t idx = _mrfstr_config.nfindchr_sub(
                        str, chr, size * tcount);
                    if (idx != MRFSTR_INVIDX)
                    {
                        mrfstr_close_mutex(mutex);
                        free(threads);
                        return idx + (mrfstr_idx_t)(uintptr_t)(str - base);
                    }

                    str += inc * tcount;
                    mrfstr_findchr_rem;

                    mrfstr_close_mutex(mutex);
                    free(threads);
                    return MRFSTR_INVIDX;
                }
                break;
            }

            data->str = str;
            data->chr = chr;
            data->size = size;
            data->start = (mrfstr_idx_t)(uintptr_t)(str - base);
            data->res = &res;
            data->mutex = MRFSTR_CAST_MUTEX(mutex);

            str += inc;

            mrfstr_create_thread(__mrfstr_findchr_threaded)
            {
                str -= inc;
                free(data);

                if (!i)
                {
                    mrfstr_idx_t idx = _mrfstr_config.nfindchr_sub(
                        str, chr, size * tcount);
                    if (idx != MRFSTR_INVIDX)
                    {
                        mrfstr_close_mutex(mutex);
                        free(threads);
                        return idx + (mrfstr_idx_t)(uintptr_t)(str - base);
                    }

                    str += inc * tcount;
                    mrfstr_findchr_rem;

                    mrfstr_close_mutex(mutex);
                    free(threads);
                    return MRFSTR_INVIDX;
                }
                break;
            }
        }

        tcount -= i;
    }

    mrfstr_idx_t idx = _mrfstr_config.tfindchr_sub(
        &res, (mrfstr_idx_t)(str - base), str, chr, size * tcount);
    if (idx != MRFSTR_INVIDX)
    {
        mrfstr_lock_mutex(MRFSTR_CAST_MUTEX(mutex));
        if (res > idx)
            res = idx;
        mrfstr_unlock_mutex(MRFSTR_CAST_MUTEX(mutex));
    }
    str += inc * tcount;

    mrfstr_findchr_rem;

    mrfstr_close_threads;
    mrfstr_close_mutex(mutex);
    free(threads);
    return res;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_findchr_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_findchr_threaded(
    LPVOID args)
#endif
{
    mrfstr_findchr_t data = (mrfstr_findchr_t)args;
    mrfstr_idx_t idx = _mrfstr_config.tfindchr_sub(
        data->res, data->start, data->str, data->chr, data->size);
    if (idx != MRFSTR_INVIDX)
    {
        mrfstr_lock_mutex(data->mutex);
        if (*data->res > idx)
            *data->res = idx;
        mrfstr_unlock_mutex(data->mutex);
    }

    free(data);
    return MRFSTR_TFUNC_RET;
}
