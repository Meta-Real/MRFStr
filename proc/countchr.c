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

struct __MRFSTR_COUNTCHR_T
{
    volatile mrfstr_size_t *count;
    mrfstr_mutex_p mutex;

    mrfstr_data_ct str;
    mrfstr_size_t size;
    mrfstr_chr_t chr;
};
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
    mrfstr_size_t tmp;
    mrfstr_ushort_t rem;
    mrfstr_ubyte_t tcount, nthreads, i;
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

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.searchchr_tlimit)
    {
        count = 0;
        rem = (mrfstr_ulong_t)str & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            rem = MRFSTR_ALIGN_SIZE - rem;
            size -= rem;
            mrfstr_countchr_rem;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size = (mrfstr_size_t)-(mrfstr_long_t)(size - rem);

single:
        count += _mrfstr_config.countchr_func(str -= size, chr, size);

        mrfstr_countchr_rem;
        return count;
    }

    mrfstr_set_tcount(_mrfstr_config.searchchr_tlimit);

    count = 0;
    rem = (mrfstr_ulong_t)str & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        rem = MRFSTR_ALIGN_SIZE - rem;
        size -= rem;
        mrfstr_countchr_rem;
    }

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (mrfstr_size_t)-(mrfstr_long_t)((size - rem) / tcount);

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

    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_countchr_t)malloc(sizeof(struct __MRFSTR_COUNTCHR_T));
        if (!data)
            break;

        data->count = &count;
        data->mutex = MRFSTR_CAST_MUTEX(mutex);
        data->str = str -= size;
        data->size = size;
        data->chr = chr;

        mrfstr_create_thread(__mrfstr_countchr_threaded)
        {
            str += size;

            free(data);
            break;
        }

        mrfstr_thread_priority;
    }

    tcount -= i;

    size *= tcount;
    tmp = _mrfstr_config.countchr_tfunc(str -= size, chr, size);

    while (rem--)
        if (*str++ == chr)
            tmp++;

    mrfstr_close_threads;
    free(threads);
    mrfstr_close_mutex(mutex);

    return count + tmp;
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
    count = _mrfstr_config.countchr_tfunc(data->str, data->chr, data->size);

    mrfstr_lock_mutex(data->mutex);
    *data->count += count;
    mrfstr_unlock_mutex(data->mutex);

    free(data);
    return MRFSTR_TFUNC_RET;
}
