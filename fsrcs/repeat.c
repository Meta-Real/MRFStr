/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>

mrfstr_res_enum_t mrfstr_repeat_chr(mrfstr_t res, mrfstr_chr_t chr, mrfstr_size_t count)
{
    if (!count)
    {
        res->size = 0;
        return MRFSTR_RES_NOERROR;
    }

    res->size = count;
    if (res->alloc <= count)
    {
        if (res->alloc)
            mrstr_aligned_free(res->data);

        res->alloc = count + 1;
        res->data = mrstr_aligned_alloc(res->alloc, MRFSTR_SIMD_SIZE);
        if (!res->data)
            return MRFSTR_RES_MEM_ERROR;
    }

    mrfstr_memset(res->data, chr, count);
    res->data[count] = '\0';
    return MRFSTR_RES_NOERROR;
}
