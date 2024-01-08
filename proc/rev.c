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
    for (; rem; rem--)  \
    {                   \
        chr = *--right; \
        *right = *str;  \
        *str++ = chr;   \
    }

#define mrfstr_rev2_rem \
    for (; rem; rem--)  \
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
    mrfstr_data_t right = str + size;
    if (size < MRFSTR_SLIMIT)
    {
        mrfstr_chr_t chr;
        while (str < right)
        {
            chr = *--right;
            *right = *str;
            *str++ = chr;
        }
        return;
    }

    if (_mrfstr_config.tcount == 1 || size < MRFSTR_TLIMIT)
    {
        mrfstr_byte_t rem = (uintptr_t)str % _mrfstr_config.nrev_size;
        mrfstr_chr_t chr;
        if (rem)
        {
            rem = _mrfstr_config.nrev_size - rem;
            size -= rem;
            mrfstr_rev_rem;
        }

        mrfstr_byte_t revsize = _mrfstr_config.nrev_size << 1;
        rem = size % revsize;
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

    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * MRFSTR_TSIZE)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_short_t rem = (uintptr_t)str & _mrfstr_config.trev_size;
    mrfstr_chr_t chr;
    if (rem)
    {
        rem = _mrfstr_config.trev_size - rem;
        size -= rem;
        mrfstr_rev_rem;
    }

    size /= (_mrfstr_config.trev_size << 1) * tcount;
    mrfstr_size_t inc = size * _mrfstr_config.trev_size;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_rev_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_REV_T));
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
    }

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

    if (i)
        mrfstr_close_threads;
    free(threads);
}

void __mrfstr_rev2(
    mrfstr_data_t left, mrfstr_data_ct right, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        for (; size; size--)
            *left++ = *--right;
        return;
    }

    if (_mrfstr_config.tcount == 1 || size < MRFSTR_TLIMIT)
    {
        mrfstr_byte_t rem = (uintptr_t)left % _mrfstr_config.nrev_size;
        if (rem)
        {
            rem = _mrfstr_config.nrev_size - rem;
            size -= rem;
            mrfstr_rev2_rem;
        }

        rem = size % _mrfstr_config.nrev_size;
        size -= rem;

        _mrfstr_config.nrev2_sub(left, right, size / _mrfstr_config.nrev_size);
        left += size;
        right -= size;

        mrfstr_rev2_rem;
        return;
    }

    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * MRFSTR_TSIZE)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_short_t rem = (uintptr_t)left & _mrfstr_config.trev_size;
    if (rem)
    {
        rem = _mrfstr_config.trev_size - rem;
        size -= rem;
        mrfstr_rev2_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.trev_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.trev_size;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_rev_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_REV_T));
            if (!data)
                break;

            data->left = left;
            data->right = (mrfstr_data_t)right;
            data->size = size;

            left += inc;
            right -= inc;

            mrfstr_create_thread(__mrfstr_rev2_threaded)
            {
                left += inc;
                right -= inc;
                free(data);
                break;
            }
        }

        tcount -= i;
    }

    _mrfstr_config.trev2_sub(left, right, size * tcount);

    inc *= tcount;
    left += inc;
    right -= inc;
    mrfstr_rev2_rem;

    if (i)
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
    mrfstr_rev_t data = (mrfstr_rev_t)args;
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
    mrfstr_rev_t data = (mrfstr_rev_t)args;
    _mrfstr_config.trev2_sub(data->left, data->right, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
