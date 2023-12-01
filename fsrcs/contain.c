/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

mrfstr_bool_t mrfstr_contain_chr(mrfstr_ct str, mrfstr_chr_t chr)
{
    if (!str->size)
        return MRFSTR_FALSE;

    return mrfstr_memchr(str->data, chr, str->size);
}
