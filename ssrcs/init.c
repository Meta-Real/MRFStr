/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr.h>
#include <alloc.h>

mrfstr_t mrfstr_init()
{
    mrfstr_t str = __mrstr_alloc_una(sizeof(struct __MRFSTR_T));
    str->alloc = 0;
    str->size = 0;
    return str;
}

void mrfstr_clear(mrfstr_t str)
{
    if (str->alloc)
        __mrstr_free(str->data);
    __mrstr_free(str);
}

void mrfstr_realloc(mrfstr_t str, mrfstr_size_t size)
{
    if (size == str->alloc)
        return;

    if (!size)
    {
        if (str->alloc)
            __mrstr_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return;
    }

    if (!str->alloc)
    {
        str->data = __mrstr_alloc(size);
        str->alloc = size;
        return;
    }

    if (size > str->alloc)
    {
        str->data = __mrstr_realloc(str->data, size);
        str->alloc = size;
        return;
    }

    str->data = __mrstr_realloc(str->data, size);
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
        str->data = __mrstr_alloc(size);
        str->alloc = size;
        return;
    }

    str->data = __mrstr_realloc(str->data, size);
    str->alloc = size;
}

void mrfstr_expand_add(mrfstr_t str, mrfstr_size_t add)
{
    if (!add)
        return;

    if (!str->alloc)
    {
        str->data = __mrstr_alloc(add);
        str->alloc = add;
        return;
    }

    str->alloc += add;
    str->data = __mrstr_realloc(str->data, str->alloc);
}

void mrfstr_shrink(mrfstr_t str, mrfstr_size_t size)
{
    if (size == str->alloc)
        return;

    if (!size)
    {
        __mrstr_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return;
    }

    str->data = __mrstr_realloc(str->data, size);
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
        __mrstr_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return;
    }

    str->alloc -= sub;
    str->data = __mrstr_realloc(str->data, str->alloc);

    if (str->alloc <= str->size)
    {
        str->size = str->alloc - 1;
        str->data[str->size] = '\0';
    }
}
