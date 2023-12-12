/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr.h>
#include <stdio.h>

mrfstr_data_t mrfstr_get_data(
    mrfstr_ct str)
{
    if (!str->size)
        return NULL;

    return str->data;
}

mrfstr_size_t mrfstr_get_size(
    mrfstr_ct str)
{
    return str->size;
}

mrfstr_chr_t mrfstr_get_chr(
    mrfstr_ct str, mrfstr_size_t idx)
{
    return str->data[idx];
}

void mrfstr_modify_chr(
    mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t idx)
{
    str->data[idx] = chr;
}

void mrfstr_print(
    mrfstr_ct str)
{
    if (!str->size)
        return;

    fputs(str->data, stdout);
}

void mrfstr_n_print(
    mrfstr_ct str, mrfstr_size_t size)
{
    if (size > str->size)
        size = str->size;

    if (!size)
        return;

    fwrite(str->data, 1, size, stdout);
}
