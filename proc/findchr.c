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

// temporary
#include <stdlib.h>

#define mrfstr_findchr_rem    \
    for (; rem; rem--, str++) \
        if (chr == *str)      \
            return (mrfstr_short_t)(str - base)

#pragma pack(push, 1)
struct __MRFSTR_FINDCHR_T
{
    volatile mrfstr_idx_t *res;
    mrfstr_mutex_p mutex;

    mrfstr_data_ct str;
    mrfstr_short_t step;
    mrfstr_byte_t start;

    mrfstr_chr_t chr;
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
    mrfstr_size_t tsize;
    mrfstr_idx_t idx;
    mrfstr_short_t rem, tcount, factor;
    mrfstr_byte_t align, i, j;
    mrfstr_bool_t endf;
    volatile mrfstr_idx_t res;
    mrfstr_thread_t *threads;
    mrfstr_findchr_t data;
    mrfstr_mutex_t mutex;

    if (size < MRFSTR_SLIMIT)
    {
        mrfstr_data_t ptr;

        ptr = (mrfstr_data_t)memchr(str, chr, size);
        return ptr ? (mrfstr_idx_t)(ptr - str) : MRFSTR_INVIDX;
    }

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_byte_t mask;

        mask = _mrfstr_config.nsearch_size - 1;
        align = (uintptr_t)str & mask;
        if (align)
        {
            align = _mrfstr_config.nsearch_size - align;
            size -= align;

            for (i = 0; i != align; i++)
                if (*str++ == chr)
                    return i;
        }

        rem = size & mask;
        size -= rem;

        idx = _mrfstr_config.nfindchr_sub(
            str, chr, size / _mrfstr_config.nsearch_size);
        if (idx != MRFSTR_INVIDX)
            return idx + align;
        str += size;

        for (i = 0; i != rem; i++)
            if (*str++ == chr)
                return align + size + i;
        return MRFSTR_INVIDX;
    }

    mrfstr_set_tcount;

    align = (uintptr_t)str & _mrfstr_config.tsearch_size - 1;
    if (align)
    {
        align = _mrfstr_config.tsearch_size - align;
        size -= align;

        for (i = 0; i < align; i++)
            if (*str++ == chr)
                return i;
    }

    factor = _mrfstr_config.tsearch_size * tcount;
    rem = size % factor;
    size = (size / factor) * factor;

    res = size;
    mrfstr_create_mutex(mutex)
    {
single:
        idx = _mrfstr_config.nfindchr_sub(
            str, chr, size / _mrfstr_config.tsearch_size);
        if (idx != MRFSTR_INVIDX)
            return idx + align;

        str += size;
        for (i = 0; i != rem; i++)
            if (*str++ == chr)
                return align + size + i;
        return MRFSTR_INVIDX;
    }

    factor = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(factor * sizeof(mrfstr_thread_t));
    if (!threads)
        goto single;

    tcount *= _mrfstr_config.tsearch_size;
    for (i = 0; i != factor; i++)
    {
        data = (mrfstr_findchr_t)malloc(sizeof(struct __MRFSTR_FINDCHR_T));
        if (!data)
        {
            if (!i)
            {
                mrfstr_close_mutex(mutex);
                free(threads);
                goto single;
            }

            // temporary
            abort();
            break;
        }

        data->res = &res;
        data->mutex = MRFSTR_CAST_MUTEX(mutex);
        data->str = str;
        data->start = i;
        data->step = tcount;
        data->chr = chr;

        mrfstr_create_thread(__mrfstr_findchr_threaded)
        {
            free(data);
            if (!i)
            {
                mrfstr_close_mutex(mutex);
                free(threads);
                goto single;
            }

            // temporary
            abort();
            break;
        }
    }

    idx = _mrfstr_config.tfindchr_sub(&res, factor, str, chr, tcount);
    mrfstr_lock_mutex(MRFSTR_CAST_MUTEX(mutex));
    if (res > idx)
        res = idx;
    mrfstr_unlock_mutex(MRFSTR_CAST_MUTEX(mutex));

    str += size;

    endf = MRFSTR_FALSE;
    for (j = 0; j != rem; j++)
        if (*str++ == chr)
        {
            idx = align + size + j;
            endf = MRFSTR_TRUE;
            break;
        }

    mrfstr_close_threads;
    free(threads);
    mrfstr_close_mutex(mutex);

    if (endf && res == size)
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
    mrfstr_idx_t idx;
    mrfstr_findchr_t data;

    data = (mrfstr_findchr_t)args;
    idx = _mrfstr_config.tfindchr_sub(
        data->res, data->start, data->str, data->chr, data->step);
    mrfstr_lock_mutex(data->mutex);
    if (*data->res > idx)
        *data->res = idx;
    mrfstr_unlock_mutex(data->mutex);

    free(data);
    return MRFSTR_TFUNC_RET;
}
