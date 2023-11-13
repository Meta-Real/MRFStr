/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>

mrtstr_t mrtstr_init()
{
    mrtstr_t str = __mrstr_alloc_una(sizeof(struct __MRTSTR_T));
    str->alloc = 0;
    str->size = 0;

    str->lock = __mrstr_alloc_una(6 * sizeof(mrtstr_lock_t));
    for (mrtstr_size_t i = 0; i < 6; i++)
        str->lock[i] = 0;

    str->mutex = PTHREAD_MUTEX_INITIALIZER;
    return str;
}

void mrtstr_clear(mrtstr_t str)
{
    for (; mrtstr_locked(str););

    if (str->alloc)
        __mrstr_free(str->data);

    __mrstr_free_una((void*)str->lock);
    __mrstr_free_una(str);
}

void mrtstr_realloc(mrtstr_t str, mrtstr_size_t size)
{
    if (size == str->alloc)
        return;

    for (; mrtstr_locked(str););

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

    str->data = __mrstr_realloc(str->data, size);
    str->alloc = size;

    if (size <= str->size)
    {
        str->size = size - 1;
        str->data[str->size] = '\0';
    }
}

void mrtstr_expand(mrtstr_t str, mrtstr_size_t size)
{
    if (size == str->alloc)
        return;

    for (; mrtstr_locked(str););

    if (!str->alloc)
    {
        str->data = __mrstr_alloc(size);
        str->alloc = size;
        return;
    }

    str->data = __mrstr_realloc(str->data, size);
    str->alloc = size;
}

void mrtstr_expand_add(mrtstr_t str, mrtstr_size_t add)
{
    if (!add)
        return;

    for (; mrtstr_locked(str););

    if (!str->alloc)
    {
        str->data = __mrstr_alloc(add);
        str->alloc = add;
        return;
    }

    str->alloc += add;
    str->data = __mrstr_realloc(str->data, str->alloc);
}

void mrtstr_shrink(mrtstr_t str, mrtstr_size_t size)
{
    if (size == str->alloc)
        return;

    for (; mrtstr_locked(str););

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

void mrtstr_shrink_sub(mrtstr_t str, mrtstr_size_t sub)
{
    if (!sub)
        return;

    for (; mrtstr_locked(str););

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
