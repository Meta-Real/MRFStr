/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

mrtstr_res_enum_t mrtstr_set(
    mrtstr_t dst, mrtstr_ct src)
{
    for (; mrtstr_locked(dst););

    if (!src->size)
    {
        if (!dst->size)
            return MRTSTR_RES_NOERROR;

        dst->size = 0;
        return MRTSTR_RES_NOERROR;
    }

    if (dst->alloc < src->size)
    {
        if (dst->alloc)
            mrstr_aligned_free(dst->data);

        dst->data = mrstr_aligned_alloc(src->size, MRTSTR_SIMD_SIZE);
        dst->alloc = src->size;
        if (!dst->data)
            return MRTSTR_RES_MEM_ERROR;
    }

    mrtstr_memcpy(dst, src, src->size);
    dst->size = src->size;
    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_set_str(
    mrtstr_t dst, mrtstr_data_ct src)
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

    dst->size = size;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_aligned_free(dst->data);

        dst->data = mrstr_aligned_alloc(size, MRTSTR_SIMD_SIZE);
        dst->alloc = size;
        if (!dst->data)
            return MRTSTR_RES_MEM_ERROR;
    }

    mrtstr_memcpy2(dst, src, size);
    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_set_nstr(
    mrtstr_t dst, mrtstr_data_ct src, mrtstr_size_t size)
{
    for (; mrtstr_locked(dst););

    if (!size)
    {
        if (!dst->size)
            return MRTSTR_RES_NOERROR;

        dst->size = 0;
        return MRTSTR_RES_NOERROR;
    }

    dst->size = size;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_aligned_free(dst->data);

        dst->data = mrstr_aligned_alloc(size, MRTSTR_SIMD_SIZE);
        dst->alloc = size;
        if (!dst->data)
            return MRTSTR_RES_MEM_ERROR;
    }

    mrtstr_memcpy2(dst, src, size);
    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_set_chr(
    mrtstr_t dst, mrtstr_chr_t src)
{
    for (; mrtstr_locked(dst););

    dst->size = 1;
    if (!dst->alloc)
    {
        if (dst->alloc)
            mrstr_aligned_free(dst->data);

        dst->data = mrstr_aligned_alloc(1, MRTSTR_SIMD_SIZE);
        dst->alloc = 1;
        if (!dst->data)
            return MRTSTR_RES_MEM_ERROR;
    }

    *dst->data = src;
    return MRTSTR_RES_NOERROR;
}
