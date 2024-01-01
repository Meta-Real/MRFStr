/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

mrfstr_idx_t mrfstr_find_chr(
    mrfstr_ct str, mrfstr_chr_t chr)
{
    if (!MRFSTR_SIZE(str))
        return MRFSTR_INVIDX;

    return mrfstr_memchr2(MRFSTR_DATA(str), chr, MRFSTR_SIZE(str));
}

mrfstr_idx_t mrfstr_n_find_chr(
    mrfstr_ct str, mrfstr_chr_t chr,
    mrfstr_size_t size)
{
    if (size > MRFSTR_SIZE(str))
        size = MRFSTR_SIZE(str);

    if (!MRFSTR_SIZE(str))
        return MRFSTR_INVIDX;

    return mrfstr_memchr2(MRFSTR_DATA(str), chr, size);
}
