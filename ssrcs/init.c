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
    str->alloc = 0;
    str->size = 0;
    return str;
}

mrfstr_t mrfstr_init2(mrfstr_data_t data)
{
    mrfstr_t str = mrstr_alloc(sizeof(struct __MRFSTR_T));

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

void mrfstr_clear(mrfstr_t str)
{
    if (str->alloc)
        mrstr_mm_free(str->data);
    mrstr_free(str);
}

void mrfstr_realloc(mrfstr_t str, mrfstr_size_t size)
{
    if (size == str->alloc)
        return;

    if (!size)
    {
        if (str->alloc)
            mrstr_mm_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return;
    }

    if (!str->alloc)
    {
        str->data = mrstr_mm_alloc(size, MRFSTR_SIMD_OFF);
        str->alloc = size;
        return;
    }

    if (size > str->alloc)
    {
        str->data = mrstr_mm_realloc(str->data, size, MRFSTR_SIMD_OFF);
        str->alloc = size;
        return;
    }

    str->data = mrstr_mm_realloc(str->data, size, MRFSTR_SIMD_OFF);
    str->alloc = size;

    if (size <= str->size)
    {
        str->size = size - 1;
        str->data[str->size] = '\0';
    }
}

void mrfstr_expand(mrfstr_t str, mrfstr_size_t size)
{
    if (size == str->alloc)
        return;

    if (!str->alloc)
    {
        str->data = mrstr_mm_alloc(size, MRFSTR_SIMD_OFF);
        str->alloc = size;
        return;
    }

    str->data = mrstr_mm_realloc(str->data, size, MRFSTR_SIMD_OFF);
    str->alloc = size;
}

void mrfstr_expand_add(mrfstr_t str, mrfstr_size_t add)
{
    if (!add)
        return;

    if (!str->alloc)
    {
        str->data = mrstr_mm_alloc(add, MRFSTR_SIMD_OFF);
        str->alloc = add;
        return;
    }

    str->alloc += add;
    str->data = mrstr_mm_realloc(str->data, str->alloc, MRFSTR_SIMD_OFF);
}

void mrfstr_shrink(mrfstr_t str, mrfstr_size_t size)
{
    if (size == str->alloc)
        return;

    if (!size)
    {
        mrstr_mm_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return;
    }

    str->data = mrstr_mm_realloc(str->data, size, MRFSTR_SIMD_OFF);
    str->alloc = size;

    if (size <= str->size)
    {
        str->size = size - 1;
        str->data[str->size] = '\0';
    }
}

void mrfstr_shrink_sub(mrfstr_t str, mrfstr_size_t sub)
{
    if (!sub)
        return;

    if (sub >= str->alloc)
    {
        mrstr_mm_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return;
    }

    str->alloc -= sub;
    str->data = mrstr_mm_realloc(str->data, str->alloc, MRFSTR_SIMD_OFF);

    if (str->alloc <= str->size)
    {
        str->size = str->alloc - 1;
        str->data[str->size] = '\0';
    }
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
