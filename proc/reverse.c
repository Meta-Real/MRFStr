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

#pragma pack(push, 1)
struct __MRFSTR_REV_T
{
    mrfstr_data_t left;
    mrfstr_data_t right;
    mrfstr_size_t size;
};
#pragma pack(pop)
typedef struct __MRFSTR_REV_T *mrfstr_rev_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_rev_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_rev_threaded(
    LPVOID args);
#endif

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
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
    mrfstr_size_t tsize, inc;
    mrfstr_short_t rem;
    mrfstr_byte_t tcount, i, nthreads;
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

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_byte_t revsize;

        rem = (uintptr_t)str & _mrfstr_config.nrev_size;
        if (rem)
        {
            rem = _mrfstr_config.nrev_size - rem;
            size -= rem;
            mrfstr_rev_rem;
        }

        revsize = _mrfstr_config.nrev_size << 1;
        rem = size & revsize - 1;
        size -= rem;

        _mrfstr_config.nrev_sub(str, right, size / revsize);
        size >>= 1;
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

    mrfstr_set_tcount;

    rem = (uintptr_t)str & _mrfstr_config.trev_size - 1;
    if (rem)
    {
        rem = _mrfstr_config.trev_size - rem;
        size -= rem;
        mrfstr_rev_rem;
    }

    size /= (_mrfstr_config.trev_size << 1) * tcount;
    inc = size * _mrfstr_config.trev_size;

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        _mrfstr_config.trev_sub(str, right, size * tcount);
        inc *= tcount;
        str += inc;
        right -= inc;

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

        data->left = str;
        data->right = right;
        data->size = size;

        str += inc;
        right -= inc;
        mrfstr_create_thread(__mrfstr_rev_threaded)
        {
            str -= inc;
            right += inc;
            free(data);
            break;
        }
    }

    tcount -= i;
    _mrfstr_config.trev_sub(str, right, size * tcount);
    inc *= tcount;
    str += inc;
    right -= inc;

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
    mrfstr_data_t left, mrfstr_data_ct right,
    mrfstr_size_t size)
{
    mrfstr_size_t tsize, inc;
    mrfstr_short_t rem, factor;
    mrfstr_byte_t tcount, i;
    mrfstr_thread_t *threads;
    mrfstr_rev_t data;

    if (size < MRFSTR_SLIMIT)
    {
        while (size--)
            *left++ = *--right;
        return;
    }

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_byte_t mask;

        mask = _mrfstr_config.nrev_size - 1;
        rem = (uintptr_t)left & mask;
        if (rem)
        {
            rem = _mrfstr_config.nrev_size - rem;
            size -= rem;
            mrfstr_rev2_rem;
        }

        rem = size & mask;
        size -= rem;

        _mrfstr_config.nrev2_sub(left, right, size / _mrfstr_config.nrev_size);
        left += size;
        right -= size;

        mrfstr_rev2_rem;
        return;
    }

    mrfstr_set_tcount;

    rem = (uintptr_t)left & _mrfstr_config.trev_size - 1;
    if (rem)
    {
        rem = _mrfstr_config.trev_size - rem;
        size -= rem;
        mrfstr_rev2_rem;
    }

    factor = _mrfstr_config.trev_size * tcount;
    rem = size % factor;
    inc = (size /= factor) * _mrfstr_config.trev_size;

    factor = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(factor * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        _mrfstr_config.trev2_sub(left, right, size * tcount);
        inc *= tcount;
        left += inc;
        right -= inc;

        mrfstr_rev2_rem;
    }

    for (i = 0; i != factor; i++)
    {
        data = (mrfstr_rev_t)malloc(sizeof(struct __MRFSTR_REV_T));
        if (!data)
            break;

        data->left = left;
        data->right = (mrfstr_data_t)right;
        data->size = size;

        left += inc;
        right -= inc;
        mrfstr_create_thread(__mrfstr_rev2_threaded)
        {
            left -= inc;
            right += inc;

            free(data);
            break;
        }
    }

    tcount -= i;
    _mrfstr_config.trev2_sub(left, right, size * tcount);
    inc *= tcount;
    left += inc;
    right -= inc;

    mrfstr_rev2_rem;

    mrfstr_close_threads;
    free(threads);
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_rev_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_rev_threaded(
    LPVOID args)
#endif
{
    mrfstr_rev_t data;

    data = (mrfstr_rev_t)args;
    _mrfstr_config.trev_sub(data->left, data->right, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_rev2_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_rev2_threaded(
    LPVOID args)
#endif
{
    mrfstr_rev_t data;

    data = (mrfstr_rev_t)args;
    _mrfstr_config.trev2_sub(data->left, data->right, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
