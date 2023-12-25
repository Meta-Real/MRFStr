/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

mrtstr_res_enum_t mrtstr_repeat_chr(
    mrtstr_t res, mrtstr_chr_t chr,
    mrtstr_size_t count)
{
    for (; mrtstr_locked(res););

    if (!count)
    {
        if (!res->size)
            return MRTSTR_RES_NOERROR;

        res->size = 0;
        return MRTSTR_RES_NOERROR;
    }

    res->size = count;
    if (res->alloc < count)
    {
        if (res->alloc)
            mrstr_aligned_free(res->data);

        res->data = mrstr_aligned_alloc(count, MRTSTR_SIMD_SIZE);
        res->alloc = count;
        if (!res->data)
            return MRTSTR_RES_MEM_ERROR;
    }

    mrtstr_memset(res, chr, count);
    return MRTSTR_RES_NOERROR;
}
