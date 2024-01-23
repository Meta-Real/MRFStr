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

struct __MRFSTR_EQUAL_T
{
    mrfstr_data_ct str1;
    mrfstr_data_ct str2;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_EQUAL_T *mrfstr_equal_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_equal_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_equal_threaded(
    LPVOID args);
#endif

mrfstr_bool_t __mrfstr_equal(
    mrfstr_data_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
        return !memcmp(str1, str2, size);

    if (_mrfstr_config.tcount == 1 || size < _mrfstr_config.tlimit)
    {
        mrfstr_byte_t rem = (uintptr_t)str1 % _mrfstr_config.ncmp_size;
        if (rem)
        {
            rem = _mrfstr_config.ncmp_size - rem;
            size -= rem;
            mrfstr_equal_rem;
        }

        rem = size % _mrfstr_config.ncmp_size;
        size -= rem;

        if (!_mrfstr_config.nequal_sub(str1, str2, size / _mrfstr_config.ncmp_size))
            return MRFSTR_FALSE;
        str1 += size;
        str2 += size;

        mrfstr_equal_rem;
        return MRFSTR_TRUE;
    }

    mrfstr_size_t tsize = _mrfstr_config.tlimit >> 1;
    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * tsize)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / tsize);

    mrfstr_short_t rem = (uintptr_t)str1 % _mrfstr_config.tcmp_size;
    if (rem)
    {
        rem = _mrfstr_config.tcmp_size - rem;
        size -= rem;
        mrfstr_equal_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.tcmp_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.tcmp_size;

    volatile mrfstr_bool_t res = MRFSTR_TRUE;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_equal_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_EQUAL_T));
            if (!data)
            {
                if (!i)
                {
                    if (!_mrfstr_config.nequal_sub(str1, str2, size * tcount))
                    {
                        free(threads);
                        return MRFSTR_FALSE;
                    }

                    inc *= tcount;
                    str1 += inc;
                    str2 += inc;

                    mrfstr_equal_rem;

                    free(threads);
                    return MRFSTR_TRUE;
                }
                break;
            }

            data->str1 = str1;
            data->str2 = str2;
            data->size = size;
            data->res = &res;

            str1 += inc;
            str2 += inc;

            mrfstr_create_thread(__mrfstr_equal_threaded)
            {
                str1 -= inc;
                str2 -= inc;
                free(data);

                if (!i)
                {
                    if (!_mrfstr_config.nequal_sub(str1, str2, size * tcount))
                    {
                        free(threads);
                        return MRFSTR_FALSE;
                    }

                    inc *= tcount;
                    str1 += inc;
                    str2 += inc;

                    mrfstr_equal_rem;

                    free(threads);
                    return MRFSTR_TRUE;
                }
                break;
            }
        }

        tcount -= i;
    }

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
    mrfstr_equal_rem;

    mrfstr_close_threads;
    free(threads);
    return res;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_equal_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_equal_threaded(
    LPVOID args)
#endif
{
    mrfstr_equal_t data = (mrfstr_equal_t)args;
    _mrfstr_config.tequal_sub(data->res, data->str1, data->str2, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
