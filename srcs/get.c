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

mrfstr_data_t mrfstr_get_str(
    mrfstr_data_t dst, mrfstr_ct src)
{
    if (!dst)
    {
        dst = malloc((MRFSTR_SIZE(src) + 1) * sizeof(mrfstr_chr_t));
        if (!dst)
            return NULL;
    }

    dst[MRFSTR_SIZE(src)] = '\0';
    if (!MRFSTR_SIZE(src))
        return dst;

    __mrfstr_copy(dst, MRFSTR_DATA(src), MRFSTR_SIZE(src));
    return dst;
}

mrfstr_data_t mrfstr_n_get_str(
    mrfstr_data_t dst, mrfstr_ct src, mrfstr_size_t size)
{
    if (size > MRFSTR_SIZE(src))
        size = MRFSTR_SIZE(src);

    if (!dst)
    {
        dst = malloc((size + 1) * sizeof(mrfstr_chr_t));
        if (!dst)
            return NULL;
    }

    dst[size] = '\0';
    if (!size)
        return dst;

    __mrfstr_copy(dst, MRFSTR_DATA(src), size);
    return dst;
}
