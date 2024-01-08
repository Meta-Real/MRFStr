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

#include <mrfstr.h>
#include <stdio.h>

void mrfstr_print(
    mrfstr_ct str)
{
    if (!MRFSTR_SIZE(str))
        return;

    fwrite(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), (size_t)MRFSTR_SIZE(str), stdout);
}

void mrfstr_n_print(
    mrfstr_ct str, mrfstr_size_t size)
{
    if (size > MRFSTR_SIZE(str))
        size = MRFSTR_SIZE(str);

    if (!size)
        return;

    fwrite(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), (size_t)size, stdout);
}
