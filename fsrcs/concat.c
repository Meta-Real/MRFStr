/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>

#include <stdio.h>

mrfstr_res_enum_t mrfstr_concat(mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2)
{
    if (res == str1)
    {
        if (!str2->size)
            return MRFSTR_RES_NOERROR;

        if (!res->size)
        {
            if (res->alloc <= str2->size)
            {
                if (res->alloc)
                    mrstr_aligned_free(res->data);

                res->alloc = str2->size + 1;
                res->data = mrstr_aligned_alloc(res->alloc, MRFSTR_SIMD_SIZE);
                if (!res->data)
                    return MRFSTR_RES_MEM_ERROR;
            }

            res->size = str2->size;
            mrfstr_memcpy(res->data, str2->data, res->size);
            return MRFSTR_RES_NOERROR;
        }

        mrfstr_size_t size = res->size + str2->size;
        if (res->alloc <= size)
        {
            res->alloc = size + 1;
            mrfstr_data_t tmp = mrstr_aligned_realloc(res->data, res->alloc, MRFSTR_SIMD_SIZE);
            if (!tmp)
                return MRFSTR_RES_MEM_ERROR;

            res->data = tmp;
        }

        mrfstr_memcpy(res->data + res->size, str2->data, str2->size);
        res->data[size] = '\0';
        res->size = size;
        return MRFSTR_RES_NOERROR;
    }

    if (res == str2)
    {
        // For later support
        return MRFSTR_RES_NOERROR;
    }

    res->size = str1->size + str2->size;
    if (res->alloc <= res->size)
    {
        if (res->alloc)
            mrstr_aligned_free(res->data);

        res->alloc = res->size + 1;
        res->data = mrstr_aligned_alloc(res->alloc, MRFSTR_SIMD_SIZE);
        if (!res->data)
            return MRFSTR_RES_MEM_ERROR;
    }

    mrfstr_memcpy(res->data, str1->data, str1->size);
    mrfstr_memcpy(res->data + str1->size, str2->data, str2->size + 1);
    return MRFSTR_RES_NOERROR;
}
