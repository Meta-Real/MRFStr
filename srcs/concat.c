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

mrfstr_res_t mrfstr_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2)
{
    mrfstr_size_t size;

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

        size = MRFSTR_SIZE(res) + MRFSTR_SIZE(str2);
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

        size = MRFSTR_SIZE(str1) + MRFSTR_SIZE(res);
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

    size = MRFSTR_SIZE(str1) + MRFSTR_SIZE(str2);
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

mrfstr_res_t mrfstr_n_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2, mrfstr_size_t size)
{
    mrfstr_size_t nsize;

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

        nsize = MRFSTR_SIZE(res) + size;
        if (nsize < size)
            return MRFSTR_RES_OVERFLOW_ERROR;

        if (MRFSTR_CAPA(res) < nsize)
            MRFSTR_CLEAR_REALLOC(res, nsize);

        __mrfstr_copy(MRFSTR_DATA(res) + MRFSTR_SIZE(res), MRFSTR_DATA(str2), size);
        MRFSTR_SIZE(res) = nsize;
        return MRFSTR_RES_NOERROR;
    }

    nsize = MRFSTR_SIZE(str1) + size;
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

mrfstr_res_t mrfstr_concat_nstr(
    mrfstr_t res, mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size2)
{
    mrfstr_size_t size;

    if (res == str1)
    {
        if (!size2)
            return MRFSTR_RES_NOERROR;

        if (!MRFSTR_SIZE(res))
        {
            if (MRFSTR_CAPA(res) < size2)
                MRFSTR_CLEAR_REALLOC(res, size2);

            __mrfstr_copy(MRFSTR_DATA(res), str2, size2);
            MRFSTR_SIZE(res) = size2;
            return MRFSTR_RES_NOERROR;
        }

        size = MRFSTR_SIZE(res) + size2;
        if (size < MRFSTR_SIZE(res))
            return MRFSTR_RES_OVERFLOW_ERROR;

        if (MRFSTR_CAPA(res) < size)
            MRFSTR_CLEAR_REALLOC(res, size);

        __mrfstr_copy(MRFSTR_DATA(res) + MRFSTR_SIZE(res), str2, size2);
        MRFSTR_SIZE(res) = size;
        return MRFSTR_RES_NOERROR;
    }

    size = MRFSTR_SIZE(str1) + size2;
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
    if (size2)
        __mrfstr_copy(MRFSTR_DATA(res) + MRFSTR_SIZE(str1), str2, size2);

    MRFSTR_SIZE(res) = size;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_concat_chr(
    mrfstr_t res, mrfstr_ct str, mrfstr_chr_t chr)
{
    mrfstr_size_t size;

    if (res == str)
    {
        if (!MRFSTR_SIZE(res))
        {
            if (MRFSTR_CAPA(res) < 1)
                MRFSTR_CLEAR_REALLOC(res, 1);

            *MRFSTR_DATA(res) = chr;
            MRFSTR_SIZE(res) = 1;
            return MRFSTR_RES_NOERROR;
        }

        size = MRFSTR_SIZE(res) + 1;
        if (size < MRFSTR_SIZE(res))
            return MRFSTR_RES_OVERFLOW_ERROR;

        if (MRFSTR_CAPA(res) < size)
            MRFSTR_CLEAR_REALLOC(res, size);

        *MRFSTR_DATA(res) = chr;
        MRFSTR_SIZE(res) = size;
        return MRFSTR_RES_NOERROR;
    }

    size = MRFSTR_SIZE(str) + 1;
    if (size < MRFSTR_SIZE(str))
        return MRFSTR_RES_OVERFLOW_ERROR;

    if (MRFSTR_CAPA(res) < size)
        MRFSTR_CLEAR_REALLOC(res, size);

    if (MRFSTR_SIZE(str))
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(str));
    MRFSTR_DATA(res)[MRFSTR_SIZE(str)] = chr;

    MRFSTR_SIZE(res) = size;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_insert(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2, mrfstr_idx_t idx)
{
    mrfstr_data_t ptr;
    mrfstr_size_t size;

    if (idx > MRFSTR_SIZE(str1))
        return MRFSTR_RES_IDXOUT_ERROR;

    if (res == str1)
    {
        if (!MRFSTR_SIZE(str2))
            return MRFSTR_RES_NOERROR;

        size = MRFSTR_SIZE(res) - idx;
        MRFSTR_SIZE(res) += MRFSTR_SIZE(str2);
        if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
            MRFSTR_REALLOC(res, MRFSTR_SIZE(res));

        ptr = MRFSTR_DATA(res) + idx;
        if (res == str2)
        {
            mrfstr_data_t tail;

            tail = ptr + MRFSTR_SIZE(str2);
            if (size)
                __mrfstr_copy(tail, ptr, size);

            __mrfstr_copy(ptr, MRFSTR_DATA(res), idx);
            __mrfstr_copy(ptr + idx, tail, size);
            return MRFSTR_RES_NOERROR;
        }

        if (size)
        {
            if (size <= MRFSTR_SIZE(str2))
                __mrfstr_copy(ptr + MRFSTR_SIZE(str2), ptr, size);
            else
                __mrfstr_rmove(ptr + MRFSTR_SIZE(str2), ptr, size);
        }

        __mrfstr_copy(ptr, MRFSTR_DATA(str2), MRFSTR_SIZE(str2));
        return MRFSTR_RES_NOERROR;
    }

    if (res == str2)
    {
        if (!MRFSTR_SIZE(str1))
            return MRFSTR_RES_NOERROR;

        size = MRFSTR_SIZE(str1) + MRFSTR_SIZE(res);
        if (MRFSTR_CAPA(res) < size)
            MRFSTR_REALLOC(res, size);

        if (!MRFSTR_SIZE(res))
        {
            __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
            MRFSTR_SIZE(res) = MRFSTR_SIZE(str1);
            return MRFSTR_RES_NOERROR;
        }

        ptr = MRFSTR_DATA(res) + idx;
        size = MRFSTR_SIZE(str1) - idx;
        if (MRFSTR_SIZE(res) <= idx)
            __mrfstr_copy(ptr, MRFSTR_DATA(res), MRFSTR_SIZE(res));
        else
            __mrfstr_rmove(ptr, MRFSTR_DATA(res), MRFSTR_SIZE(res));

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), idx);
        __mrfstr_copy(ptr + MRFSTR_SIZE(res), MRFSTR_DATA(str2) + idx, size);
        MRFSTR_SIZE(res) += MRFSTR_SIZE(str1);
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_SIZE(res) = MRFSTR_SIZE(str1) + MRFSTR_SIZE(str2);
    if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
        MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(res));

    if (!MRFSTR_SIZE(str1))
    {
        if (!MRFSTR_SIZE(str2))
            return MRFSTR_RES_NOERROR;

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str2), MRFSTR_SIZE(str2));
        return MRFSTR_RES_NOERROR;
    }

    if (!MRFSTR_SIZE(str2))
    {
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
        return MRFSTR_RES_NOERROR;
    }

    if (idx)
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), idx);

    ptr = MRFSTR_DATA(res) + idx;
    __mrfstr_copy(ptr, MRFSTR_DATA(str2), MRFSTR_SIZE(str2));
    __mrfstr_copy(ptr + MRFSTR_SIZE(str2), MRFSTR_DATA(str1) + idx, MRFSTR_SIZE(str1) - idx);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_n_insert(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2, mrfstr_idx_t idx, mrfstr_size_t size)
{
    mrfstr_data_t ptr;
    mrfstr_size_t rsize;

    if (idx > MRFSTR_SIZE(str1))
        return MRFSTR_RES_IDXOUT_ERROR;

    if (size > MRFSTR_SIZE(str2))
        size = MRFSTR_SIZE(str2);

    if (res == str1)
    {
        if (!size)
            return MRFSTR_RES_NOERROR;

        rsize = MRFSTR_SIZE(res) - idx;
        MRFSTR_SIZE(res) += size;
        if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
            MRFSTR_REALLOC(res, MRFSTR_SIZE(res));

        ptr = MRFSTR_DATA(res) + idx;
        if (res == str2)
        {
            mrfstr_data_t tail;

            tail = ptr + size;
            if (rsize)
                __mrfstr_copy(tail, ptr, rsize);

            __mrfstr_copy(ptr, MRFSTR_DATA(res), idx);
            __mrfstr_copy(ptr + idx, tail, rsize);
            return MRFSTR_RES_NOERROR;
        }

        if (rsize)
        {
            if (rsize <= size)
                __mrfstr_copy(ptr + size, ptr, rsize);
            else
                __mrfstr_rmove(ptr + size, ptr, rsize);
        }

        __mrfstr_copy(ptr, MRFSTR_DATA(str2), size);
        return MRFSTR_RES_NOERROR;
    }

    if (res == str2)
    {
        if (!MRFSTR_SIZE(str1))
            return MRFSTR_RES_NOERROR;

        rsize = MRFSTR_SIZE(str1) + MRFSTR_SIZE(res);
        if (MRFSTR_CAPA(res) < rsize)
            MRFSTR_REALLOC(res, rsize);

        if (!MRFSTR_SIZE(res))
        {
            __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
            MRFSTR_SIZE(res) = MRFSTR_SIZE(str1);
            return MRFSTR_RES_NOERROR;
        }

        ptr = MRFSTR_DATA(res) + idx;
        rsize = MRFSTR_SIZE(str1) - idx;
        if (MRFSTR_SIZE(res) <= idx)
            __mrfstr_copy(ptr, MRFSTR_DATA(res), MRFSTR_SIZE(res));
        else
            __mrfstr_rmove(ptr, MRFSTR_DATA(res), MRFSTR_SIZE(res));

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), idx);
        __mrfstr_copy(ptr + MRFSTR_SIZE(res), MRFSTR_DATA(str2) + idx, size);
        MRFSTR_SIZE(res) += MRFSTR_SIZE(str1);
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_SIZE(res) = MRFSTR_SIZE(str1) + size;
    if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
        MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(res));

    if (!MRFSTR_SIZE(str1))
    {
        if (!size)
            return MRFSTR_RES_NOERROR;

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str2), size);
        return MRFSTR_RES_NOERROR;
    }

    if (!size)
    {
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
        return MRFSTR_RES_NOERROR;
    }

    if (idx)
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), idx);

    ptr = MRFSTR_DATA(res) + idx;
    __mrfstr_copy(ptr, MRFSTR_DATA(str2), size);
    __mrfstr_copy(ptr + size, MRFSTR_DATA(str1) + idx, MRFSTR_SIZE(str1) - idx);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_insert_nstr(
    mrfstr_t res, mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size2, mrfstr_idx_t idx)
{
    mrfstr_data_t ptr;
    mrfstr_size_t size;

    if (idx > MRFSTR_SIZE(str1))
        return MRFSTR_RES_IDXOUT_ERROR;

    if (res == str1)
    {
        if (!size2)
            return MRFSTR_RES_NOERROR;

        size = MRFSTR_SIZE(res) - idx;
        MRFSTR_SIZE(res) += size2;
        if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
            MRFSTR_REALLOC(res, MRFSTR_SIZE(res));

        ptr = MRFSTR_DATA(res) + idx;
        if (size)
        {
            if (size <= size2)
                __mrfstr_copy(ptr + size2, ptr, size);
            else
                __mrfstr_rmove(ptr + size2, ptr, size);
        }

        __mrfstr_copy(ptr, str2, size2);
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_SIZE(res) = MRFSTR_SIZE(str1) + size2;
    if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
        MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(res));

    if (!MRFSTR_SIZE(str1))
    {
        if (!size2)
            return MRFSTR_RES_NOERROR;

        __mrfstr_copy(MRFSTR_DATA(res), str2, size2);
        return MRFSTR_RES_NOERROR;
    }

    if (!size2)
    {
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
        return MRFSTR_RES_NOERROR;
    }

    if (idx)
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str1), idx);

    ptr = MRFSTR_DATA(res) + idx;
    __mrfstr_copy(ptr, str2, size2);
    __mrfstr_copy(ptr + size2, MRFSTR_DATA(str1) + idx, MRFSTR_SIZE(str1) - idx);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_insert_chr(
    mrfstr_t res, mrfstr_ct str, mrfstr_chr_t chr, mrfstr_idx_t idx)
{
    mrfstr_data_t ptr;
    mrfstr_size_t size;

    if (idx > MRFSTR_SIZE(str))
        return MRFSTR_RES_IDXOUT_ERROR;

    if (res == str)
    {
        size = MRFSTR_SIZE(res) - idx;
        MRFSTR_SIZE(res)++;
        if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
            MRFSTR_REALLOC(res, MRFSTR_SIZE(res));

        ptr = MRFSTR_DATA(res) + idx;
        if (size)
            __mrfstr_rmove(ptr + 1, ptr, size);

        *ptr = chr;
        return MRFSTR_RES_NOERROR;
    }

    MRFSTR_SIZE(res) = MRFSTR_SIZE(str) + 1;
    if (MRFSTR_CAPA(res) < MRFSTR_SIZE(res))
        MRFSTR_CLEAR_REALLOC(res, MRFSTR_SIZE(res));

    if (!MRFSTR_SIZE(str))
    {
        *MRFSTR_DATA(res) = chr;
        return MRFSTR_RES_NOERROR;
    }

    if (idx)
        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), idx);

    ptr = MRFSTR_DATA(res) + idx;
    *ptr = chr;
    __mrfstr_copy(ptr + 1, MRFSTR_DATA(str) + idx, MRFSTR_SIZE(str) - idx);
    return MRFSTR_RES_NOERROR;
}
