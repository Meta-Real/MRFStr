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

mrfstr_res_t mrfstr_remove(
    mrfstr_t res, mrfstr_ct str, mrfstr_idx_t idx)
{
    mrfstr_size_t size;

    if (idx >= MRFSTR_SIZE(str))
        return MRFSTR_RES_IDXOUT_ERROR;

    if (MRFSTR_SIZE(str) == 1)
    {
        MRFSTR_SIZE(res) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (res == str)
    {
        size = --MRFSTR_SIZE(res) - idx;
        if (!size)
            return MRFSTR_RES_NOERROR;

        __mrfstr_move(MRFSTR_DATA(res) + idx, MRFSTR_DATA(res) + idx + 1, size);
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_SIZE(res) = MRFSTR_SIZE(str) - 1;
    if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
        MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(res));

    if (idx)
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), idx);

    size = MRFSTR_SIZE(res) - idx;
    if (size)
        __mrfstr_copy(MRFSTR_DATA(res) + idx, MRFSTR_DATA(str) + idx + 1, size);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_n_remove(
    mrfstr_t res, mrfstr_ct str, mrfstr_idx_t idx, mrfstr_size_t count)
{
    if (idx >= MRFSTR_SIZE(str))
        return MRFSTR_RES_IDXOUT_ERROR;

    if (!count)
    {
        if (res == str)
            return MRFSTR_RES_NOERROR;

        if (!MRFSTR_SIZE(str))
        {
            MRFSTR_SIZE(res) = 0;
            return MRFSTR_RES_NOERROR;
        }

        if (MRFSTR_CAPA(res) < MRFSTR_SIZE(str))
            MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(str));

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(res));
        MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
        return MRFSTR_RES_NOERROR;
    }

    if (res == str)
    {
        mrfstr_size_t size;

        if (count >= MRFSTR_SIZE(res) - idx)
        {
            MRFSTR_SIZE(res) = idx;
            return MRFSTR_RES_NOERROR;
        }

        MRFSTR_SIZE(res) -= count;
        size = MRFSTR_SIZE(res) - idx;

        __mrfstr_move(MRFSTR_DATA(res) + idx, MRFSTR_DATA(res) + idx + count, size);
        return MRFSTR_RES_NOERROR;
    }

    if (count >= MRFSTR_SIZE(str) - idx)
        MRFSTR_SIZE(res) = idx;
    else
        MRFSTR_SIZE(res) = MRFSTR_SIZE(str) - count;

    if (!MRFSTR_SIZE(res))
        return MRFSTR_RES_NOERROR;

    if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
        MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(res));

    if (idx)
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), idx);

    if (MRFSTR_SIZE(res) == idx)
        return MRFSTR_RES_NOERROR;

    __mrfstr_copy(MRFSTR_DATA(res) + idx, MRFSTR_DATA(str) + idx + count, MRFSTR_SIZE(res) - idx);
    return MRFSTR_RES_NOERROR;
}
