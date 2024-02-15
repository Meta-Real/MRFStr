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

mrfstr_res_t mrfstr_replace_chr(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr)
{
    if (!MRFSTR_SIZE(str))
    {
        MRFSTR_SIZE(res) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (ochr == nchr)
    {
        if (res == str)
            return MRFSTR_RES_NOERROR;

        if (MRFSTR_CAPA(res) < MRFSTR_SIZE(str))
            MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(str));

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(str));
        MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
        return MRFSTR_RES_NOERROR;
    }

    if (res == str)
    {
        __mrfstr_replchr(MRFSTR_DATA(res), ochr, nchr, MRFSTR_SIZE(res));
        return MRFSTR_RES_NOERROR;
    }

    if (MRFSTR_CAPA(res) < MRFSTR_SIZE(str))
        MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(str));

    __mrfstr_replchr2(MRFSTR_DATA(res), MRFSTR_DATA(str), ochr, nchr, MRFSTR_SIZE(str));
    MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_n_replace_chr(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size)
{
    if (!MRFSTR_SIZE(str))
    {
        MRFSTR_SIZE(res) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (ochr == nchr || !size)
    {
        if (res == str)
            return MRFSTR_RES_NOERROR;

        if (MRFSTR_CAPA(res) < MRFSTR_SIZE(str))
            MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(str));

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(str));
        MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
        return MRFSTR_RES_NOERROR;
    }

    if (size > MRFSTR_SIZE(str))
        size = MRFSTR_SIZE(str);

    if (res == str)
    {
        __mrfstr_replchr(MRFSTR_DATA(res), ochr, nchr, size);
        return MRFSTR_RES_NOERROR;
    }

    if (MRFSTR_CAPA(res) < MRFSTR_SIZE(str))
        MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(str));

    __mrfstr_replchr2(MRFSTR_DATA(res), MRFSTR_DATA(str), ochr, nchr, size);
    if (size != MRFSTR_SIZE(str))
        __mrfstr_copy(MRFSTR_DATA(res) + size, MRFSTR_DATA(str) + size, MRFSTR_SIZE(str) - size);

    MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
    return MRFSTR_RES_NOERROR;
}
