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

#define mrfstr_rev_rem  \
    while (rem--)       \
    {                   \
        chr = *--right; \
        *right = *str;  \
        *str++ = chr;   \
    }

#define mrfstr_rev2_rem \
    while (rem--)       \
        *left++ = *--right

struct __MRFSTR_REV_T
{
    mrfstr_data_t left;
    mrfstr_data_t right;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_REV_T *mrfstr_rev_t;

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_rev_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_rev_threaded(
    LPVOID args);
#endif

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_rev2_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_rev2_threaded(
    LPVOID args);
#endif

void __mrfstr_rev(
    mrfstr_data_t str, mrfstr_size_t size)
{
    mrfstr_data_t right;
    mrfstr_chr_t chr;
    mrfstr_ushort_t rem, tcount2;
    mrfstr_ubyte_t tcount, nthreads, i;
    mrfstr_thread_t *threads;
    mrfstr_rev_t data;

    right = str + size;
    if (size < MRFSTR_SLIMIT)
    {
        while (str < right)
        {
            chr = *--right;
            *right = *str;
            *str++ = chr;
        }
        return;
    }

    if (size < _mrfstr_config.rev_tlimit || _mrfstr_config.tcount == 1)
    {
        rem = (mrfstr_ulong_t)str & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            rem = MRFSTR_ALIGN_SIZE - rem;
            size -= rem;
            mrfstr_rev_rem;
        }

        rem = size & ((MRFSTR_ALIGN_SIZE << 1) - 1);
        size = (size - rem) >> 1;
        _mrfstr_config.rev_func(str, right, size);
        str += size;
        right -= size;

        while (str < right)
        {
            chr = *--right;
            *right = *str;
            *str++ = chr;
        }
        return;
    }

    mrfstr_set_tcount(_mrfstr_config.rev_tlimit);

    rem = (mrfstr_ulong_t)str & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        rem = MRFSTR_ALIGN_SIZE - rem;
        size -= rem << 1;
        mrfstr_rev_rem;
    }

    tcount2 = (mrfstr_ushort_t)tcount << 1;
    rem = size % (MRFSTR_ALIGN_SIZE * tcount2);
    size = (size - rem) / tcount2;

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        size *= tcount;
        _mrfstr_config.rev_tfunc(str, right, size);
        str += size;
        right -= size;

        while (str < right)
        {
            chr = *--right;
            *right = *str;
            *str++ = chr;
        }
        return;
    }

    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_rev_t)malloc(sizeof(struct __MRFSTR_REV_T));
        if (!data)
            break;

        *data = (struct __MRFSTR_REV_T){.left=str, .right=right, .size=size};
        mrfstr_create_thread(__mrfstr_rev_threaded)
        {
            free(data);
            break;
        }

        str += size;
        right -= size;
        mrfstr_thread_priority;
    }

    tcount -= i;
    size *= tcount;
    _mrfstr_config.rev_tfunc(str, right, size);
    str += size;
    right -= size;

    while (str < right)
    {
        chr = *--right;
        *right = *str;
        *str++ = chr;
    }

    mrfstr_close_threads;
    free(threads);
}

void __mrfstr_rev2(
    mrfstr_data_t left, mrfstr_data_ct right, mrfstr_size_t size)
{
    mrfstr_ushort_t rem;
    mrfstr_ubyte_t tcount, nthreads, i;
    mrfstr_thread_t *threads;
    mrfstr_rev_t data;

    if (size < MRFSTR_SLIMIT)
    {
        while (size--)
            *left++ = *--right;
        return;
    }

    if (size < _mrfstr_config.rev_tlimit || _mrfstr_config.tcount == 1)
    {
        rem = (mrfstr_ulong_t)left & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            rem = MRFSTR_ALIGN_SIZE - rem;
            size -= rem;
            mrfstr_rev2_rem;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size -= rem;
        _mrfstr_config.rev2_func(left, right, size);
        left += size;
        right -= size;

        mrfstr_rev2_rem;
        return;
    }

    mrfstr_set_tcount(_mrfstr_config.rev_tlimit);

    rem = (mrfstr_ulong_t)left & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        rem = MRFSTR_ALIGN_SIZE - rem;
        size -= rem;
        mrfstr_rev2_rem;
    }

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (size - rem) / tcount;

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        size *= tcount;
        _mrfstr_config.rev2_tfunc(left, right, size);
        left += size;
        right -= size;

        mrfstr_rev2_rem;
    }

    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_rev_t)malloc(sizeof(struct __MRFSTR_REV_T));
        if (!data)
            break;

        *data = (struct __MRFSTR_REV_T){.left=left, .right=(mrfstr_data_t)right, .size=size};
        mrfstr_create_thread(__mrfstr_rev2_threaded)
        {
            free(data);
            break;
        }

        left += size;
        right -= size;
        mrfstr_thread_priority;
    }

    tcount -= i;
    size *= tcount;
    _mrfstr_config.rev2_tfunc(left, right, size);
    left += size;
    right -= size;

    mrfstr_rev2_rem;

    mrfstr_close_threads;
    free(threads);
}

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_rev_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_rev_threaded(
    LPVOID args)
#endif
{
    mrfstr_rev_t data;

    data = (mrfstr_rev_t)args;
    _mrfstr_config.rev_tfunc(data->left, data->right, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_rev2_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_rev2_threaded(
    LPVOID args)
#endif
{
    mrfstr_rev_t data;

    data = (mrfstr_rev_t)args;
    _mrfstr_config.rev2_tfunc(data->left, data->right, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
