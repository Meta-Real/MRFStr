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
#include <stdarg.h>

void mrfstr_init(
    mrfstr_t str)
{
    MRFSTR_SIZE(str) = 0;
    MRFSTR_CAPA(str) = 0;
}

void mrfstr_inits(
    mrfstr_p str, ...)
{
    va_list strs;

    va_start(strs, str);
    do
    {
        MRFSTR_SIZE(str) = 0;
        MRFSTR_CAPA(str) = 0;

        str = va_arg(strs, mrfstr_p);
    } while (str);
    va_end(strs);
}

void mrfstr_init_str(
    mrfstr_t str, mrfstr_data_t data)
{
    MRFSTR_DATA(str) = data;
    MRFSTR_SIZE(str) = mrfstr_strlen(data);
    MRFSTR_CAPA(str) = MRFSTR_SIZE(str) + 1;
}

void mrfstr_init_nstr(
    mrfstr_t str, mrfstr_data_t data, mrfstr_size_t size)
{
    MRFSTR_DATA(str) = data;
    MRFSTR_SIZE(str) = size;
    MRFSTR_CAPA(str) = size;
}

void mrfstr_init_nstr2(
    mrfstr_t str, mrfstr_data_t data, mrfstr_size_t size, mrfstr_size_t capa)
{
    MRFSTR_DATA(str) = data;
    MRFSTR_SIZE(str) = size;
    MRFSTR_CAPA(str) = capa;
}

mrfstr_res_t mrfstr_init_alloc(
    mrfstr_t str, mrfstr_size_t size)
{
    MRFSTR_SIZE(str) = 0;
    if (!size)
    {
        MRFSTR_CAPA(str) = 0;
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_ALLOC(str, size);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_alloc(
    mrfstr_t str,  mrfstr_size_t size)
{
    if (!size)
        return MRFSTR_RES_NOERROR;

    MRFSTR_ALLOC(str, size);
    return MRFSTR_RES_NOERROR;
}

void mrfstr_free(
    mrfstr_t str)
{
    if (MRFSTR_CAPA(str))
        free(MRFSTR_DATA(str));
}

void mrfstr_frees(
    mrfstr_p str, ...)
{
    va_list strs;

    va_start(strs, str);
    do
    {
        if (MRFSTR_CAPA(str))
            free(MRFSTR_DATA(str));

        str = va_arg(strs, mrfstr_p);
    } while (str);
    va_end(strs);
}

void mrfstr_clear(
    mrfstr_t str)
{
    MRFSTR_FREE(str);
}

void mrfstr_clears(
    mrfstr_p str, ...)
{
    va_list strs;

    va_start(strs, str);
    do
    {
        MRFSTR_FREE(str);
        str = va_arg(strs, mrfstr_p);
    } while (str);
    va_end(strs);
}

mrfstr_res_t mrfstr_realloc(
    mrfstr_t str, mrfstr_size_t size)
{
    if (size == MRFSTR_CAPA(str))
        return MRFSTR_RES_NOERROR;

    if (!size)
    {
        MRFSTR_FREE(str);
        return MRFSTR_RES_NOERROR;
    }

    if (!MRFSTR_SIZE(str))
    {
        MRFSTR_CLEAR_REALLOC(str, size);
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_REALLOC(str, size);
    if (MRFSTR_SIZE(str) > size)
        MRFSTR_SIZE(str) = size;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_clear_realloc(
    mrfstr_t str, mrfstr_size_t size)
{
    if (MRFSTR_CAPA(str) == size)
    {
        MRFSTR_SIZE(str) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (!size)
    {
        MRFSTR_FREE(str);
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_CLEAR_REALLOC(str, size);
    MRFSTR_SIZE(str) = 0;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_expand(
    mrfstr_t str, mrfstr_size_t size)
{
    if (!MRFSTR_SIZE(str))
    {
        MRFSTR_CLEAR_REALLOC(str, size);
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_REALLOC(str, size);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_clear_expand(
    mrfstr_t str, mrfstr_size_t size)
{
    MRFSTR_CLEAR_REALLOC(str, size);
    MRFSTR_SIZE(str) = 0;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_shrink(
    mrfstr_t str, mrfstr_size_t size)
{
    if (!size)
    {
        MRFSTR_FREE(str);
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_REALLOC(str, size);
    if (MRFSTR_SIZE(str) > size)
        MRFSTR_SIZE(str) = size;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_clear_shrink(
    mrfstr_t str, mrfstr_size_t size)
{
    if (!size)
    {
        MRFSTR_FREE(str);
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_CLEAR_REALLOC(str, size);
    MRFSTR_SIZE(str) = 0;
    return MRFSTR_RES_NOERROR;
}

void mrfstr_swap(
    mrfstr_t str1, mrfstr_t str2)
{
    mrfstr_data_t data;
    mrfstr_size_t size;

    data = MRFSTR_DATA(str1);
    MRFSTR_DATA(str1) = MRFSTR_DATA(str2);
    MRFSTR_DATA(str2) = data;

    size = MRFSTR_SIZE(str1);
    MRFSTR_SIZE(str1) = MRFSTR_SIZE(str2);
    MRFSTR_SIZE(str2) = size;

    size = MRFSTR_CAPA(str1);
    MRFSTR_CAPA(str1) = MRFSTR_CAPA(str2);
    MRFSTR_CAPA(str2) = size;
}
