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

mrfstr_res_t mrfstr_import(
    mrfstr_t str)
{
    mrfstr_data_t ptr;
    mrfstr_size_t capa;
    mrfstr_long_t alloc;
    mrfstr_short_t newline_idx;

    MRFSTR_SIZE(str) = 0;
    capa = MRFSTR_CAPA(str);
    ptr = MRFSTR_DATA(str);

    alloc = (mrfstr_long_t)_mrfstr_config.stdalloc + 1;
    newline_idx = _mrfstr_config.stdalloc - 1;
    while (capa > alloc)
    {
        capa -= alloc;

        ptr[_mrfstr_config.stdalloc] = EOF;
        if (!fgets(ptr, alloc * sizeof(mrfstr_chr_t), stdin))
            return MRFSTR_RES_READ_ERROR;

        if (ptr[_mrfstr_config.stdalloc] == EOF || ptr[newline_idx] == '\n')
        {
            MRFSTR_SIZE(str) += strlen(ptr) - 1; //mrfstr_strlen(ptr) - 1;
            return MRFSTR_RES_NOERROR;
        }

        ptr += _mrfstr_config.stdalloc;
        MRFSTR_SIZE(str) += _mrfstr_config.stdalloc;
    }

    while (1)
    {
        MRFSTR_CAPA(str) += alloc;
        MRFSTR_REALLOC(str, MRFSTR_CAPA(str));
        ptr = MRFSTR_DATA(str) + MRFSTR_SIZE(str);

        ptr[_mrfstr_config.stdalloc] = EOF;
        if (!fgets(ptr, alloc * sizeof(mrfstr_chr_t), stdin))
            return MRFSTR_RES_READ_ERROR;

        if (ptr[_mrfstr_config.stdalloc] == EOF || ptr[newline_idx] == '\n')
        {
            MRFSTR_SIZE(str) += strlen(ptr) - 1; //mrfstr_strlen(ptr) - 1;
            return MRFSTR_RES_NOERROR;
        }

        MRFSTR_SIZE(str) += _mrfstr_config.stdalloc;
    }
}

mrfstr_res_t mrfstr_n_import(
    mrfstr_t str, mrfstr_size_t size)
{
    if (!size)
    {
        MRFSTR_SIZE(str) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (MRFSTR_CAPA(str) < size)
        MRFSTR_CLEAR_REALLOC(str, size);

    if (!fread(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), size, stdin))
        return MRFSTR_RES_READ_ERROR;

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

mrfstr_res_t mrfstr_fimport(
    mrfstr_t str,
    FILE *stream)
{
    if (!stream || stream == stdin)
        return mrfstr_import(str);

#ifdef MRFSTR_BUILD_UNIX
    fseeko64(stream, 0, SEEK_END);
    MRFSTR_SIZE(str) = ftello64(stream);
    fseeko64(stream, 0, SEEK_END);
#elif defined(_MSC_VER)
    _fseeki64(stream, 0, SEEK_END);
    MRFSTR_SIZE(str) = _ftelli64(stream);
    _fseeki64(stream, 0, SEEK_END);
#endif

    if (MRFSTR_CAPA(str) < MRFSTR_SIZE(str))
        MRFSTR_CLEAR_REALLOC(str, MRFSTR_SIZE(str));

    if (!fread(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), MRFSTR_SIZE(str), stream))
        return MRFSTR_RES_READ_ERROR;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_n_fimport(
    mrfstr_t str,
    FILE *stream, mrfstr_size_t size)
{
    if (!size)
    {
        MRFSTR_SIZE(str) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (MRFSTR_CAPA(str) < size)
        MRFSTR_CLEAR_REALLOC(str, size);

    if (!stream)
        stream = stdin;

    if (!fread(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), size, stream))
        return MRFSTR_RES_READ_ERROR;
    MRFSTR_SIZE(str) = size;
    return MRFSTR_RES_NOERROR;
}
