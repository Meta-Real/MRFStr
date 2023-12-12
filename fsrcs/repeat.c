/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

mrfstr_res_enum_t mrfstr_repeat(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_size_t count)
{
    if (str->size == 1)
        return mrfstr_repeat_chr(res, *str->data, count);

    if (!(count && str->size))
    {
        res->size = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (count == 1)
    {
        if (res == str)
            return MRFSTR_RES_NOERROR;

        res->size = str->size;

        mrfstr_size_t size = str->size + 1;
        if (res->alloc < size)
        {
            if (res->alloc)
                mrstr_aligned_free(res->data);

            res->alloc = size;
            res->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
            if (!res->data)
                return MRFSTR_RES_MEM_ERROR;
        }

        mrfstr_memcpy(res->data, str->data, size);
        return MRFSTR_RES_NOERROR;
    }

    if (res == str)
    {
        mrfstr_size_t size = res->size * count;
        if (size / count != res->size)
            return MRFSTR_RES_OVERFLOW_ERROR;

        if (res->alloc <= size)
        {
            res->alloc = size + 1;
            ptr_t block = mrstr_aligned_realloc(res->data, res->alloc, MRFSTR_SIMD_SIZE);
            if (!block)
                return MRFSTR_RES_MEM_ERROR;

            res->data = block;
        }

        // We can do better
        while (res->size <= size - res->size)
        {
            mrfstr_memcpy(res->data + res->size, res->data, res->size);
            res->size <<= 1;
        }

        res->size = size - res->size;
        if (res->size)
            mrfstr_memcpy(res->data + res->size, res->data, res->size);

        res->data[size] = '\0';
        res->size = size;
        return MRFSTR_RES_NOERROR;
    }

    mrfstr_size_t size = str->size * count;
    if (size / count != str->size)
        return MRFSTR_RES_OVERFLOW_ERROR;

    res->size = str->size;
    if (res->alloc <= size)
    {
        if (res->alloc)
            mrstr_aligned_free(res->data);

        res->alloc = size + 1;
        res->data = mrstr_aligned_alloc(res->alloc, MRFSTR_SIMD_SIZE);
        if (!res->data)
            return MRFSTR_RES_MEM_ERROR;
    }

    // We can do better
    while (res->size <= size - res->size)
    {
        mrfstr_memcpy(res->data + res->size, res->data, res->size);
        res->size <<= 1;
    }

    res->size = size - res->size;
    if (res->size)
        mrfstr_memcpy(res->data + res->size, res->data, res->size);

    res->data[size] = '\0';
    res->size = size;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_repeat_chr(
    mrfstr_t res, mrfstr_chr_t chr,
    mrfstr_size_t count)
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
