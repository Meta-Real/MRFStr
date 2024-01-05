/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

mrfstr_res_enum_t mrfstr_repeat(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_size_t count)
{
    if (MRFSTR_SIZE(str) == 1)
    {
        mrfstr_repeat_chr(res, *MRFSTR_DATA(str), count);
        return MRFSTR_RES_NOERROR;
    }

    if (!(count && MRFSTR_SIZE(str)))
    {
        free(MRFSTR_DATA(res));

        MRFSTR_DATA(res) = NULL;
        MRFSTR_SIZE(res) = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (count == 1)
    {
        if (res == str)
            return MRFSTR_RES_NOERROR;

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(str));
        MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
        return MRFSTR_RES_NOERROR;
    }

    if (res == str)
    {
        mrfstr_size_t size = MRFSTR_SIZE(res) * count;
        if (size / count != MRFSTR_SIZE(res))
            return MRFSTR_RES_OVERFLOW_ERROR;

        // We can do better
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

    mrfstr_size_t size = MRFSTR_SIZE(str) * count;
    if (size / count != MRFSTR_SIZE(str))
        return MRFSTR_RES_OVERFLOW_ERROR;

    __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(str));
    MRFSTR_SIZE(res) = MRFSTR_SIZE(str);

    // We can do better
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

void mrfstr_repeat_chr(
    mrfstr_t res, mrfstr_chr_t chr,
    mrfstr_size_t count)
{
    if (!count)
    {
        free(MRFSTR_DATA(res));

        MRFSTR_DATA(res) = NULL;
        MRFSTR_SIZE(res) = 0;
        return;
    }

    __mrfstr_fill(MRFSTR_DATA(res), chr, count);
    MRFSTR_SIZE(res) = count;
}
