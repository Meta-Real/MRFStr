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
#include <limits.h>

void mrfstr_export(
    mrfstr_ct str)
{
    if (!MRFSTR_SIZE(str))
        return;

    fwrite(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), MRFSTR_SIZE(str), stdout);
}

void mrfstr_n_export(
    mrfstr_ct str, mrfstr_size_t size)
{
    if (size > MRFSTR_SIZE(str))
        size = MRFSTR_SIZE(str);

    if (!size)
        return;

    fwrite(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), size, stdout);
}

mrfstr_res_enum_t mrfstr_import(
    mrfstr_t str)
{
    MRFSTR_SIZE(str) = 0;

    mrfstr_size_t ncapa = MRFSTR_CAPA(str);
    mrfstr_data_t ptr = MRFSTR_DATA(str);
    while (ncapa > INT_MAX)
    {
        ncapa -= INT_MAX;

        ptr[INT_MAX - 1] = EOF;
        fgets(ptr, INT_MAX * sizeof(mrfstr_chr_t), stdin);

        if (ptr[INT_MAX - 1] != '\0' ||
            (ptr[INT_MAX - 1] == '\0' && ptr[INT_MAX - 2] == '\n'))
        {
            MRFSTR_SIZE(str) += mrfstr_strlen(ptr) - 1;
            return MRFSTR_RES_NOERROR;
        }

        ptr += INT_MAX;
        MRFSTR_SIZE(str) += INT_MAX - 1;
    }

    if (ncapa-- > 1)
    {
        ptr[ncapa] = EOF;
        fgets(ptr, (int)((ncapa + 1) * sizeof(mrfstr_chr_t)), stdin);

        if (ptr[ncapa] != '\0' ||
            (ptr[ncapa] == '\0' && ptr[ncapa - 1] == '\n'))
        {
            MRFSTR_SIZE(str) += mrfstr_strlen(ptr) - 1;
            return MRFSTR_RES_NOERROR;
        }

        MRFSTR_SIZE(str) += ncapa;
    }

    ncapa = (int)_mrfstr_config.stdin_alloc + 1;
    mrfstr_byte_t newline_idx = _mrfstr_config.stdin_alloc - 1;
    while (1)
    {
        MRFSTR_CAPA(str) += ncapa;
        MRFSTR_REALLOC(str, MRFSTR_CAPA(str));

        ptr = MRFSTR_DATA(str) + MRFSTR_SIZE(str);
        ptr[_mrfstr_config.stdin_alloc] = EOF;

        fgets(ptr, (int)(ncapa * sizeof(mrfstr_chr_t)), stdin);

        if (ptr[_mrfstr_config.stdin_alloc] != '\0' ||
            (ptr[_mrfstr_config.stdin_alloc] == '\0' && ptr[newline_idx] == '\n'))
        {
            MRFSTR_SIZE(str) += mrfstr_strlen(ptr) - 1;
            return MRFSTR_RES_NOERROR;
        }

        MRFSTR_SIZE(str) += _mrfstr_config.stdin_alloc;
    }
}

mrfstr_res_enum_t mrfstr_n_import(
    mrfstr_t str, mrfstr_size_t size)
{
    if (!size)
    {
        MRFSTR_SIZE(str) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (MRFSTR_CAPA(str) < size)
        MRFSTR_CLEAR_REALLOC(str, size);

    fread(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), size, stdin);
    MRFSTR_SIZE(str) = size;
    return MRFSTR_RES_NOERROR;
}

void mrfstr_fexport(
    FILE *stream,
    mrfstr_ct str)
{
    if (!MRFSTR_SIZE(str))
        return;

    fwrite(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), MRFSTR_SIZE(str), stream);
}

void mrfstr_n_fexport(
    FILE *stream,
    mrfstr_ct str, mrfstr_size_t size)
{
    if (size > MRFSTR_SIZE(str))
        size = MRFSTR_SIZE(str);

    if (!size)
        return;

    fwrite(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), size, stream);
}
