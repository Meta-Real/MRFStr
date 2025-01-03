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

struct __MRFSTR_CONTCHR_T
{
    volatile mrfstr_bool_t *res;

    mrfstr_data_ct str;
    mrfstr_size_t size;
    mrfstr_chr_t chr;
};
typedef struct __MRFSTR_CONTCHR_T *mrfstr_contchr_t;

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_contchr_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_contchr_threaded(
    LPVOID args);
#endif

mrfstr_bool_t __mrfstr_contchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_ushort_t rem;
    mrfstr_ubyte_t tcount, nthreads, i;
    volatile mrfstr_bool_t res;
    mrfstr_thread_t *threads;
    mrfstr_contchr_t data;

    if (size < MRFSTR_SLIMIT)
        return memchr(str, chr, size) != NULL;

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.searchchr_tlimit)
    {
        rem = (mrfstr_ulong_t)str & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            rem = MRFSTR_ALIGN_SIZE - rem;
            size -= rem;
            mrfstr_contchr_rem;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size = (mrfstr_size_t)-(mrfstr_long_t)(size - rem);

single:
        if (_mrfstr_config.contchr_func(str -= size, chr, size))
            return MRFSTR_TRUE;

        mrfstr_contchr_rem;
        return MRFSTR_FALSE;
    }

    mrfstr_set_tcount(_mrfstr_config.searchchr_tlimit);

    rem = (mrfstr_ulong_t)str & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        rem = MRFSTR_ALIGN_SIZE - rem;
        size -= rem;
        mrfstr_contchr_rem;
    }

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (mrfstr_size_t)-(mrfstr_long_t)((size - rem) / tcount);

    res = MRFSTR_FALSE;

    nthreads = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(nthreads * sizeof(mrfstr_thread_t));
    if (!threads)
    {
        size *= tcount;
        goto single;
    }

    for (i = 0; i != nthreads; i++)
    {
        data = (mrfstr_contchr_t)malloc(sizeof(struct __MRFSTR_CONTCHR_T));
        if (!data)
        {
            if (!i)
            {
                free(threads);

                size *= tcount;
                goto single;
            }
            break;
        }

        *data = (struct __MRFSTR_CONTCHR_T){.res=&res, .str=str -= size, .size=size, .chr=chr};
        mrfstr_create_thread(__mrfstr_contchr_threaded)
        {
            str += size;

            free(data);
            if (!i)
            {
                free(threads);

                size *= tcount;
                goto single;
            }
            break;
        }

        mrfstr_thread_priority;
    }

    tcount -= i;

    size *= tcount;
    _mrfstr_config.contchr_tfunc(&res, str -= size, chr, size);

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

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_contchr_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_contchr_threaded(
    LPVOID args)
#endif
{
    mrfstr_contchr_t data;

    data = (mrfstr_contchr_t)args;
    _mrfstr_config.contchr_tfunc(data->res, data->str, data->chr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
