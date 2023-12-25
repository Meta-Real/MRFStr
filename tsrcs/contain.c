/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

mrtstr_res_enum_t mrtstr_contain_chr(
    mrtstr_bres_t *res,
    mrtstr_ct str, mrtstr_chr_t chr)
{
    if (!str->size)
    {
        res->res = MRTSTR_FALSE;
        return MRTSTR_RES_NOERROR;
    }

    mrtstr_memchr(res, str, chr, str->size);
    return MRTSTR_RES_NOERROR;
}
