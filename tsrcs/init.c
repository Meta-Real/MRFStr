/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

mrtstr_t mrtstr_init()
{
    mrtstr_t str = mrstr_alloc(sizeof(struct __MRTSTR_T));
    if (!str)
        return NULL;

    str->size = 0;
    str->alloc = 0;
    str->lock = mrstr_calloc(MRTSTR_THREAD_COUNT, sizeof(mrtstr_lock_t));
    if (!str->lock)
    {
        mrstr_free(str);
        return NULL;
    }

    if (pthread_mutex_init(&str->mutex, NULL))
    {
        mrstr_free((void*)str->lock);
        mrstr_free(str);
        return NULL;
    }

    return str;
}

mrtstr_t mrtstr_init2(
    mrtstr_data_t data)
{
    mrtstr_t str = mrstr_alloc(sizeof(struct __MRTSTR_T));
    if (!str)
        return NULL;

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

    str->lock = mrstr_calloc(MRTSTR_THREAD_COUNT, sizeof(mrtstr_lock_t));
    if (!str->lock)
    {
        mrstr_free(str);
        return NULL;
    }

    if (pthread_mutex_init(&str->mutex, NULL))
    {
        mrstr_free((void*)str->lock);
        mrstr_free(str);
        return NULL;
    }

    return str;
}

mrtstr_t mrtstr_init3(
    mrtstr_data_t data, mrtstr_size_t size)
{
    mrtstr_t str = mrstr_alloc(sizeof(struct __MRTSTR_T));
    if (!str)
        return NULL;

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

    str->lock = mrstr_calloc(MRTSTR_THREAD_COUNT, sizeof(mrtstr_lock_t));
    if (!str->lock)
    {
        mrstr_free(str);
        return NULL;
    }

    if (pthread_mutex_init(&str->mutex, NULL))
    {
        mrstr_free((void*)str->lock);
        mrstr_free(str);
        return NULL;
    }

    return str;
}

mrtstr_res_enum_t mrtstr_alloc(
    mrtstr_t str, mrtstr_size_t size)
{
    if (!size)
        return MRTSTR_RES_NOERROR;

    str->data = mrstr_aligned_alloc(size, MRTSTR_SIMD_SIZE);
    if (!str->data)
        return MRTSTR_RES_MEM_ERROR;

    str->alloc = size;
    return MRTSTR_RES_NOERROR;
}

void mrtstr_free(
    mrtstr_t str)
{
    for (; mrtstr_locked(str););

    if (str->alloc)
        mrstr_aligned_free(str->data);

    mrstr_free((void*)str->lock);

    pthread_mutex_lock(&str->mutex);
    pthread_mutex_unlock(&str->mutex);
    pthread_mutex_destroy(&str->mutex);

    mrstr_free(str);
}

void mrtstr_clear(
    mrtstr_t str)
{
    for (; mrtstr_locked(str););
    str->size = 0;
}

