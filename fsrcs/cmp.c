/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

mrfstr_bool_t mrfstr_equal(
    mrfstr_ct str1, mrfstr_ct str2)
{
    if (str1 == str2)
        return MRFSTR_TRUE;

    if (MRFSTR_SIZE(str1) != MRFSTR_SIZE(str2))
        return MRFSTR_FALSE;

    if (!MRFSTR_SIZE(str1))
        return MRFSTR_TRUE;

    return mrfstr_memcmp(MRFSTR_DATA(str1), MRFSTR_DATA(str2), MRFSTR_SIZE(str1));
}

mrfstr_bool_t mrfstr_n_equal(
    mrfstr_ct str1, mrfstr_ct str2,
    mrfstr_size_t size)
{
    if (str1 == str2)
        return MRFSTR_TRUE;

    if (MRFSTR_SIZE(str1) < size)
    {
        if (MRFSTR_SIZE(str1) != MRFSTR_SIZE(str2))
            return MRFSTR_FALSE;

        size = MRFSTR_SIZE(str1);
    }
    else if (MRFSTR_SIZE(str2) < size)
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(MRFSTR_DATA(str1), MRFSTR_DATA(str2), size);
}

mrfstr_bool_t mrfstr_equal_str(
    mrfstr_ct str1, mrfstr_data_ct str2)
{
    if (MRFSTR_SIZE(str1) != strlen(str2))
        return MRFSTR_FALSE;

    if (!MRFSTR_SIZE(str1))
        return MRFSTR_TRUE;

    return mrfstr_memcmp(MRFSTR_DATA(str1), str2, MRFSTR_SIZE(str1));
}

mrfstr_bool_t mrfstr_n_equal_str(
    mrfstr_ct str1, mrfstr_data_ct str2,
    mrfstr_size_t size)
{
    if (MRFSTR_SIZE(str1) < size)
    {
        if (MRFSTR_SIZE(str1) != strlen(str2))
            return MRFSTR_FALSE;

        size = MRFSTR_SIZE(str1);
    }
    else if (strlen(str2) < size)
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(MRFSTR_DATA(str1), str2, size);
}

mrfstr_bool_t mrfstr_equal_nstr(
    mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
    if (MRFSTR_SIZE(str1) != size)
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(MRFSTR_DATA(str1), str2, size);
}

mrfstr_bool_t mrfstr_n_equal_nstr(
    mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size2,
    mrfstr_size_t size)
{
    if (MRFSTR_SIZE(str1) < size)
    {
        if (MRFSTR_SIZE(str1) != size2)
            return MRFSTR_FALSE;

        size = size2;
    }
    else if (size2 < size)
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return mrfstr_memcmp(MRFSTR_DATA(str1), str2, size);
}

mrfstr_bool_t mrfstr_equal_chr(
    mrfstr_ct str, mrfstr_chr_t chr)
{
    if (MRFSTR_SIZE(str) != 1)
        return MRFSTR_FALSE;

    return *MRFSTR_DATA(str) == chr;
}
