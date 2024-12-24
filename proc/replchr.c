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

#define mrfstr_replchr_rem    \
    for (; rem; rem--, str++) \
        if (*str == ochr)     \
            *str = nchr

#define mrfstr_replchr2_rem   \
    for (; rem; rem--, str++) \
        *res++ = *str == ochr ? nchr : *str

#pragma pack(push, 1)
struct __MRFSTR_REPLCHR_T
{
    mrfstr_data_t str;
    mrfstr_size_t size;

    mrfstr_chr_t ochr;
    mrfstr_chr_t nchr;
};
#pragma pack(pop)
typedef struct __MRFSTR_REPLCHR_T *mrfstr_replchr_t;

#pragma pack(push, 1)
struct __MRFSTR_REPLCHR2_T
{
    mrfstr_data_t res;
    mrfstr_data_ct str;
    mrfstr_size_t size;

    mrfstr_chr_t ochr;
    mrfstr_chr_t nchr;
};
#pragma pack(pop)
typedef struct __MRFSTR_REPLCHR2_T *mrfstr_replchr2_t;

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_replchr_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replchr_threaded(
    LPVOID args);
#endif

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_replchr2_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replchr2_threaded(
    LPVOID args);
#endif

void __mrfstr_replchr(
    mrfstr_data_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
{
    mrfstr_ushort_t rem;
    mrfstr_ubyte_t tcount, nthreads, i;
    mrfstr_thread_t *threads;
    mrfstr_replchr_t data;

    if (size < MRFSTR_SLIMIT)
    {
        for (; size; size--, str++)
            if (*str == ochr)
                *str = nchr;
        return;
    }

    if (size < _mrfstr_config.replchr_tlimit || _mrfstr_config.tcount == 1)
    {
        rem = (uintptr_t)str & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            rem = MRFSTR_ALIGN_SIZE - rem;
            size -= rem;
            mrfstr_replchr_rem;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size -= rem;
        _mrfstr_config.replchr_func(str += size, ochr, nchr,
            (mrfstr_size_t)-(mrfstr_ssize_t)size);

        mrfstr_replchr_rem;
        return;
    }

    mrfstr_set_tcount(_mrfstr_config.replchr_tlimit);

    rem = (uintptr_t)str & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        rem = MRFSTR_ALIGN_SIZE - rem;
        size -= rem;
        mrfstr_replchr_rem;
    }

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (mrfstr_size_t)-(mrfstr_ssize_t)((size - rem) / tcount);

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        size *= tcount;
        _mrfstr_config.replchr_tfunc(str -= size, ochr, nchr, size);

        mrfstr_replchr_rem;
        return;
    }

    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_replchr_t)malloc(sizeof(struct __MRFSTR_REPLCHR_T));
        if (!data)
            break;

        data->str = str -= size;
        data->size = size;
        data->ochr = ochr;
        data->nchr = nchr;

        mrfstr_create_thread(__mrfstr_replchr_threaded)
        {
            str += size;

            free(data);
            break;
        }

        mrfstr_thread_priority;
    }

    tcount -= i;
    size *= tcount;
    _mrfstr_config.replchr_tfunc(str -= size, ochr, nchr, size);

    mrfstr_replchr_rem;

    mrfstr_close_threads;
    free(threads);
}

void __mrfstr_replchr2(
    mrfstr_data_t res, mrfstr_data_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
{
    mrfstr_ushort_t rem;
    mrfstr_ubyte_t tcount, nthreads, i;
    mrfstr_thread_t *threads;
    mrfstr_replchr2_t data;

    if (size < MRFSTR_SLIMIT)
    {
        for (; size; size--, str++)
            *res++ = *str == ochr ? nchr : *str;
        return;
    }

    if (size < _mrfstr_config.replchr_tlimit || _mrfstr_config.tcount == 1)
    {
        rem = (uintptr_t)res & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            rem = MRFSTR_ALIGN_SIZE - rem;
            size -= rem;
            mrfstr_replchr2_rem;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size -= rem;
        _mrfstr_config.replchr2_func(res += size, str += size, ochr, nchr,
            (mrfstr_size_t)-(mrfstr_ssize_t)size);

        mrfstr_replchr2_rem;
        return;
    }

    mrfstr_set_tcount(_mrfstr_config.replchr_tlimit);

    rem = (uintptr_t)res & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        rem = MRFSTR_ALIGN_SIZE - rem;
        size -= rem;
        mrfstr_replchr2_rem;
    }

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (mrfstr_size_t)-(mrfstr_ssize_t)((size - rem) / tcount);

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        size *= tcount;
        _mrfstr_config.replchr2_tfunc(res -= size, str -= size, ochr, nchr, size);

        mrfstr_replchr2_rem;
        return;
    }

    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_replchr2_t)malloc(sizeof(struct __MRFSTR_REPLCHR2_T));
        if (!data)
            break;

        data->res = res -= size;
        data->str = str -= size;
        data->size = size;
        data->ochr = ochr;
        data->nchr = nchr;

        mrfstr_create_thread(__mrfstr_replchr2_threaded)
        {
            res += size;
            str += size;

            free(data);
            break;
        }

        mrfstr_thread_priority;
    }

    tcount -= i;
    size *= tcount;
    _mrfstr_config.replchr2_tfunc(res -= size, str -= size, ochr, nchr, size);

    mrfstr_replchr2_rem;

    mrfstr_close_threads;
    free(threads);
}

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_replchr_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replchr_threaded(
    LPVOID args)
#endif
{
    mrfstr_replchr_t data;

    data = (mrfstr_replchr_t)args;
    _mrfstr_config.replchr_tfunc(data->str, data->ochr, data->nchr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_replchr2_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replchr2_threaded(
    LPVOID args)
#endif
{
    mrfstr_replchr2_t data;

    data = (mrfstr_replchr2_t)args;
    _mrfstr_config.replchr2_tfunc(
        data->res, data->str, data->ochr, data->nchr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
