/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>

mrfstr_res_enum_t mrfstr_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2)
{
    if (res == str1)
    {
        if (!MRFSTR_SIZE(str2))
            return MRFSTR_RES_NOERROR;

        if (!MRFSTR_SIZE(res))
        {
            mrfstr_memcpy(MRFSTR_DATA(res), MRFSTR_DATA(str2), MRFSTR_SIZE(str2));
            MRFSTR_SIZE(res) = MRFSTR_SIZE(str2);
            return MRFSTR_RES_NOERROR;
        }

        mrfstr_size_t size = MRFSTR_SIZE(res) + MRFSTR_SIZE(str2);
        if (size < MRFSTR_SIZE(res))
            return MRFSTR_RES_OVERFLOW_ERROR;

        mrfstr_memcpy(MRFSTR_DATA(res) + MRFSTR_SIZE(res), MRFSTR_DATA(str2), MRFSTR_SIZE(str2));
        MRFSTR_SIZE(res) = size;
        return MRFSTR_RES_NOERROR;
    }

    if (res == str2)
    {
        // For later support
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_size_t size = MRFSTR_SIZE(str1) + MRFSTR_SIZE(str2);
    if (size < MRFSTR_SIZE(str1))
        return MRFSTR_RES_OVERFLOW_ERROR;

    mrfstr_memcpy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
    mrfstr_memcpy(MRFSTR_DATA(res) + MRFSTR_SIZE(str1), MRFSTR_DATA(str2), MRFSTR_SIZE(str2));

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
            mrfstr_memcpy(MRFSTR_DATA(res), MRFSTR_DATA(str2), size);
            MRFSTR_SIZE(res) = size;
            return MRFSTR_RES_NOERROR;
        }

        mrfstr_size_t nsize = MRFSTR_SIZE(res) + size;
        if (nsize < size)
            return MRFSTR_RES_OVERFLOW_ERROR;

        mrfstr_memcpy(MRFSTR_DATA(res) + MRFSTR_SIZE(res), MRFSTR_DATA(str2), size);
        MRFSTR_SIZE(res) = nsize;
        return MRFSTR_RES_NOERROR;
    }

    if (res == str2)
    {
        // For later support
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_size_t nsize = MRFSTR_SIZE(str1) + size;
    if (nsize < size)
        return MRFSTR_RES_OVERFLOW_ERROR;

    mrfstr_memcpy(MRFSTR_DATA(res), MRFSTR_DATA(str1), MRFSTR_SIZE(str1));
    mrfstr_memcpy(MRFSTR_DATA(res) + MRFSTR_SIZE(str1), MRFSTR_DATA(str2), size);

    MRFSTR_SIZE(res) = nsize;
    return MRFSTR_RES_NOERROR;
}
