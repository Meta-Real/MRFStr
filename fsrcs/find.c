/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

mrfstr_idx_t mrfstr_find_chr(mrfstr_ct str, mrfstr_chr_t chr)
{
    if (!str->size)
        return MRFSTR_INVIDX;

    return mrfstr_memchr2(str->data, chr, str->size);
}

mrfstr_idx_t mrfstr_n_find_chr(mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (!str->size)
        return MRFSTR_INVIDX;

    if (size > str->size)
        size = str->size;

    return mrfstr_memchr2(str->data, chr, size);
}
