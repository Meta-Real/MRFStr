/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

mrtstr_res_enum_t mrtstr_set(mrtstr_t dst, mrtstr_ct src)
{
    for (; mrtstr_locked(dst););

    if (!src->size)
    {
        if (!dst->size)
            return MRTSTR_RES_NOERROR;

        dst->size = 0;
        return MRTSTR_RES_NOERROR;
    }

    mrtstr_size_t size = src->size + 1;
    if (dst->alloc < size)
    {
        if (dst->alloc && dst->data != src->data)
            mrstr_aligned_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_aligned_alloc(dst->alloc, MRTSTR_SIMD_SIZE);
        if (!dst->data)
            return MRTSTR_RES_MEM_ERROR;
    }

    mrtstr_memcpy(dst, src, size);
    dst->size = src->size;
    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_set_str(mrtstr_t dst, mrtstr_data_ct src)
{
    for (; mrtstr_locked(dst););

    mrtstr_size_t size = strlen(src);
    if (!size)
    {
        if (!dst->size)
            return MRTSTR_RES_NOERROR;

        dst->size = 0;
        return MRTSTR_RES_NOERROR;
    }

    dst->size = size++;
    if (dst->alloc < size)
    {
        if (dst->alloc && dst->data != src)
            mrstr_aligned_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_aligned_alloc(dst->alloc, MRTSTR_SIMD_SIZE);
        if (!dst->data)
            return MRTSTR_RES_MEM_ERROR;
    }

    mrtstr_memcpy2(dst, src, size);
    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_set_nstr(mrtstr_t dst, mrtstr_data_ct src, mrtstr_size_t size)
{
    for (; mrtstr_locked(dst););

    if (!size)
    {
        if (!dst->size)
            return MRTSTR_RES_NOERROR;

        dst->size = 0;
        return MRTSTR_RES_NOERROR;
    }

    dst->size = size++;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_aligned_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_aligned_alloc(dst->alloc, MRTSTR_SIMD_SIZE);
        if (!dst->data)
            return MRTSTR_RES_MEM_ERROR;
    }

    mrtstr_memcpy2(dst, src, size);
    return MRTSTR_RES_NOERROR;
}
