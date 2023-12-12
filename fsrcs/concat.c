/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>

mrfstr_res_enum_t mrfstr_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2)
{
    if (res == str1)
    {
        if (!str2->size)
            return MRFSTR_RES_NOERROR;

        if (!res->size)
        {
            if (res->alloc < str2->size)
            {
                if (res->alloc)
                    mrstr_aligned_free(res->data);

                res->alloc = str2->size;
                res->data = mrstr_aligned_alloc(res->alloc, MRFSTR_SIMD_SIZE);
                if (!res->data)
                    return MRFSTR_RES_MEM_ERROR;
            }

            res->size = str2->size;
            mrfstr_memcpy(res->data, str2->data, res->size);
            return MRFSTR_RES_NOERROR;
        }

        mrfstr_size_t size = res->size + str2->size;
        if (size < res->size)
            return MRFSTR_RES_OVERFLOW_ERROR;

        if (res->alloc < size)
        {
            res->alloc = size;
            mrfstr_data_t tmp = mrstr_aligned_realloc(res->data, size, MRFSTR_SIMD_SIZE);
            if (!tmp)
                return MRFSTR_RES_MEM_ERROR;

            res->data = tmp;
        }

        mrfstr_memcpy(res->data + res->size, str2->data, str2->size);
        res->size = size;
        return MRFSTR_RES_NOERROR;
    }

    if (res == str2)
    {
        // For later support
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_size_t size = str1->size + str2->size;
    if (size < str1->size)
        return MRFSTR_RES_OVERFLOW_ERROR;

    res->size = size;
    if (res->alloc < size)
    {
        if (res->alloc)
            mrstr_aligned_free(res->data);

        res->alloc = size;
        res->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
        if (!res->data)
            return MRFSTR_RES_MEM_ERROR;
    }

    mrfstr_memcpy(res->data, str1->data, str1->size);
    mrfstr_memcpy(res->data + str1->size, str2->data, str2->size);
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_n_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2,
    mrfstr_size_t size)
{
    if (size > str2->size)
        size = str2->size;

    if (res == str1)
    {
        if (!size)
            return MRFSTR_RES_NOERROR;

        if (!res->size)
        {
            if (res->alloc < size)
            {
                if (res->alloc)
                    mrstr_aligned_free(res->data);

                res->alloc = size;
                res->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
                if (!res->data)
                    return MRFSTR_RES_MEM_ERROR;
            }

            mrfstr_memcpy(res->data, str2->data, size);
            res->size = size;
            return MRFSTR_RES_NOERROR;
        }

        mrfstr_size_t nsize = res->size + size;
        if (nsize < res->size)
            return MRFSTR_RES_OVERFLOW_ERROR;

        if (res->alloc < nsize)
        {
            res->alloc = nsize;
            mrfstr_data_t tmp = mrstr_aligned_realloc(res->data, nsize, MRFSTR_SIMD_SIZE);
            if (!tmp)
                return MRFSTR_RES_MEM_ERROR;

            res->data = tmp;
        }

        mrfstr_memcpy(res->data + res->size, str2->data, size);
        res->size = nsize;
        return MRFSTR_RES_NOERROR;
    }

    if (res == str2)
    {
        // For later support
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_size_t nsize = str1->size + size;
    if (nsize < str1->size)
        return MRFSTR_RES_OVERFLOW_ERROR;

    res->size = nsize;
    if (res->alloc < nsize)
    {
        if (res->alloc)
            mrstr_aligned_free(res->data);

        res->alloc = nsize;
        res->data = mrstr_aligned_alloc(nsize, MRFSTR_SIMD_SIZE);
        if (!res->data)
            return MRFSTR_RES_MEM_ERROR;
    }

    mrfstr_memcpy(res->data, str1->data, str1->size);
    mrfstr_memcpy(res->data + str1->size, str2->data, size);
    return MRFSTR_RES_NOERROR;
}
