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

mrfstr_bool_t mrfstr_equal(
    mrfstr_ct str1, mrfstr_ct str2)
{
    if (str1 == str2)
        return MRFSTR_TRUE;

    if (MRFSTR_SIZE(str1) != MRFSTR_SIZE(str2))
        return MRFSTR_FALSE;

    if (!MRFSTR_SIZE(str1))
        return MRFSTR_TRUE;

    return __mrfstr_equal(MRFSTR_DATA(str1), MRFSTR_DATA(str2), MRFSTR_SIZE(str1));
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

    return __mrfstr_equal(MRFSTR_DATA(str1), MRFSTR_DATA(str2), size);
}

mrfstr_bool_t mrfstr_equal_str(
    mrfstr_ct str1, mrfstr_data_ct str2)
{
    if (MRFSTR_SIZE(str1) != mrfstr_strlen(str2))
        return MRFSTR_FALSE;

    if (!MRFSTR_SIZE(str1))
        return MRFSTR_TRUE;

    return __mrfstr_equal(MRFSTR_DATA(str1), str2, MRFSTR_SIZE(str1));
}

mrfstr_bool_t mrfstr_n_equal_str(
    mrfstr_ct str1, mrfstr_data_ct str2,
    mrfstr_size_t size)
{
    if (MRFSTR_SIZE(str1) < size)
    {
        if (MRFSTR_SIZE(str1) != mrfstr_strlen(str2))
            return MRFSTR_FALSE;

        size = MRFSTR_SIZE(str1);
    }
    else if (mrfstr_strlen(str2) < size)
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return __mrfstr_equal(MRFSTR_DATA(str1), str2, size);
}

mrfstr_bool_t mrfstr_equal_nstr(
    mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
    if (MRFSTR_SIZE(str1) != size)
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return __mrfstr_equal(MRFSTR_DATA(str1), str2, size);
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

    return __mrfstr_equal(MRFSTR_DATA(str1), str2, size);
}

mrfstr_bool_t mrfstr_equal_chr(
    mrfstr_ct str, mrfstr_chr_t chr)
{
    if (MRFSTR_SIZE(str) != 1)
        return MRFSTR_FALSE;

    return *MRFSTR_DATA(str) == chr;
}
