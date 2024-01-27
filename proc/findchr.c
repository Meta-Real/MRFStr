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

    mrfstr_byte_t step;
    mrfstr_byte_t start;

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
        mrfstr_data_t ptr = (mrfstr_data_t)memchr(str, chr, size);
        return ptr ? (mrfstr_idx_t)(ptr - str) : MRFSTR_INVIDX;
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

    mrfstr_byte_t align = (uintptr_t)str % _mrfstr_config.tsearch_size;
    if (align)
    {
        align = _mrfstr_config.tsearch_size - align;
        size -= align;

        for (mrfstr_byte_t i = 0; i < align; i++)
            if (*str++ == chr)
                return i;
    }

    mrfstr_short_t factor = _mrfstr_config.tsearch_size * tcount;
    mrfstr_short_t rem = size % factor;
    size = (size / factor) * factor;

    volatile mrfstr_idx_t res = size;

    mrfstr_mutex_t mutex;
    mrfstr_create_mutex(mutex)
    {
        mrfstr_idx_t idx = _mrfstr_config.nfindchr_sub(
            str, chr, size / _mrfstr_config.tsearch_size);
        if (idx != MRFSTR_INVIDX)
            return idx + align;

        str += size;
        mrfstr_findchr_rem;
        return MRFSTR_INVIDX;
    }

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_findchr_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = (mrfstr_findchr_t)malloc(sizeof(struct __MRFSTR_FINDCHR_T));
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

                    str += size * _mrfstr_config.tsearch_size;
                    mrfstr_findchr_rem;

                    mrfstr_close_mutex(mutex);
                    free(threads);
                    return MRFSTR_INVIDX;
                }
                break;
            }

            data->str = str;
            data->chr = chr;
            data->start = i;
            data->step = tcount;
            data->res = &res;
            data->mutex = MRFSTR_CAST_MUTEX(mutex);

            mrfstr_create_thread(__mrfstr_findchr_threaded)
            {
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

                    str += size * _mrfstr_config.tsearch_size;
                    mrfstr_findchr_rem;

                    mrfstr_close_mutex(mutex);
                    free(threads);
                    return MRFSTR_INVIDX;
                }
                break;
            }
        }
    }

    mrfstr_idx_t idx = _mrfstr_config.tfindchr_sub(
        &res, nthreads, str, chr, tcount);
    mrfstr_lock_mutex(MRFSTR_CAST_MUTEX(mutex));
    if (res > idx)
        res = idx;
    mrfstr_unlock_mutex(MRFSTR_CAST_MUTEX(mutex));

    str += size;

    mrfstr_bool_t end = MRFSTR_FALSE;
    for (; rem; rem--, str++)
        if (*str == chr)
        {
            idx = (mrfstr_idx_t)(str - base);
            end = MRFSTR_TRUE;
            break;
        }

    mrfstr_close_threads;
    mrfstr_close_mutex(mutex);
    free(threads);

    if (end && res == size)
        return idx;
    return res == size ? MRFSTR_INVIDX : res + align;
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
        data->res, data->start, data->str, data->chr, data->step);
    mrfstr_lock_mutex(data->mutex);
    if (*data->res > idx)
        *data->res = idx;
    mrfstr_unlock_mutex(data->mutex);

    free(data);
    return MRFSTR_TFUNC_RET;
}
