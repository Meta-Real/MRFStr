/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

mrfstr_res_enum_t mrfstr_set(mrfstr_t dst, mrfstr_ct src)
{
    if (!src->size)
    {
        dst->size = 0;
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_size_t size = src->size + 1;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_aligned_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_aligned_alloc(dst->alloc, MRFSTR_SIMD_SIZE);
        if (!dst->data)
            return MRFSTR_RES_MEM_ERROR;
    }

    mrfstr_memcpy(dst->data, src->data, size);
    dst->size = src->size;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_set_str(mrfstr_t dst, mrfstr_data_ct src)
{
    mrfstr_size_t size = strlen(src);
    if (!size)
    {
        dst->size = 0;
        return MRFSTR_RES_NOERROR;
    }

    dst->size = size++;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_aligned_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_aligned_alloc(dst->alloc, MRFSTR_SIMD_SIZE);
        if (!dst->data)
            return MRFSTR_RES_MEM_ERROR;
    }

    mrfstr_memcpy(dst->data, src, size);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_set_nstr(mrfstr_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    if (!size)
    {
        dst->size = 0;
        return MRFSTR_RES_NOERROR;
    }

    dst->size = size++;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_aligned_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_aligned_alloc(dst->alloc, MRFSTR_SIMD_SIZE);
        if (!dst->data)
            return MRFSTR_RES_MEM_ERROR;
    }

    mrfstr_memcpy(dst->data, src, size);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_set_chr(mrfstr_t dst, mrfstr_chr_t src)
{
    dst->size = 1;
    if (dst->alloc < 2)
    {
        if (dst->alloc)
            mrstr_aligned_free(dst->data);

        dst->alloc = 2;
        dst->data = mrstr_aligned_alloc(2, MRFSTR_SIMD_SIZE);
        if (!dst->data)
            return MRFSTR_RES_MEM_ERROR;
    }

    *dst->data = src;
    dst->data[1] = '\0';
    return MRFSTR_RES_NOERROR;
}
