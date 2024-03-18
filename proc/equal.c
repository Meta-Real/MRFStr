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
    mrfstr_size_t tsize;
    mrfstr_short_t rem, factor;
    mrfstr_byte_t tcount, i;
    volatile mrfstr_bool_t res;
    mrfstr_thread_t *threads;
    mrfstr_equal_t data;

    if (size < MRFSTR_SLIMIT)
        return !memcmp(str1, str2, size);

    if (size < _mrfstr_config.cmp_tlimit || _mrfstr_config.tcount == 1)
    {
        rem = (uintptr_t)str1 & MRFSTR_ALIGN_MASK;
        if (rem)
        {
            rem = MRFSTR_ALIGN_SIZE - rem;
            size -= rem;
            mrfstr_equal_rem;
        }

        rem = size & MRFSTR_ALIGN_MASK;
        size -= rem;
        if (!_mrfstr_config.equal_func(str1 += size, str2 += size,
                (mrfstr_size_t)-(mrfstr_ssize_t)size))
            return MRFSTR_FALSE;

        mrfstr_equal_rem;
        return MRFSTR_TRUE;
    }

    mrfstr_set_tcount(_mrfstr_config.cmp_tlimit);

    rem = (uintptr_t)str1 & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        rem = MRFSTR_ALIGN_SIZE - rem;
        size -= rem;
        mrfstr_equal_rem;
    }

    rem = size % (MRFSTR_ALIGN_SIZE * tcount);
    size = (mrfstr_size_t)-(mrfstr_ssize_t)((size - rem) / tcount);

    res = MRFSTR_TRUE;

    factor = tcount - 1;
    threads = (mrfstr_thread_t*)malloc(factor * sizeof(mrfstr_thread_t));
    if (!threads)
    {
single:
        size *= tcount;
        if (!_mrfstr_config.equal_func(str1 -= size, str2 -= size, size))
            return MRFSTR_FALSE;

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
        data->str1 = str1 -= size;
        data->str2 = str2 -= size;
        data->size = size;

        mrfstr_create_thread(__mrfstr_equal_threaded)
        {
            str1 += size;
            str2 += size;

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
    size *= tcount;
    _mrfstr_config.equal_tfunc(&res, str1 -= size, str2 -= size, size);
    if (!res)
    {
        mrfstr_close_threads;
        free(threads);
        return MRFSTR_FALSE;
    }

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
    _mrfstr_config.equal_tfunc(data->res, data->str1, data->str2, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
