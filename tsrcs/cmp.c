/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

mrtstr_res_enum_t mrtstr_equal(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_ct str2)
{
    if (str1->size != str2->size)
    {
        res->res = MRTSTR_FALSE;
        return MRTSTR_RES_NOERROR;
    }

    if (!str1->size)
    {
        res->res = MRTSTR_TRUE;
        return MRTSTR_RES_NOERROR;
    }

    mrtstr_memcmp(res, str1, str2, str1->size);
    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_equal_str(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2)
{
    mrtstr_size_t size = strlen(str2);
    if (str1->size != size)
    {
        res->res = MRTSTR_FALSE;
        return MRTSTR_RES_NOERROR;
    }

    if (!str1->size)
    {
        res->res = MRTSTR_TRUE;
        return MRTSTR_RES_NOERROR;
    }

    mrtstr_memcmp2(res, str1, str2, size);
    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_equal_nstr(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2, mrtstr_size_t size)
{
    if (str1->size != size)
    {
        res->res = MRTSTR_FALSE;
        return MRTSTR_RES_NOERROR;
    }

    if (!str1->size)
    {
        res->res = MRTSTR_TRUE;
        return MRTSTR_RES_NOERROR;
    }

    mrtstr_memcmp2(res, str1, str2, size);
    return MRTSTR_RES_NOERROR;
}
