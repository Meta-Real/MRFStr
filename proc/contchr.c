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

#define mrfstr_contchr_rem \
    while (rem--)          \
        if (chr == *str++) \
            return MRFSTR_TRUE

#pragma pack(push, 1)
struct __MRFSTR_CONTCHR_T
{
    volatile mrfstr_bool_t *res;

    mrfstr_data_ct str;
    mrfstr_size_t size;
    mrfstr_chr_t chr;
};
#pragma pack(pop)
typedef struct __MRFSTR_CONTCHR_T *mrfstr_contchr_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_contchr_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_contchr_threaded(
    LPVOID args);
#endif

mrfstr_bool_t __mrfstr_contchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_size_t tsize, inc;
    mrfstr_short_t rem, factor;
    mrfstr_byte_t tcount, i;
    volatile mrfstr_bool_t res;
    mrfstr_thread_t *threads;
    mrfstr_contchr_t data;

    if (size < MRFSTR_SLIMIT)
        return memchr(str, chr, size) != NULL;

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_byte_t mask;

        mask = _mrfstr_config.nsearch_size - 1;
        rem = (uintptr_t)str & mask;
        if (rem)
        {
            rem = _mrfstr_config.nsearch_size - rem;
            size -= rem;
            mrfstr_contchr_rem;
        }

        rem = size & mask;
        size -= rem;

        if (_mrfstr_config.ncontchr_sub(str, chr, size / _mrfstr_config.nsearch_size))
            return MRFSTR_TRUE;
        str += size;

        mrfstr_contchr_rem;
        return MRFSTR_FALSE;
    }

    mrfstr_set_tcount;

    rem = (uintptr_t)str & (_mrfstr_config.tsearch_size - 1);
    if (rem)
    {
        rem = _mrfstr_config.tsearch_size - rem;
        size -= rem;
        mrfstr_contchr_rem;
    }

    factor = _mrfstr_config.tsearch_size * tcount;
    rem = size % factor;
    inc = (size /= factor) * _mrfstr_config.tsearch_size;

    res = MRFSTR_FALSE;

    factor = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(factor * sizeof(mrfstr_thread_t));
    if (!threads)
    {
single:
        if (_mrfstr_config.ncontchr_sub(str, chr, size * tcount))
            return MRFSTR_TRUE;

        str += inc * tcount;
        mrfstr_contchr_rem;
        return MRFSTR_FALSE;
    }

    for (i = 0; i != factor; i++)
    {
        data = (mrfstr_contchr_t)malloc(sizeof(struct __MRFSTR_CONTCHR_T));
        if (!data)
        {
            if (!i)
            {
                free(threads);
                goto single;
            }
            break;
        }

        data->res = &res;
        data->str = str;
        data->size = size;
        data->chr = chr;

        str += inc;
        mrfstr_create_thread(__mrfstr_contchr_threaded)
        {
            str -= inc;

            free(data);
            if (!i)
            {
                free(threads);
                goto single;
            }
            break;
        }
    }

    tcount -= i;
    _mrfstr_config.tcontchr_sub(&res, str, chr, size * tcount);
    str += inc * tcount;

    while (rem--)
        if (chr == *str++)
        {
            res = MRFSTR_TRUE;
            break;
        }

    mrfstr_close_threads;
    free(threads);
    return res;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_contchr_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_contchr_threaded(
    LPVOID args)
#endif
{
    mrfstr_contchr_t data;

    data = (mrfstr_contchr_t)args;
    _mrfstr_config.tcontchr_sub(data->res, data->str, data->chr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
