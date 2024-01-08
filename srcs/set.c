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

void mrfstr_set(
    mrfstr_t dst, mrfstr_ct src)
{
    if (!MRFSTR_SIZE(src))
    {
        free(MRFSTR_DATA(dst));

        MRFSTR_DATA(dst) = NULL;
        MRFSTR_SIZE(dst) = 0;
        return;
    }

    __mrfstr_copy(MRFSTR_DATA(dst), MRFSTR_DATA(src), MRFSTR_SIZE(src));
    MRFSTR_SIZE(dst) = MRFSTR_SIZE(src);
}

void mrfstr_set_str(
    mrfstr_t dst, mrfstr_data_ct src)
{
    mrfstr_size_t size = mrfstr_strlen(src);
    if (!size)
    {
        free(MRFSTR_DATA(dst));

        MRFSTR_DATA(dst) = NULL;
        MRFSTR_SIZE(dst) = 0;
        return;
    }

    __mrfstr_copy(MRFSTR_DATA(dst), src, size);
    MRFSTR_SIZE(dst) = size;
}

void mrfstr_set_nstr(
    mrfstr_t dst, mrfstr_data_ct src,
    mrfstr_size_t size)
{
    if (!size)
    {
        free(MRFSTR_DATA(dst));

        MRFSTR_DATA(dst) = NULL;
        MRFSTR_SIZE(dst) = 0;
        return;
    }

    __mrfstr_copy(MRFSTR_DATA(dst), src, size);
    MRFSTR_SIZE(dst) = size;
}

void mrfstr_set_chr(
    mrfstr_t dst, mrfstr_chr_t src)
{
    *MRFSTR_DATA(dst) = src;
    MRFSTR_SIZE(dst) = 1;
}
