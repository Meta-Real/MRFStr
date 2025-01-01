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

#define mrfstr_move_rem \
    while (rem--)       \
        *dst++ = *src++

#define mrfstr_rmove_rem \
    while (rem--)        \
        *--dst = *--src

struct __MRFSTR_MOVE_T
{
    mrfstr_data_t dst;
    mrfstr_data_ct src;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_MOVE_T *mrfstr_move_t;

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_move_threaded(
    mrfstr_ptr_t args);
mrfstr_ptr_t __mrfstr_rmove_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_move_threaded(
    LPVOID args);
DWORD WINAPI __mrfstr_rmove_threaded(
    LPVOID args);
#endif

void __mrfstr_move(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    mrfstr_data_t tmp, dstc;
    mrfstr_size_t diff, tmpptr;
    mrfstr_ushort_t rem;
    mrfstr_ubyte_t tcount, nthreads, i;
    mrfstr_thread_t *threads;
    mrfstr_move_t data;

    if (size < MRFSTR_SLIMIT)
    {
        memmove(dst, src, size);
        return;
    }

    diff = src - dst;
    if (size < _mrfstr_config.mem_tlimit || _mrfstr_config.tcount == 1 || diff > _mrfstr_config.move_tlimit)
    {
        rem = (mrfstr_ulong_t)dst & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            rem = MRFSTR_ALIGN_SIZE - rem;
            size -= rem;
            mrfstr_move_rem;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size = (mrfstr_size_t)-(mrfstr_long_t)(size - rem);

single:
        _mrfstr_config.copy_func(dst -= size, src -= size, size);

        mrfstr_move_rem;
        return;
    }

    mrfstr_set_tcount(_mrfstr_config.mem_tlimit);

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (mrfstr_size_t)-(mrfstr_long_t)((size - rem) / tcount);

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        size *= tcount;
        goto single;
    }

    tmp = malloc(diff * nthreads * sizeof(mrfstr_chr_t));
    if (!tmp)
    {
        size *= tcount;
        goto single;
    }

    tmpptr = 0;
    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_move_t)malloc(sizeof(struct __MRFSTR_MOVE_T));
        if (!data)
            break;

        data->dst = dst -= size;
        data->src = src -= size;
        data->size = size;

        memcpy(tmp + tmpptr, src - diff, diff);
        tmpptr += diff;

        mrfstr_create_thread(__mrfstr_move_threaded)
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
    dstc = dst;
    _mrfstr_config.copy_tfunc(dst -= size, src -= size, size);

    mrfstr_move_rem;

    mrfstr_close_threads;
    free(threads);

    size /= tcount;
    while (tmpptr)
    {
        tmpptr -= diff;
        memcpy(dstc - diff, tmp + tmpptr, diff);
        dstc += size;
    }

    free(tmp);
}

void __mrfstr_rmove(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    mrfstr_data_t tmp, dstc;
    mrfstr_size_t diff, tmpptr;
    mrfstr_ushort_t rem;
    mrfstr_ubyte_t tcount, nthreads, i;
    mrfstr_thread_t *threads;
    mrfstr_move_t data;

    if (size < MRFSTR_SLIMIT)
    {
        memmove(dst, src, size);
        return;
    }

    dst += size;
    src += size;

    diff = dst - src;
    if (size < _mrfstr_config.mem_tlimit || _mrfstr_config.tcount == 1 || diff > _mrfstr_config.move_tlimit)
    {
        rem = (mrfstr_ulong_t)dst & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            size -= rem;
            mrfstr_rmove_rem;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size -= rem;

single:
        _mrfstr_config.rcopy_func(dst -= size, src -= size, size);

        mrfstr_rmove_rem;
        return;
    }

    mrfstr_set_tcount(_mrfstr_config.mem_tlimit);

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (size - rem) / tcount;

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        size *= tcount;
        goto single;
    }

    tmp = malloc(diff * nthreads * sizeof(mrfstr_chr_t));
    if (!tmp)
    {
        size *= tcount;
        goto single;
    }

    tmpptr = 0;
    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_move_t)malloc(sizeof(struct __MRFSTR_MOVE_T));
        if (!data)
            break;

        data->dst = dst -= size;
        data->src = src -= size;
        data->size = size;

        memcpy(tmp + tmpptr, src, diff);
        tmpptr += diff;

        mrfstr_create_thread(__mrfstr_move_threaded)
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
    _mrfstr_config.rcopy_tfunc(dst -= size, src -= size, size);

    dstc = dst;
    mrfstr_rmove_rem;

    mrfstr_close_threads;
    free(threads);

    size /= tcount;
    while (tmpptr)
    {
        tmpptr -= diff;
        dstc += size;
        memcpy(dstc + diff, tmp + tmpptr, diff);
    }

    free(tmp);
}

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_move_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_move_threaded(
    LPVOID args)
#endif
{
    mrfstr_move_t data;

    data = (mrfstr_move_t)args;
    _mrfstr_config.copy_tfunc(data->dst, data->src, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_rmove_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_rmove_threaded(
    LPVOID args)
#endif
{
    mrfstr_move_t data;

    data = (mrfstr_move_t)args;
    _mrfstr_config.rcopy_tfunc(data->dst, data->src, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
