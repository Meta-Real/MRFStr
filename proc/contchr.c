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
    mrfstr_data_ct str;
    mrfstr_chr_t chr;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
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
    if (size < MRFSTR_SLIMIT)
        return memchr(str, chr, size) != NULL;

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_byte_t rem = (uintptr_t)str % _mrfstr_config.nsearch_size;
        if (rem)
        {
            rem = _mrfstr_config.nsearch_size - rem;
            size -= rem;
            mrfstr_contchr_rem;
        }

        rem = size % _mrfstr_config.nsearch_size;
        size -= rem;

        if (_mrfstr_config.ncontchr_sub(str, chr, size / _mrfstr_config.nsearch_size))
            return MRFSTR_TRUE;
        str += size;

        mrfstr_contchr_rem;
        return MRFSTR_FALSE;
    }

    mrfstr_size_t tsize = _mrfstr_config.tlimit >> 1;
    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * tsize)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / tsize);

    mrfstr_short_t rem = (uintptr_t)str % _mrfstr_config.tsearch_size;
    if (rem)
    {
        rem = _mrfstr_config.tsearch_size - rem;
        size -= rem;
        mrfstr_contchr_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.tsearch_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.tsearch_size;

    volatile mrfstr_bool_t res = MRFSTR_FALSE;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_contchr_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_CONTCHR_T));
            if (!data)
            {
                if (!i)
                {
                    if (_mrfstr_config.ncontchr_sub(str, chr, size * tcount))
                    {
                        free(threads);
                        return MRFSTR_TRUE;
                    }

                    str += inc * tcount;
                    mrfstr_contchr_rem;

                    free(threads);
                    return MRFSTR_FALSE;
                }
                break;
            }

            data->str = str;
            data->chr = chr;
            data->size = size;
            data->res = &res;

            str += inc;

            mrfstr_create_thread(__mrfstr_contchr_threaded)
            {
                str -= inc;
                free(data);

                if (!i)
                {
                    if (_mrfstr_config.ncontchr_sub(str, chr, size * tcount))
                    {
                        free(threads);
                        return MRFSTR_TRUE;
                    }

                    str += inc * tcount;
                    mrfstr_contchr_rem;

                    free(threads);
                    return MRFSTR_FALSE;
                }
                break;
            }
        }

        tcount -= i;
    }

    _mrfstr_config.tcontchr_sub(&res, str, chr, size * tcount);
    str += inc * tcount;

    mrfstr_contchr_rem;

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
    mrfstr_contchr_t data = (mrfstr_contchr_t)args;
    _mrfstr_config.tcontchr_sub(data->res, data->str, data->chr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
