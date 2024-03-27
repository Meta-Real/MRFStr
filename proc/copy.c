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

#define mrfstr_copy_rem \
    while (rem--)       \
        *dst++ = *src++

struct __MRFSTR_COPY_T
{
    mrfstr_data_t dst;
    mrfstr_data_ct src;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_COPY_T *mrfstr_copy_t;

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_copy_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_copy_threaded(
    LPVOID args);
#endif

void __mrfstr_copy(
    mrfstr_data_t dst, mrfstr_data_ct src,
    mrfstr_size_t size)
{
    mrfstr_short_t rem;
    mrfstr_byte_t tcount, nthreads, i;
    mrfstr_thread_t *threads;
    mrfstr_copy_t data;

    if (size < MRFSTR_SLIMIT)
    {
        memcpy(dst, src, size);
        return;
    }

    if (size < _mrfstr_config.mem_tlimit || _mrfstr_config.tcount == 1)
    {
        rem = (uintptr_t)dst & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            rem = MRFSTR_ALIGN_SIZE - rem;
            size -= rem;
            mrfstr_copy_rem;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size -= rem;
        _mrfstr_config.copy_func(dst += size, src += size,
            (mrfstr_size_t)-(mrfstr_ssize_t)size);

        mrfstr_copy_rem;
        return;
    }

    mrfstr_set_tcount(_mrfstr_config.mem_tlimit);

    rem = (uintptr_t)dst & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        rem = MRFSTR_ALIGN_SIZE - rem;
        size -= rem;
        mrfstr_copy_rem;
    }

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (mrfstr_size_t)-(mrfstr_ssize_t)((size - rem) / tcount);

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        size *= tcount;
        _mrfstr_config.copy_tfunc(dst -= size, src -= size, size);

        mrfstr_copy_rem;
        return;
    }

    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_copy_t)malloc(sizeof(struct __MRFSTR_COPY_T));
        if (!data)
            break;

        data->dst = dst -= size;
        data->src = src -= size;
        data->size = size;

        mrfstr_create_thread(__mrfstr_copy_threaded)
        {
            dst += size;
            src += size;

            free(data);
            break;
        }

        mrfstr_thread_priority;
    }

    tcount -= i;
    size *= tcount;
    _mrfstr_config.copy_tfunc(dst -= size, src -= size, size);

    mrfstr_copy_rem;

    mrfstr_close_threads;
    free(threads);
}

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_copy_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_copy_threaded(
    LPVOID args)
#endif
{
    mrfstr_copy_t data;

    data = (mrfstr_copy_t)args;
    _mrfstr_config.copy_tfunc(data->dst, data->src, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
