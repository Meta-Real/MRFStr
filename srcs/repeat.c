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

mrfstr_res_t mrfstr_repeat(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_size_t count)
{
    mrfstr_size_t size;

    if (MRFSTR_SIZE(str) == 1)
        return mrfstr_repeat_chr(res, *MRFSTR_DATA(str), count);

    if (!(count && MRFSTR_SIZE(str)))
    {
        MRFSTR_SIZE(res) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (count == 1)
    {
        if (res == str)
            return MRFSTR_RES_NOERROR;

        if (MRFSTR_CAPA(res) < MRFSTR_SIZE(str))
            MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(str));

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(str));
        MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
        return MRFSTR_RES_NOERROR;
    }

    size = MRFSTR_SIZE(str) * count;
    if (size / count != MRFSTR_SIZE(str))
        return MRFSTR_RES_OVERFLOW_ERROR;

    if (MRFSTR_CAPA(res) < size)
        MRFSTR_CLEAR_REALLOC(res, size);

    if (res != str)
    {
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(str));
        MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
    }

    while (MRFSTR_SIZE(res) <= size - MRFSTR_SIZE(res))
    {
        __mrfstr_copy(MRFSTR_DATA(res) + MRFSTR_SIZE(res), MRFSTR_DATA(res), MRFSTR_SIZE(res));
        MRFSTR_SIZE(res) <<= 1;
    }

    MRFSTR_SIZE(res) = size - MRFSTR_SIZE(res);
    if (MRFSTR_SIZE(res))
        __mrfstr_copy(MRFSTR_DATA(res) + MRFSTR_SIZE(res), MRFSTR_DATA(res), MRFSTR_SIZE(res));

    MRFSTR_SIZE(res) = size;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_repeat_chr(
    mrfstr_t res, mrfstr_chr_t chr,
    mrfstr_size_t count)
{
    MRFSTR_SIZE(res) = count;
    if (!count)
        return MRFSTR_RES_NOERROR;

    if (MRFSTR_CAPA(res) < count)
        MRFSTR_CLEAR_REALLOC(res, count);

    __mrfstr_fill(MRFSTR_DATA(res), chr, count);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_fill(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (MRFSTR_SIZE(str) >= size)
    {
        if (res == str)
            return MRFSTR_RES_NOERROR;

        MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
        if (!MRFSTR_SIZE(res))
            return MRFSTR_RES_NOERROR;

        if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
            MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(res));

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(res));
        return MRFSTR_RES_NOERROR;
    }

    if (res == str)
    {
        mrfstr_size_t diff;

        if (MRFSTR_CAPA(res) < size)
            MRFSTR_REALLOC(res, size);

        diff = size - MRFSTR_SIZE(res);
        if (MRFSTR_SIZE(res))
        {
            if (diff >= MRFSTR_SIZE(res))
                __mrfstr_copy(MRFSTR_DATA(res) + diff, MRFSTR_DATA(res), MRFSTR_SIZE(res));
            else
                __mrfstr_rmove(MRFSTR_DATA(res) + diff, MRFSTR_DATA(res), MRFSTR_SIZE(res));
        }

        __mrfstr_fill(MRFSTR_DATA(res), chr, diff);
        MRFSTR_SIZE(res) = size;
        return MRFSTR_RES_NOERROR;
    }

    if (MRFSTR_CAPA(res) < size)
        MRFSTR_CLEAR_REALLOC(res, size);

    MRFSTR_SIZE(res) = size;
    size -= MRFSTR_SIZE(str);

    __mrfstr_fill(MRFSTR_DATA(res), chr, size);
    if (MRFSTR_SIZE(str))
        __mrfstr_copy(MRFSTR_DATA(res) + size, MRFSTR_DATA(str), MRFSTR_SIZE(str));
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_rfill(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (MRFSTR_SIZE(str) >= size)
    {
        if (res == str)
            return MRFSTR_RES_NOERROR;

        MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
        if (!MRFSTR_SIZE(res))
            return MRFSTR_RES_NOERROR;

        if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
            MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(res));

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(res));
        return MRFSTR_RES_NOERROR;
    }

    if (res == str)
    {
        if (MRFSTR_CAPA(res) < size)
            MRFSTR_REALLOC(res, size);

        __mrfstr_fill(MRFSTR_DATA(res) + MRFSTR_SIZE(res), chr, size - MRFSTR_SIZE(res));
        MRFSTR_SIZE(res) = size;
        return MRFSTR_RES_NOERROR;
    }

    if (MRFSTR_CAPA(res) < size)
        MRFSTR_CLEAR_REALLOC(res, size);

    MRFSTR_SIZE(res) = size;
    size -= MRFSTR_SIZE(str);

    if (MRFSTR_SIZE(str))
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(str));
    __mrfstr_fill(MRFSTR_DATA(res) + MRFSTR_SIZE(str), chr, size);
    return MRFSTR_RES_NOERROR;
}
