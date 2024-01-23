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

#define mrfstr_countchr_rem \
    while (rem--)           \
        if (*str++ == chr)  \
            count++

#pragma pack(push, 1)
struct __MRFSTR_COUNTCHR_T
{
    mrfstr_data_ct str;
    mrfstr_chr_t chr;
    mrfstr_size_t size;

    volatile mrfstr_size_t *count;
    mrfstr_mutex_p mutex;
};
#pragma pack(pop)
typedef struct __MRFSTR_COUNTCHR_T *mrfstr_countchr_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_countchr_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_countchr_threaded(
    LPVOID args);
#endif

mrfstr_size_t __mrfstr_countchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        mrfstr_size_t count = 0;
        while (size--)
            if (*str++ == chr)
                count++;

        return count;
    }

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_size_t count = 0;
        mrfstr_byte_t rem = (uintptr_t)str % _mrfstr_config.nsearch_size;
        if (rem)
        {
            rem = _mrfstr_config.nsearch_size - rem;
            size -= rem;
            mrfstr_countchr_rem;
        }

        rem = size % _mrfstr_config.nsearch_size;
        size -= rem;

        count += _mrfstr_config.ncountchr_sub(
            str, chr, size / _mrfstr_config.nsearch_size);
        str += size;

        mrfstr_countchr_rem;
        return count;
    }

    mrfstr_size_t tsize = _mrfstr_config.tlimit >> 1;
    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * tsize)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / tsize);

    volatile mrfstr_size_t count = 0;
    mrfstr_short_t rem = (uintptr_t)str % _mrfstr_config.tsearch_size;
    if (rem)
    {
        rem = _mrfstr_config.tsearch_size - rem;
        size -= rem;
        mrfstr_countchr_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.tsearch_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.tsearch_size;

    mrfstr_mutex_t mutex;
    mrfstr_create_mutex(mutex)
    {
        count += _mrfstr_config.ncountchr_sub(
            str, chr, size * tcount);
        str += inc * tcount;

        mrfstr_countchr_rem;
        return count;
    }

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_countchr_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = (mrfstr_countchr_t)malloc(sizeof(struct __MRFSTR_COUNTCHR_T));
            if (!data)
                break;

            data->str = str;
            data->chr = chr;
            data->size = size;
            data->count = &count;
            data->mutex = MRFSTR_CAST_MUTEX(mutex);

            str += inc;

            mrfstr_create_thread(__mrfstr_countchr_threaded)
            {
                str -= inc;
                free(data);
                break;
            }
        }

        tcount -= i;
    }

    mrfstr_size_t tmp = _mrfstr_config.tcountchr_sub(
        str, chr, size * tcount);
    str += inc * tcount;

    while (rem--)
        if (*str++ == chr)
            tmp++;

    mrfstr_lock_mutex(MRFSTR_CAST_MUTEX(mutex));
    count += tmp;
    mrfstr_unlock_mutex(MRFSTR_CAST_MUTEX(mutex));

    mrfstr_close_threads;
    mrfstr_close_mutex(mutex);
    free(threads);
    return count;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_countchr_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_countchr_threaded(
    LPVOID args)
#endif
{
    mrfstr_countchr_t data = (mrfstr_countchr_t)args;
    mrfstr_size_t count = _mrfstr_config.tcountchr_sub(
        data->str, data->chr, data->size);

    mrfstr_lock_mutex(data->mutex);
    *data->count += count;
    mrfstr_unlock_mutex(data->mutex);

    free(data);
    return MRFSTR_TFUNC_RET;
}
