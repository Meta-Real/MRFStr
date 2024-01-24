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

mrfstr_res_enum_t mrfstr_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2)
{
    if (res == str1)
    {
        if (!MRFSTR_SIZE(str2))
            return MRFSTR_RES_NOERROR;

        if (!MRFSTR_SIZE(res))
        {
            if (MRFSTR_CAPA(res) < MRFSTR_SIZE(str2))
                MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(str2));

            __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str2), MRFSTR_SIZE(str2));
            MRFSTR_SIZE(res) = MRFSTR_SIZE(str2);
            return MRFSTR_RES_NOERROR;
        }

        mrfstr_size_t size = MRFSTR_SIZE(res) + MRFSTR_SIZE(str2);
        if (size < MRFSTR_SIZE(res))
            return MRFSTR_RES_OVERFLOW_ERROR;

        if (MRFSTR_CAPA(res) < size)
            MRFSTR_CLEAR_REALLOC(res, size);

        __mrfstr_copy(MRFSTR_DATA(res) + MRFSTR_SIZE(res), MRFSTR_DATA(str2), MRFSTR_SIZE(str2));
        MRFSTR_SIZE(res) = size;
        return MRFSTR_RES_NOERROR;
    }

    if (res == str2)
    {
        if (!MRFSTR_SIZE(str1))
            return MRFSTR_RES_NOERROR;

        if (!MRFSTR_SIZE(res))
        {
            if (MRFSTR_CAPA(res) < MRFSTR_SIZE(str1))
                MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(str1));

            __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
            MRFSTR_SIZE(res) = MRFSTR_SIZE(str1);
            return MRFSTR_RES_NOERROR;
        }

        mrfstr_size_t size = MRFSTR_SIZE(str1) + MRFSTR_SIZE(res);
        if (size < MRFSTR_SIZE(res))
            return MRFSTR_RES_OVERFLOW_ERROR;

        if (MRFSTR_CAPA(res) < size)
            MRFSTR_CLEAR_REALLOC(res, size);

        if (MRFSTR_SIZE(str1) >= MRFSTR_SIZE(res))
            __mrfstr_copy(MRFSTR_DATA(res) + MRFSTR_SIZE(str1),
                MRFSTR_DATA(res), MRFSTR_SIZE(res));
        else
            __mrfstr_rmove(MRFSTR_DATA(res) + MRFSTR_SIZE(str1),
                MRFSTR_DATA(res), MRFSTR_SIZE(res));

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
        MRFSTR_SIZE(res) = size;
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_size_t size = MRFSTR_SIZE(str1) + MRFSTR_SIZE(str2);
    if (size < MRFSTR_SIZE(str1))
        return MRFSTR_RES_OVERFLOW_ERROR;

    if (!size)
    {
        MRFSTR_SIZE(res) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (MRFSTR_CAPA(res) < size)
        MRFSTR_CLEAR_REALLOC(res, size);

    if (MRFSTR_SIZE(str1))
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
    if (MRFSTR_SIZE(str2))
        __mrfstr_copy(MRFSTR_DATA(res) + MRFSTR_SIZE(str1), MRFSTR_DATA(str2), MRFSTR_SIZE(str2));

    MRFSTR_SIZE(res) = size;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_n_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2,
    mrfstr_size_t size)
{
    if (size > MRFSTR_SIZE(str2))
        size = MRFSTR_SIZE(str2);

    if (res == str1)
    {
        if (!size)
            return MRFSTR_RES_NOERROR;

        if (!MRFSTR_SIZE(res))
        {
            if (MRFSTR_CAPA(res) < size)
                MRFSTR_CLEAR_REALLOC(res, size);

            __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str2), size);
            MRFSTR_SIZE(res) = size;
            return MRFSTR_RES_NOERROR;
        }

        mrfstr_size_t nsize = MRFSTR_SIZE(res) + size;
        if (nsize < size)
            return MRFSTR_RES_OVERFLOW_ERROR;

        if (MRFSTR_CAPA(res) < nsize)
            MRFSTR_CLEAR_REALLOC(res, nsize);

        __mrfstr_copy(MRFSTR_DATA(res) + MRFSTR_SIZE(res), MRFSTR_DATA(str2), size);
        MRFSTR_SIZE(res) = nsize;
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_size_t nsize = MRFSTR_SIZE(str1) + size;
    if (nsize < size)
        return MRFSTR_RES_OVERFLOW_ERROR;

    if (!nsize)
    {
        MRFSTR_SIZE(res) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (MRFSTR_CAPA(res) < nsize)
        MRFSTR_CLEAR_REALLOC(res, nsize);

    if (MRFSTR_SIZE(str1))
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
    if (size)
        __mrfstr_copy(MRFSTR_DATA(res) + MRFSTR_SIZE(str1), MRFSTR_DATA(str2), size);

    MRFSTR_SIZE(res) = nsize;
    return MRFSTR_RES_NOERROR;
}
