/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

mrfstr_t mrfstr_init()
{
    mrfstr_t str = mrstr_alloc(sizeof(struct __MRFSTR_T));
    if (!str)
        return NULL;

    str->alloc = 0;
    str->size = 0;
    return str;
}

mrfstr_t mrfstr_init2(mrfstr_data_t data)
{
    mrfstr_t str = mrstr_alloc(sizeof(struct __MRFSTR_T));
    if (!str)
        return NULL;

    if (!data)
    {
        str->alloc = 0;
        str->size = 0;
        return str;
    }

    str->data = data;
    str->size = strlen(data);
    str->alloc = str->size + 1;
    return str;
}

mrfstr_t mrfstr_init3(mrfstr_data_t data, mrfstr_size_t size)
{
    mrfstr_t str = mrstr_alloc(sizeof(struct __MRFSTR_T));
    if (!str)
        return NULL;

    if (!data)
    {
        str->alloc = 0;
        str->size = 0;
        return str;
    }

    str->data = data;
    str->size = size;
    str->alloc = size + 1;
    return str;
}

mrfstr_res_enum_t mrfstr_alloc(mrfstr_t str,  mrfstr_size_t size)
{
    if (!size)
        return MRFSTR_RES_NOERROR;

    str->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
    if (!str->data)
        return MRFSTR_RES_MEM_ERROR;

    str->alloc = size;
    return MRFSTR_RES_NOERROR;
}

void mrfstr_free(mrfstr_t str)
{
    if (str->alloc)
        mrstr_aligned_free(str->data);
    mrstr_free(str);
}

void mrfstr_clear(mrfstr_t str)
{
    str->size = 0;
}

