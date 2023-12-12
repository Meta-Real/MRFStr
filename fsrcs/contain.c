/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

mrfstr_bool_t mrfstr_contain_chr(
    mrfstr_ct str, mrfstr_chr_t chr)
{
    if (!str->size)
        return MRFSTR_FALSE;

    return mrfstr_memchr(str->data, chr, str->size);
}

mrfstr_bool_t mrfstr_n_contain_chr(
    mrfstr_ct str, mrfstr_chr_t chr,
    mrfstr_size_t size)
{
    if (!str->size)
        return MRFSTR_FALSE;

    if (size > str->size)
        size = str->size;

    return mrfstr_memchr(str->data, chr, size);
}
