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

mrfstr_bool_t mrfstr_contain_chr(
    mrfstr_ct str, mrfstr_chr_t chr)
{
    if (!MRFSTR_SIZE(str))
        return MRFSTR_FALSE;

    return __mrfstr_contchr(MRFSTR_DATA(str), chr, MRFSTR_SIZE(str));
}

mrfstr_bool_t mrfstr_n_contain_chr(
    mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (size > MRFSTR_SIZE(str))
        size = MRFSTR_SIZE(str);

    if (!size)
        return MRFSTR_FALSE;

    return __mrfstr_contchr(MRFSTR_DATA(str), chr, size);
}

mrfstr_bool_t mrfstr_startswith(
    mrfstr_ct str, mrfstr_ct substr)
{
    if (MRFSTR_SIZE(substr) > MRFSTR_SIZE(str))
        return MRFSTR_FALSE;

    if (!MRFSTR_SIZE(substr) || str == substr)
        return MRFSTR_TRUE;

    return __mrfstr_equal(MRFSTR_DATA(str), MRFSTR_DATA(substr), MRFSTR_SIZE(substr));
}

mrfstr_bool_t mrfstr_startswith_str(
    mrfstr_ct str, mrfstr_data_ct substr)
{
    mrfstr_size_t size;

    size = mrfstr_strlen(substr);
    if (size > MRFSTR_SIZE(str))
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return __mrfstr_equal(MRFSTR_DATA(str), substr, size);
}

mrfstr_bool_t mrfstr_startswith_nstr(
    mrfstr_ct str, mrfstr_data_ct substr, mrfstr_size_t size)
{
    if (size > MRFSTR_SIZE(str))
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return __mrfstr_equal(MRFSTR_DATA(str), substr, size);
}

mrfstr_bool_t mrfstr_endswith(
    mrfstr_ct str, mrfstr_ct substr)
{
    if (MRFSTR_SIZE(substr) > MRFSTR_SIZE(str))
        return MRFSTR_FALSE;

    if (!MRFSTR_SIZE(substr) || str == substr)
        return MRFSTR_TRUE;

    return __mrfstr_equal(MRFSTR_DATA(str) + MRFSTR_SIZE(str) - MRFSTR_SIZE(substr),
        MRFSTR_DATA(substr), MRFSTR_SIZE(substr));
}

mrfstr_bool_t mrfstr_endswith_str(
    mrfstr_ct str, mrfstr_data_ct substr)
{
    mrfstr_size_t size;

    size = mrfstr_strlen(substr);
    if (size > MRFSTR_SIZE(str))
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return __mrfstr_equal(MRFSTR_DATA(str) + MRFSTR_SIZE(str) - size, substr, size);
}

mrfstr_bool_t mrfstr_endswith_nstr(
    mrfstr_ct str, mrfstr_data_ct substr, mrfstr_size_t size)
{
    if (size > MRFSTR_SIZE(str))
        return MRFSTR_FALSE;

    if (!size)
        return MRFSTR_TRUE;

    return __mrfstr_equal(MRFSTR_DATA(str) + MRFSTR_SIZE(str) - size, substr, size);
}