mrfstr_res_enum_t mrfstr_realloc(mrfstr_t str, mrfstr_size_t size)
{
    if (size == str->alloc)
        return MRFSTR_RES_NOERROR;

    if (!size)
    {
        if (str->alloc)
        {
            mrstr_aligned_free(str->data);
            str->alloc = 0;
        }

        str->size = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (!str->alloc)
    {
        str->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
        if (!str->data)
            return MRFSTR_RES_MEM_ERROR;

        str->alloc = size;
        return MRFSTR_RES_NOERROR;
    }

    str->alloc = size;
    if (!str->size)
    {
        mrstr_aligned_free(str->data);
        str->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
        return str->data ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
    }

    ptr_t block = mrstr_aligned_realloc(str->data, size, MRFSTR_SIMD_SIZE);
    if (!block)
        return MRFSTR_RES_MEM_ERROR;

    str->data = block;
    if (size <= str->size)
    {
        str->size = size - 1;
        str->data[str->size] = '\0';
    }

    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_clear_realloc(mrfstr_t str, mrfstr_size_t size)
{
    str->size = 0;
    if (size == str->alloc)
        return MRFSTR_RES_NOERROR;

    if (str->alloc)
        mrstr_aligned_free(str->data);

    str->alloc = size;
    if (!size)
        return MRFSTR_RES_NOERROR;

    str->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
    return str->data ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
}

mrfstr_res_enum_t mrfstr_expand(mrfstr_t str, mrfstr_size_t size)
{
    if (size == str->alloc)
        return MRFSTR_RES_NOERROR;

    if (!str->alloc)
    {
        str->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
        if (!str->data)
            return MRFSTR_RES_MEM_ERROR;

        str->alloc = size;
        return MRFSTR_RES_NOERROR;
    }

    str->alloc = size;
    if (!str->size)
    {
        mrstr_aligned_free(str->data);
        str->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
        return str->data ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
    }

    ptr_t block = mrstr_aligned_realloc(str->data, size, MRFSTR_SIMD_SIZE);
    if (!block)
        return MRFSTR_RES_MEM_ERROR;

    str->data = block;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_clear_expand(mrfstr_t str, mrfstr_size_t size)
{
    str->size = 0;
    if (size == str->size)
        return MRFSTR_RES_NOERROR;

    if (str->alloc)
        mrstr_aligned_free(str->data);

    str->alloc = size;
    str->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
    return str->data ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
}

mrfstr_res_enum_t mrfstr_expand_add(mrfstr_t str, mrfstr_size_t add)
{
    if (!add)
        return MRFSTR_RES_NOERROR;

    if (!str->alloc)
    {
        str->data = mrstr_aligned_alloc(add, MRFSTR_SIMD_SIZE);
        if (!str->data)
            return MRFSTR_RES_MEM_ERROR;

        str->alloc = add;
        return MRFSTR_RES_NOERROR;
    }

    str->alloc += add;
    if (!str->size)
    {
        mrstr_aligned_free(str->data);
        str->data = mrstr_aligned_alloc(str->alloc, MRFSTR_SIMD_SIZE);
        return str->data ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
    }

    ptr_t block = mrstr_aligned_realloc(str->data, str->alloc, MRFSTR_SIMD_SIZE);
    if (!block)
        return MRFSTR_RES_MEM_ERROR;

    str->data = block;
    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_clear_expand_add(mrfstr_t str, mrfstr_size_t add)
{
    str->size = 0;
    if (!add)
        return MRFSTR_RES_NOERROR;

    if (str->alloc)
        mrstr_aligned_free(str->data);

    str->alloc += add;
    str->data = mrstr_aligned_alloc(str->alloc, MRFSTR_SIMD_SIZE);
    return str->data ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
}

mrfstr_res_enum_t mrfstr_shrink(mrfstr_t str, mrfstr_size_t size)
{
    if (size == str->alloc)
        return MRFSTR_RES_NOERROR;

    if (!size)
    {
        mrstr_aligned_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return MRFSTR_RES_NOERROR;
    }

    ptr_t block = mrstr_aligned_realloc(str->data, size, MRFSTR_SIMD_SIZE);
    if (!block)
        return MRFSTR_RES_MEM_ERROR;

    str->data = block;
    str->alloc = size;

    if (size <= str->size)
    {
        str->size = size - 1;
        str->data[str->size] = '\0';
    }

    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_clear_shrink(mrfstr_t str, mrfstr_size_t size)
{
    str->size = 0;
    if (size == str->alloc)
        return MRFSTR_RES_NOERROR;

    if (str->alloc)
        mrstr_aligned_free(str->data);

    str->alloc = size;
    if (!size)
        return MRFSTR_RES_NOERROR;

    str->data = mrstr_aligned_alloc(size, MRFSTR_SIMD_SIZE);
    return str->data ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
}

mrfstr_res_enum_t mrfstr_shrink_sub(mrfstr_t str, mrfstr_size_t sub)
{
    if (!sub)
        return MRFSTR_RES_NOERROR;

    if (sub >= str->alloc)
    {
        mrstr_aligned_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return MRFSTR_RES_NOERROR;
    }

    str->alloc -= sub;
    ptr_t block = mrstr_aligned_realloc(str->data, str->alloc, MRFSTR_SIMD_SIZE);
    if (!block)
        return MRFSTR_RES_MEM_ERROR;

    if (str->alloc <= str->size)
    {
        str->size = str->alloc - 1;
        str->data[str->size] = '\0';
    }

    return MRFSTR_RES_NOERROR;
}

mrfstr_res_enum_t mrfstr_clear_shrink_sub(mrfstr_t str, mrfstr_size_t sub)
{
    str->size = 0;
    if (!sub)
        return MRFSTR_RES_NOERROR;

    if (str->alloc)
        mrstr_aligned_free(str->data);

    if (str->alloc <= sub)
    {
        str->alloc = 0;
        return MRFSTR_RES_NOERROR;
    }

    str->alloc -= sub;
    str->data = mrstr_aligned_alloc(str->alloc, MRFSTR_SIMD_SIZE);
    return str->data ? MRFSTR_RES_NOERROR : MRFSTR_RES_MEM_ERROR;
}

void mrfstr_swap(mrfstr_t str1, mrfstr_t str2)
{
    if (str1 == str2)
        return;

    mrfstr_data_t data = str1->data;
    str1->data = str2->data;
    str2->data = data;

    mrfstr_size_t size = str1->size;
    str1->size = str2->size;
    str2->size = size;

    size = str1->alloc;
    str1->alloc = str2->alloc;
    str2->alloc = size;
}
