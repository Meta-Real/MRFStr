/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
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