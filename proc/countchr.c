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
    volatile mrfstr_size_t *count;
    mrfstr_mutex_p mutex;

    mrfstr_data_ct str;
    mrfstr_size_t size;
    mrfstr_chr_t chr;
};
#pragma pack(pop)
typedef struct __MRFSTR_COUNTCHR_T *mrfstr_countchr_t;

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_countchr_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_countchr_threaded(
    LPVOID args);
#endif

mrfstr_size_t __mrfstr_countchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_size_t tsize, inc, tmp;
    mrfstr_short_t rem, factor;
    mrfstr_byte_t tcount, i;
    volatile mrfstr_size_t count;
    mrfstr_thread_t *threads;
    mrfstr_countchr_t data;
    mrfstr_mutex_t mutex;

    if (size < MRFSTR_SLIMIT)
    {
        count = 0;
        while (size--)
            if (*str++ == chr)
                count++;

        return count;
    }

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_byte_t mask;

        count = 0;
        mask = _mrfstr_config.nsearch_size - 1;
        rem = (uintptr_t)str & mask;
        if (rem)
        {
            rem = _mrfstr_config.nsearch_size - rem;
            size -= rem;
            mrfstr_countchr_rem;
        }

        rem = size & mask;
        size -= rem;

        count += _mrfstr_config.ncountchr_sub(
            str, chr, size / _mrfstr_config.nsearch_size);
        str += size;

        mrfstr_countchr_rem;
        return count;
    }

    mrfstr_set_tcount;

    count = 0;
    rem = (uintptr_t)str & (_mrfstr_config.tsearch_size - 1);
    if (rem)
    {
        rem = _mrfstr_config.tsearch_size - rem;
        size -= rem;
        mrfstr_countchr_rem;
    }

    factor = _mrfstr_config.tsearch_size * tcount;
    rem = size % factor;
    inc = (size /= factor) * _mrfstr_config.tsearch_size;

    mrfstr_create_mutex(mutex)
    {
single:
        count += _mrfstr_config.ncountchr_sub(
            str, chr, size * tcount);
        str += inc * tcount;

        mrfstr_countchr_rem;
        return count;
    }

    factor = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(factor * sizeof(mrfstr_thread_t));
    if (!threads)
        goto single;

    for (i = 0; i != factor; i++)
    {
        data = (mrfstr_countchr_t)malloc(sizeof(struct __MRFSTR_COUNTCHR_T));
        if (!data)
            break;

        data->count = &count;
        data->mutex = MRFSTR_CAST_MUTEX(mutex);
        data->str = str;
        data->size = size;
        data->chr = chr;

        str += inc;
        mrfstr_create_thread(__mrfstr_countchr_threaded)
        {
            str -= inc;

            free(data);
            break;
        }
    }

    tcount -= i;
    tmp = _mrfstr_config.tcountchr_sub(str, chr, size * tcount);
    str += inc * tcount;

    while (rem--)
        if (*str++ == chr)
            tmp++;

    mrfstr_lock_mutex(MRFSTR_CAST_MUTEX(mutex));
    count += tmp;
    mrfstr_unlock_mutex(MRFSTR_CAST_MUTEX(mutex));

    mrfstr_close_threads;
    free(threads);
    mrfstr_close_mutex(mutex);
    return count;
}

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_countchr_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_countchr_threaded(
    LPVOID args)
#endif
{
    mrfstr_size_t count;
    mrfstr_countchr_t data;

    data = (mrfstr_countchr_t)args;
    count = _mrfstr_config.tcountchr_sub(data->str, data->chr, data->size);

    mrfstr_lock_mutex(data->mutex);
    *data->count += count;
    mrfstr_unlock_mutex(data->mutex);

    free(data);
    return MRFSTR_TFUNC_RET;
}