mrtstr_res_enum_t mrtstr_realloc(
    mrtstr_t str, mrtstr_size_t size)
{
    if (size == str->alloc)
        return MRTSTR_RES_NOERROR;

    for (; mrtstr_locked(str););

    if (!size)
    {
        if (str->alloc)
            mrstr_aligned_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return MRTSTR_RES_NOERROR;
    }

    if (!str->alloc)
    {
        str->data = mrstr_aligned_alloc(size, MRTSTR_SIMD_SIZE);
        if (!str->data)
            return MRTSTR_RES_MEM_ERROR;

        str->alloc = size;
        return MRTSTR_RES_NOERROR;
    }

    str->alloc = size;
    if (!str->size)
    {
        mrstr_aligned_free(str->data);
        str->data = mrstr_aligned_alloc(size, MRTSTR_SIMD_SIZE);
        return str->data ? MRTSTR_RES_NOERROR : MRTSTR_RES_MEM_ERROR;
    }

    mrtstr_data_t block = mrstr_aligned_realloc(str->data, size, MRTSTR_SIMD_SIZE);
    if (!block)
        return MRTSTR_RES_MEM_ERROR;

    str->data = block;
    if (size <= str->size)
        str->size = size - 1;

    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_clear_realloc(
    mrtstr_t str, mrtstr_size_t size)
{
    for (; mrtstr_locked(str););

    str->size = 0;
    if (size == str->alloc)
        return MRTSTR_RES_NOERROR;

    if (str->alloc)
        mrstr_aligned_free(str->data);

    str->alloc = size;
    if (!size)
        return MRTSTR_RES_NOERROR;

    str->data = mrstr_aligned_alloc(size, MRTSTR_SIMD_SIZE);
    return str->data ? MRTSTR_RES_NOERROR : MRTSTR_RES_MEM_ERROR;
}

mrtstr_res_enum_t mrtstr_expand(
    mrtstr_t str, mrtstr_size_t size)
{
    if (size == str->alloc)
        return MRTSTR_RES_NOERROR;

    for (; mrtstr_locked(str););

    if (!str->alloc)
    {
        str->data = mrstr_aligned_alloc(size, MRTSTR_SIMD_SIZE);
        if (!str->data)
            return MRTSTR_RES_MEM_ERROR;

        str->alloc = size;
        return MRTSTR_RES_NOERROR;
    }

    str->alloc = size;
    if (!str->size)
    {
        mrstr_aligned_free(str->data);
        str->data = mrstr_aligned_alloc(size, MRTSTR_SIMD_SIZE);
        return str->data ? MRTSTR_RES_NOERROR : MRTSTR_RES_MEM_ERROR;
    }

    mrtstr_data_t block = mrstr_aligned_realloc(str->data, size, MRTSTR_SIMD_SIZE);
    if (!block)
        return MRTSTR_RES_MEM_ERROR;

    str->data = block;
    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_clear_expand(
    mrtstr_t str, mrtstr_size_t size)
{
    for (; mrtstr_locked(str););

    str->size = 0;
    if (size == str->alloc)
        return MRTSTR_RES_NOERROR;

    if (str->alloc)
        mrstr_aligned_free(str->data);

    str->alloc = size;
    str->data = mrstr_aligned_alloc(size, MRTSTR_SIMD_SIZE);
    return str->data ? MRTSTR_RES_NOERROR : MRTSTR_RES_MEM_ERROR;
}

mrtstr_res_enum_t mrtstr_expand_add(
    mrtstr_t str, mrtstr_size_t add)
{
    if (!add)
        return MRTSTR_RES_NOERROR;

    for (; mrtstr_locked(str););

    if (!str->alloc)
    {
        str->data = mrstr_aligned_alloc(add, MRTSTR_SIMD_SIZE);
        if (!str->data)
            return MRTSTR_RES_MEM_ERROR;

        str->alloc = add;
        return MRTSTR_RES_NOERROR;
    }

    str->alloc += add;
    if (!str->size)
    {
        mrstr_aligned_free(str->data);
        str->data = mrstr_aligned_alloc(str->alloc, MRTSTR_SIMD_SIZE);
        return str->data ? MRTSTR_RES_NOERROR : MRTSTR_RES_MEM_ERROR;
    }

    mrtstr_data_t block = mrstr_realloc(str->data, str->alloc);
    if (!block)
        return MRTSTR_RES_MEM_ERROR;

    str->data = block;
    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_clear_expand_add(
    mrtstr_t str, mrtstr_size_t add)
{
    for (; mrtstr_locked(str););

    str->size = 0;
    if (!add)
        return MRTSTR_RES_NOERROR;

    if (str->alloc)
        mrstr_aligned_free(str->data);

    str->alloc += add;
    str->data = mrstr_aligned_alloc(str->alloc, MRTSTR_SIMD_SIZE);
    return str->data ? MRTSTR_RES_NOERROR : MRTSTR_RES_MEM_ERROR;
}

mrtstr_res_enum_t mrtstr_shrink(
    mrtstr_t str, mrtstr_size_t size)
{
    if (size == str->alloc)
        return MRTSTR_RES_NOERROR;

    for (; mrtstr_locked(str););

    if (!size)
    {
        mrstr_aligned_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return MRTSTR_RES_NOERROR;
    }

    mrtstr_data_t block = mrstr_aligned_realloc(str->data, size, MRTSTR_SIMD_SIZE);
    if (!block)
        return MRTSTR_RES_MEM_ERROR;

    str->data = block;
    str->alloc = size;

    if (size <= str->size)
        str->size = size - 1;

    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_clear_shrink(
    mrtstr_t str, mrtstr_size_t size)
{
    for (; mrtstr_locked(str););

    str->size = 0;
    if (size == str->alloc)
        return MRTSTR_RES_NOERROR;

    if (str->alloc)
        mrstr_aligned_free(str->data);

    str->alloc = size;
    if (!size)
        return MRTSTR_RES_NOERROR;

    str->data = mrstr_aligned_alloc(size, MRTSTR_SIMD_SIZE);
    return str->data ? MRTSTR_RES_NOERROR : MRTSTR_RES_MEM_ERROR;
}

mrtstr_res_enum_t mrtstr_shrink_sub(
    mrtstr_t str, mrtstr_size_t sub)
{
    if (!sub)
        return MRTSTR_RES_NOERROR;

    for (; mrtstr_locked(str););

    if (sub >= str->alloc)
    {
        mrstr_aligned_free(str->data);

        str->alloc = 0;
        str->size = 0;
        return MRTSTR_RES_NOERROR;
    }

    str->alloc -= sub;
    mrtstr_data_t block = mrstr_aligned_realloc(str->data, str->alloc, MRTSTR_SIMD_SIZE);
    if (!block)
        return MRTSTR_RES_MEM_ERROR;

    str->data = block;
    if (str->alloc <= str->size)
        str->size = str->alloc - 1;

    return MRTSTR_RES_NOERROR;
}

mrtstr_res_enum_t mrtstr_clear_shrink_sub(
    mrtstr_t str, mrtstr_size_t sub)
{
    for (; mrtstr_locked(str););

    str->size = 0;
    if (!sub)
        return MRTSTR_RES_NOERROR;

    if (str->alloc)
        mrstr_aligned_free(str->data);

    if (str->alloc <= sub)
    {
        str->alloc = 0;
        return MRTSTR_RES_NOERROR;
    }

    str->alloc -= sub;
    mrtstr_data_t block = mrstr_aligned_alloc(str->alloc, MRTSTR_SIMD_SIZE);
    if (!block)
        return MRTSTR_RES_MEM_ERROR;

    str->data = block;
    return MRTSTR_RES_NOERROR;
}

void mrtstr_swap(
    mrtstr_ct str1, mrtstr_ct str2)
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
