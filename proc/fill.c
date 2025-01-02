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

#define mrfstr_fill_rem \
    while (rem--)       \
        *res++ = chr

struct __MRFSTR_FILL_T
{
    mrfstr_data_t res;
    mrfstr_size_t size;
    mrfstr_chr_t chr;
};
typedef struct __MRFSTR_FILL_T *mrfstr_fill_t;

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_fill_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_fill_threaded(
    LPVOID args);
#endif

void __mrfstr_fill(
    mrfstr_data_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_ushort_t rem;
    mrfstr_ubyte_t tcount, nthreads, i;
    mrfstr_thread_t *threads;
    mrfstr_fill_t data;

    if (size < MRFSTR_SLIMIT)
    {
        memset(res, chr, size);
        return;
    }

    if (size < _mrfstr_config.mem_tlimit || _mrfstr_config.tcount == 1)
    {
        rem = (mrfstr_ulong_t)res & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            rem = MRFSTR_ALIGN_SIZE - rem;
            size -= rem;
            mrfstr_fill_rem;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size -= rem;
        _mrfstr_config.fill_func(res += size, chr, (mrfstr_size_t)-(mrfstr_long_t)size);

        mrfstr_fill_rem;
        return;
    }

    mrfstr_set_tcount(_mrfstr_config.mem_tlimit);

    rem = (mrfstr_ulong_t)res & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        rem = MRFSTR_ALIGN_SIZE - rem;
        size -= rem;
        mrfstr_fill_rem;
    }

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (mrfstr_size_t)-(mrfstr_long_t)((size - rem) / tcount);

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        size *= tcount;
        _mrfstr_config.fill_func(res -= size, chr, size);

        mrfstr_fill_rem;
        return;
    }

    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_fill_t)malloc(sizeof(struct __MRFSTR_FILL_T));
        if (!data)
            break;

        *data = (struct __MRFSTR_FILL_T){.res=res -= size, .size=size, .chr=chr};
        mrfstr_create_thread(__mrfstr_fill_threaded)
        {
            res += size;

            free(data);
            break;
        }

        mrfstr_thread_priority;
    }

    tcount -= i;
    size *= tcount;
    _mrfstr_config.fill_tfunc(res -= size, chr, size);

    mrfstr_fill_rem;

    mrfstr_close_threads;
    free(threads);
}

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_fill_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_fill_threaded(
    LPVOID args)
#endif
{
    mrfstr_fill_t data;

    data = (mrfstr_fill_t)args;
    _mrfstr_config.fill_tfunc(data->res, data->chr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
