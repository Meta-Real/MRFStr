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
    for (; rem; rem--)  \
        *res++ = chr

#pragma pack(push, 1)
struct __MRFSTR_FILL_T
{
    mrfstr_data_t res;
    mrfstr_chr_t chr;
    mrfstr_size_t size;
};
#pragma pack(pop)
typedef struct __MRFSTR_FILL_T *mrfstr_fill_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_fill_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_fill_threaded(
    LPVOID args);
#endif

void __mrfstr_fill(
    mrfstr_data_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        memset(res, chr, size);
        return;
    }

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_byte_t rem = (uintptr_t)res % _mrfstr_config.nmem_size;
        if (rem)
        {
            rem = _mrfstr_config.nmem_size - rem;
            size -= rem;
            mrfstr_fill_rem;
        }

        rem = size % _mrfstr_config.nmem_size;
        size -= rem;

        _mrfstr_config.nfill_sub(res, chr, size / _mrfstr_config.nmem_size);
        res += size;

        mrfstr_fill_rem;
        return;
    }

    mrfstr_size_t tsize = _mrfstr_config.tlimit >> 1;
    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * tsize)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / tsize);

    mrfstr_short_t rem = (uintptr_t)res % _mrfstr_config.tmem_size;
    if (rem)
    {
        rem = _mrfstr_config.tmem_size - rem;
        size -= rem;
        mrfstr_fill_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.tmem_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.tmem_size;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_fill_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_FILL_T));
            if (!data)
                break;

            data->res = res;
            data->chr = chr;
            data->size = size;

            res += inc;

            mrfstr_create_thread(__mrfstr_fill_threaded)
            {
                res -= inc;

                free(data);
                break;
            }
        }

        tcount -= i;
    }

    _mrfstr_config.tfill_sub(res, chr, size * tcount);
    res += inc * tcount;

    mrfstr_fill_rem;

    if (i)
        mrfstr_close_threads;
    free(threads);
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_fill_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_fill_threaded(
    LPVOID args)
#endif
{
    mrfstr_fill_t data = (mrfstr_fill_t)args;
    _mrfstr_config.tfill_sub(data->res, data->chr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
