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

#define mrfstr_equal_rem        \
    while (rem--)               \
        if (*str1++ != *str2++) \
            return MRFSTR_FALSE

#pragma pack(push, 1)
struct __MRFSTR_EQUAL_T
{
    volatile mrfstr_bool_t *res;

    mrfstr_data_ct str1;
    mrfstr_data_ct str2;
    mrfstr_size_t size;
};
#pragma pack(pop)
typedef struct __MRFSTR_EQUAL_T *mrfstr_equal_t;

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_equal_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_equal_threaded(
    LPVOID args);
#endif

mrfstr_bool_t __mrfstr_equal(
    mrfstr_data_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
    mrfstr_size_t tsize, inc;
    mrfstr_short_t rem, factor;
    mrfstr_byte_t tcount, i;
    volatile mrfstr_bool_t res;
    mrfstr_thread_t *threads;
    mrfstr_equal_t data;

    if (size < MRFSTR_SLIMIT)
        return !memcmp(str1, str2, size);

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_byte_t mask;

        mask = _mrfstr_config.ncmp_size - 1;
        rem = (uintptr_t)str1 & mask;
        if (rem)
        {
            rem = _mrfstr_config.ncmp_size - rem;
            size -= rem;
            mrfstr_equal_rem;
        }

        rem = size & mask;
        size -= rem;

        if (!_mrfstr_config.nequal_sub(str1, str2, size / _mrfstr_config.ncmp_size))
            return MRFSTR_FALSE;
        str1 += size;
        str2 += size;

        mrfstr_equal_rem;
        return MRFSTR_TRUE;
    }

    mrfstr_set_tcount;

    rem = (uintptr_t)str1 & (_mrfstr_config.tcmp_size - 1);
    if (rem)
    {
        rem = _mrfstr_config.tcmp_size - rem;
        size -= rem;
        mrfstr_equal_rem;
    }

    factor = _mrfstr_config.tcmp_size * tcount;
    rem = size % factor;
    inc = (size /= factor) * _mrfstr_config.tcmp_size;

    res = MRFSTR_TRUE;

    factor = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(factor * sizeof(mrfstr_thread_t));
    if (!threads)
    {
single:
        if (!_mrfstr_config.nequal_sub(str1, str2, size * tcount))
            return MRFSTR_FALSE;
        inc *= tcount;
        str1 += inc;
        str2 += inc;

        mrfstr_equal_rem;
        return res;
    }

    for (i = 0; i != factor; i++)
    {
        data = (mrfstr_equal_t)malloc(sizeof(struct __MRFSTR_EQUAL_T));
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
        data->str1 = str1;
        data->str2 = str2;
        data->size = size;

        str1 += inc;
        str2 += inc;
        mrfstr_create_thread(__mrfstr_equal_threaded)
        {
            str1 -= inc;
            str2 -= inc;

            free(data);
            if (!i)
            {
                free(threads);
                goto single;
            }
            break;
        }

        mrfstr_thread_priority;
    }

    tcount -= i;
    _mrfstr_config.tequal_sub(&res, str1, str2, size * tcount);
    if (!res)
    {
        mrfstr_close_threads;
        free(threads);
        return MRFSTR_FALSE;
    }

    inc *= tcount;
    str1 += inc;
    str2 += inc;
    while (rem--)
        if (*str1++ != *str2++)
        {
            res = MRFSTR_FALSE;
            break;
        }

    mrfstr_close_threads;
    free(threads);
    return res;
}

#ifdef MRFSTR_BUILD_UNIX
mrfstr_ptr_t __mrfstr_equal_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_equal_threaded(
    LPVOID args)
#endif
{
    mrfstr_equal_t data;

    data = (mrfstr_equal_t)args;
    _mrfstr_config.tequal_sub(data->res, data->str1, data->str2, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
