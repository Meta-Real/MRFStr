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
    if (dst == src)
        return;

    MRFSTR_SIZE(dst) = MRFSTR_SIZE(src);
    if (MRFSTR_SIZE(dst))
        __mrfstr_copy(MRFSTR_DATA(dst), MRFSTR_DATA(src), MRFSTR_SIZE(dst));
}

void mrfstr_set_str(
    mrfstr_t dst, mrfstr_data_ct src)
{
    MRFSTR_SIZE(dst) = mrfstr_strlen(src);
    if (MRFSTR_SIZE(dst))
        __mrfstr_copy(MRFSTR_DATA(dst), src, MRFSTR_SIZE(dst));
}

void mrfstr_set_nstr(
    mrfstr_t dst, mrfstr_data_ct src,
    mrfstr_size_t size)
{
    MRFSTR_SIZE(dst) = size;
    if (size)
        __mrfstr_copy(MRFSTR_DATA(dst), src, size);
}

void mrfstr_set_chr(
    mrfstr_t dst, mrfstr_chr_t src)
{
    *MRFSTR_DATA(dst) = src;
    MRFSTR_SIZE(dst) = 1;
}
