/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

mrfstr_bool_t mrfstr_contain_chr(
    mrfstr_ct str, mrfstr_chr_t chr)
{
    if (!MRFSTR_SIZE(str))
        return MRFSTR_FALSE;

    return __mrfstr_contain_chr(MRFSTR_DATA(str), chr, MRFSTR_SIZE(str));
}

mrfstr_bool_t mrfstr_n_contain_chr(
    mrfstr_ct str, mrfstr_chr_t chr,
    mrfstr_size_t size)
{
    if (size > MRFSTR_SIZE(str))
        size = MRFSTR_SIZE(str);

    if (!size)
        return MRFSTR_FALSE;

    return __mrfstr_contain_chr(MRFSTR_DATA(str), chr, size);
}
