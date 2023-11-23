/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

void mrfstr_set(mrfstr_t dst, mrfstr_ct src)
{
    if (!src->size)
    {
        if (!dst->size)
            return;

        dst->size = 0;
        return;
    }

    mrfstr_size_t size = src->size + 1;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_alloc(dst->alloc);
    }

    mrfstr_memcpy(dst->data, src->data, size);
}

void mrfstr_set_str(mrfstr_t dst, mrfstr_data_ct src)
{
    mrfstr_size_t size = strlen(src);
    if (!size)
    {
        if (!dst->size)
            return;

        dst->size = 0;
        return;
    }

    dst->size = size++;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_alloc(dst->alloc);
    }

    mrfstr_memcpy(dst->data, src, size);
}

void mrfstr_set_nstr(mrfstr_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    if (!size)
    {
        if (!dst->size)
            return;

        dst->size = 0;
        return;
    }

    dst->size = size++;
    if (dst->alloc < size)
    {
        if (dst->alloc)
            mrstr_free(dst->data);

        dst->alloc = size;
        dst->data = mrstr_alloc(dst->alloc);
    }

    mrfstr_memcpy(dst->data, src, size);
}
