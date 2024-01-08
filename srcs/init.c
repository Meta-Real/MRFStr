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

mrfstr_t mrfstr_init(void)
{
    mrfstr_t str = malloc(sizeof(struct __MRFSTR_T));
    if (!str)
        return NULL;

    MRFSTR_DATA(str) = NULL;
    MRFSTR_SIZE(str) = 0;
    return str;
}

mrfstr_t mrfstr_init2(
    mrfstr_data_t data)
{
    mrfstr_t str = malloc(sizeof(struct __MRFSTR_T));
    if (!str)
        return NULL;

    MRFSTR_SIZE(str) = mrfstr_strlen(data);
    MRFSTR_DATA(str) = MRFSTR_SIZE(str) ? data : NULL;
    return str;
}

mrfstr_t mrfstr_init3(
    mrfstr_data_t data, mrfstr_size_t size)
{
    mrfstr_t str = malloc(sizeof(struct __MRFSTR_T));
    if (!str)
        return NULL;

    MRFSTR_DATA(str) = size ? data : NULL;
    MRFSTR_SIZE(str) = size;
    return str;
}

mrfstr_res_enum_t mrfstr_alloc(
    mrfstr_t str,  mrfstr_size_t size)
{
    if (!size)
        return MRFSTR_RES_NOERROR;

    MRFSTR_DATA(str) = malloc(size * sizeof(mrfstr_chr_t));
    return MRFSTR_DATA(str) ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
}

void mrfstr_free(
    mrfstr_t str)
{
    free(MRFSTR_DATA(str));
    free(str);
}

void mrfstr_clear(
    mrfstr_t str)
{
    free(MRFSTR_DATA(str));

    MRFSTR_DATA(str) = NULL;
    MRFSTR_SIZE(str) = 0;
}

mrfstr_res_enum_t mrfstr_realloc(
    mrfstr_t str, mrfstr_size_t size)
{
    if (!size)
    {
        free(MRFSTR_DATA(str));

        MRFSTR_DATA(str) = NULL;
        MRFSTR_SIZE(str) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (!MRFSTR_SIZE(str))
    {
        free(MRFSTR_DATA(str));
        MRFSTR_DATA(str) = malloc(size * sizeof(mrfstr_chr_t));
        return MRFSTR_DATA(str) ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
    }

    mrfstr_data_t block = realloc(MRFSTR_DATA(str), size * sizeof(mrfstr_chr_t));
    if (!block)
        return MRFSTR_RES_MEM_ERROR;

    MRFSTR_DATA(str) = block;
    if (MRFSTR_SIZE(str) > size)
        MRFSTR_SIZE(str) = size;

    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_clear_realloc(
    mrfstr_t str, mrfstr_size_t size)
{
    free(MRFSTR_DATA(str));
    MRFSTR_SIZE(str) = 0;

    if (!size)
    {
        MRFSTR_DATA(str) = NULL;
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_DATA(str) = malloc(size * sizeof(mrfstr_chr_t));
    return MRFSTR_DATA(str) ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
}

mrfstr_res_enum_t mrfstr_expand(
    mrfstr_t str, mrfstr_size_t size)
{
    if (!MRFSTR_SIZE(str))
    {
        free(MRFSTR_DATA(str));
        MRFSTR_DATA(str) = malloc(size * sizeof(mrfstr_chr_t));
        return MRFSTR_DATA(str) ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
    }

    mrfstr_data_t block = realloc(MRFSTR_DATA(str), size * sizeof(mrfstr_chr_t));
    if (!block)
        return MRFSTR_RES_MEM_ERROR;

    MRFSTR_DATA(str) = block;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_clear_expand(
    mrfstr_t str, mrfstr_size_t size)
{
    free(MRFSTR_DATA(str));
    MRFSTR_SIZE(str) = 0;

    MRFSTR_DATA(str) = malloc(size * sizeof(mrfstr_chr_t));
    return MRFSTR_DATA(str) ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
}

mrfstr_res_enum_t mrfstr_shrink(
    mrfstr_t str, mrfstr_size_t size)
{
    if (!size)
    {
        free(MRFSTR_DATA(str));

        MRFSTR_DATA(str) = NULL;
        MRFSTR_SIZE(str) = 0;
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_data_t block = realloc(MRFSTR_DATA(str), size * sizeof(mrfstr_chr_t));
    if (!block)
        return MRFSTR_RES_MEM_ERROR;

    MRFSTR_DATA(str) = block;
    if (MRFSTR_SIZE(str) > size)
        MRFSTR_SIZE(str) = size;

    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_clear_shrink(
    mrfstr_t str, mrfstr_size_t size)
{
    free(MRFSTR_DATA(str));
    MRFSTR_SIZE(str) = 0;

    if (!size)
    {
        MRFSTR_DATA(str) = NULL;
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_DATA(str) = malloc(size * sizeof(mrfstr_chr_t));
    return MRFSTR_DATA(str) ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
}

void mrfstr_swap(
    mrfstr_t str1, mrfstr_t str2)
{
    mrfstr_data_t data = MRFSTR_DATA(str1);
    MRFSTR_DATA(str1) = MRFSTR_DATA(str2);
    MRFSTR_DATA(str2) = data;

    mrfstr_size_t size = MRFSTR_SIZE(str1);
    MRFSTR_SIZE(str1) = MRFSTR_SIZE(str2);
    MRFSTR_SIZE(str2) = size;
}
