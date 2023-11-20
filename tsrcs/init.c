/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

mrtstr_t mrtstr_init()
{
    mrtstr_t str = __mrstr_alloc_una(sizeof(struct __MRTSTR_T));
    str->size = 0;
    str->alloc = 0;
    str->lock = __mrstr_calloc(MRTSTR_THREAD_COUNT, sizeof(mrtstr_lock_t));
    pthread_mutex_init(&str->mutex, NULL);
    return str;
}

mrtstr_t mrtstr_init2(mrtstr_data_t data)
{
    mrtstr_t str = __mrstr_alloc_una(sizeof(struct __MRTSTR_T));

    if (data)
    {
        str->data = data;
        str->size = strlen(data);
        str->alloc = str->size + 1;
    }
    else
    {
        str->size = 0;
        str->alloc = 0;
    }

    str->lock = __mrstr_calloc(MRTSTR_THREAD_COUNT, sizeof(mrtstr_lock_t));
    pthread_mutex_init(&str->mutex, NULL);
    return str;
}

mrtstr_t mrtstr_init3(mrtstr_data_t data, mrtstr_size_t size)
{
    mrtstr_t str = __mrstr_alloc_una(sizeof(struct __MRTSTR_T));

    if (data)
    {
        str->data = data;
        str->size = size;
        str->alloc = size + 1;
    }
    else
    {
        str->size = 0;
        str->alloc = 0;
    }

    str->lock = __mrstr_calloc(MRTSTR_THREAD_COUNT, sizeof(mrtstr_lock_t));
    pthread_mutex_init(&str->mutex, NULL);
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

void mrtstr_swap(mrtstr_ct str1, mrtstr_ct str2)
{
    if (str1 == str2)
        return;

    mrtstr_data_t data = str1->data;
    str1->data = str2->data;
    str2->data = data;

    mrtstr_size_t size = str1->size;
    str1->size = str2->size;
    str2->size = size;

    mrtstr_lock_t *lock = str1->lock;
    str1->lock = str2->lock;
    str2->lock = lock;

    mrtstr_bool_t forced = str1->forced;
    str1->forced = str2->forced;
    str2->forced = forced;

    for (; mrtstr_locked(str1););
    for (; mrtstr_locked(str2););

    pthread_mutex_t mutex = str1->mutex;
    str1->mutex = str2->mutex;
    str2->mutex = mutex;
}
