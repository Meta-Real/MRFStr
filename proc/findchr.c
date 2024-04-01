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
    volatile mrfstr_idx_t *res;
    mrfstr_mutex_p mutex;
    mrfstr_idx_t start;

    mrfstr_data_ct str;
    mrfstr_size_t size;
    mrfstr_chr_t chr;
};
#pragma pack(pop)
typedef struct __MRFSTR_FINDCHR_T *mrfstr_findchr_t;

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_findchr_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_findchr_threaded(
    LPVOID args);
#endif

mrfstr_idx_t __mrfstr_findchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_idx_t idx, start;
    mrfstr_short_t rem, i;
    mrfstr_byte_t align, tcount, nthreads;
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

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.searchchr_tlimit)
    {
        align = (uintptr_t)str & MRFSTR_ALIGN_MASK;
        if (align)
        {
            align = MRFSTR_ALIGN_SIZE - align;
            size -= align;

            for (i = 0; i != align; i++)
                if (*str++ == chr)
                    return i;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size = (mrfstr_size_t)-(mrfstr_ssize_t)(size - rem);

single:
        idx = _mrfstr_config.findchr_func(str -= size, chr, size);
        if (idx != MRFSTR_INVIDX)
            return idx + align;

        for (i = 0; i != rem; i++)
            if (*str++ == chr)
                return align - size + i;
        return MRFSTR_INVIDX;
    }

    mrfstr_set_tcount(_mrfstr_config.searchchr_tlimit);

    align = (uintptr_t)str & MRFSTR_ALIGN_MASK;
    if (align)
    {
        align = MRFSTR_ALIGN_SIZE - align;
        size -= align;

        for (i = 0; i < align; i++)
            if (*str++ == chr)
                return i;
    }

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (mrfstr_size_t)-(mrfstr_ssize_t)((size - rem) / tcount);

    mrfstr_create_mutex(mutex)
    {
        size *= tcount;
        goto single;
    }

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        size *= tcount;
        goto single;
    }

    res = MRFSTR_INVIDX;
    start = 0;
    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_findchr_t)malloc(sizeof(struct __MRFSTR_FINDCHR_T));
        if (!data)
        {
            if (!i)
            {
                mrfstr_close_mutex(mutex);
                free(threads);

                size *= tcount;
                goto single;
            }
            break;
        }

        data->res = &res;
        data->mutex = MRFSTR_CAST_MUTEX(mutex);
        data->start = start;
        data->str = str -= size;
        data->size = size;
        data->chr = chr;

        mrfstr_create_thread(__mrfstr_findchr_threaded)
        {
            str += size;

            free(data);
            if (!i)
            {
                mrfstr_close_mutex(mutex);
                free(threads);

                size *= tcount;
                goto single;
            }
            break;
        }

        start -= size;
        mrfstr_thread_priority;
    }

    tcount -= (mrfstr_byte_t)i;

    size *= tcount;
    idx = _mrfstr_config.findchr_tfunc(&res, start, str -= size, chr, size);
    mrfstr_lock_mutex(MRFSTR_CAST_MUTEX(mutex));
    if (res > idx)
        res = idx;
    mrfstr_unlock_mutex(MRFSTR_CAST_MUTEX(mutex));

    mrfstr_close_threads;
    free(threads);
    mrfstr_close_mutex(mutex);

    if (res == MRFSTR_INVIDX)
        for (i = 0; i != rem; i++)
            if (*str++ == chr)
                return i - size + start + align;
    return res + align;
}

#ifdef MRFSTR_BUILD_UNIX
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
    idx = _mrfstr_config.findchr_tfunc(data->res, data->start, data->str, data->chr, data->size);
    mrfstr_lock_mutex(data->mutex);
    if (*data->res > idx)
        *data->res = idx;
    mrfstr_unlock_mutex(data->mutex);

    free(data);
    return MRFSTR_TFUNC_RET;
}
