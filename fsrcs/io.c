/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr.h>
#include <stdio.h>

void mrfstr_print(
    mrfstr_ct str)
{
    if (!MRFSTR_SIZE(str))
        return;

    fwrite(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), MRFSTR_SIZE(str), stdout);
}

void mrfstr_n_print(
    mrfstr_ct str, mrfstr_size_t size)
{
    fwrite(MRFSTR_DATA(str), sizeof(mrfstr_chr_t), size, stdout);
}
