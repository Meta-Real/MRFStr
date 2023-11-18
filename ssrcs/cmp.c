/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#if MRFSTR_THREADING
#include <pthread.h>

struct __MRFSTR_EQUAL_T
{
    mrfstr_simd_block_t *str1;
    mrfstr_simd_block_t *str2;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_EQUAL_T *mrfstr_equal_t;

void *mrfstr_equal_threaded(void *args);
#endif

mrfstr_bool_t mrfstr_equal(mrfstr_ct str1, mrfstr_ct str2)
{
    if (str1->size != str2->size)
        return MRFSTR_FALSE;

    if (!str1->size || str1->data == str2->data)
        return MRFSTR_TRUE;

    mrfstr_simd_block_t *s1block = (mrfstr_simd_block_t*)str1->data;
    mrfstr_simd_block_t *s2block = (mrfstr_simd_block_t*)str2->data;

    mrfstr_size_t size = str1->size;

#if MRFSTR_THREADING
    if (size <= MRFSTR_THREAD_LIMIT)
    {
#endif
        mrfstr_size_t rem = size & MRFSTR_SIMD_CHAR_MASK;
        size >>= MRFSTR_SIMD_CHAR_SHIFT;

        for (; size; s1block++, s2block++, size--)
            if (mrfstr_simd_cmpneq_func(*s1block, *s2block))
                return MRFSTR_FALSE;

        return !memcmp((mrfstr_data_ct)s1block, (mrfstr_data_ct)s2block, rem);
#if MRFSTR_THREADING
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    volatile mrfstr_bool_t res = MRFSTR_TRUE;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_equal_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRFSTR_EQUAL_T));
        data->str1 = s1block;
        data->str2 = s2block;
        data->size = size;
        data->res = &res;

        s1block += size;
        s2block += size;

        pthread_create(threads + i, NULL, mrfstr_equal_threaded, data);
    }

    mrfstr_bool_t rres = !memcmp((mrfstr_data_t)s2block, (mrfstr_data_t)s1block, rem);
    if (!rres)
        res = MRFSTR_FALSE;

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    return res;
#endif
}

mrfstr_bool_t mrfstr_equal_str(mrfstr_ct str1, mrfstr_data_ct str2)
{
    mrfstr_size_t size = strlen(str2);
    if (size != str1->size)
        return MRFSTR_FALSE;

    if (!str1->size || str1->data == str2)
        return MRFSTR_TRUE;

    mrfstr_simd_block_t *s1block = (mrfstr_simd_block_t*)str1->data;
    mrfstr_simd_block_t *s2block = (mrfstr_simd_block_t*)str2;

#if MRFSTR_THREADING
    if (size <= MRFSTR_THREAD_LIMIT)
    {
#endif
        mrfstr_size_t rem = size & MRFSTR_SIMD_CHAR_MASK;
        size >>= MRFSTR_SIMD_CHAR_SHIFT;

        for (; size; s1block++, s2block++, size--)
            if (mrfstr_simd_cmpneq_func(*s1block, *s2block))
                return MRFSTR_FALSE;

        return !memcmp((mrfstr_data_ct)s1block, (mrfstr_data_ct)s2block, rem);
#if MRFSTR_THREADING
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    mrfstr_bool_t res = MRFSTR_TRUE;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_equal_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRFSTR_EQUAL_T));
        data->str1 = s1block;
        data->str2 = s2block;
        data->size = size;
        data->res = &res;

        s1block += size;
        s2block += size;

        pthread_create(threads + i, NULL, mrfstr_equal_threaded, data);
    }

    mrfstr_bool_t rres = !memcmp((mrfstr_data_t)s2block, (mrfstr_data_t)s1block, rem);
    if (!rres)
        res = MRFSTR_FALSE;

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    return res;
#endif
}

mrfstr_bool_t mrfstr_equal_nstr(mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
    if (str1->size != size)
        return MRFSTR_FALSE;

    if (!size || str1->data == str2)
        return MRFSTR_TRUE;

    mrfstr_simd_block_t *s1block = (mrfstr_simd_block_t*)str1->data;
    mrfstr_simd_block_t *s2block = (mrfstr_simd_block_t*)str2;

#if MRFSTR_THREADING
    if (size <= MRFSTR_THREAD_LIMIT)
    {
#endif
        mrfstr_size_t rem = size & MRFSTR_SIMD_CHAR_MASK;
        size >>= MRFSTR_SIMD_CHAR_SHIFT;

        for (; size; s1block++, s2block++, size--)
            if (mrfstr_simd_cmpneq_func(*s1block, *s2block))
                return MRFSTR_FALSE;

        return !memcmp((mrfstr_data_ct)s1block, (mrfstr_data_ct)s2block, rem);
#if MRFSTR_THREADING
    }

    mrfstr_size_t rem = size % MRFSTR_THREAD_CHUNK;
    size /= MRFSTR_THREAD_CHUNK;

    mrfstr_bool_t res = MRFSTR_TRUE;

    pthread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_bit_t i;
    mrfstr_equal_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = __mrstr_alloc_una(sizeof(struct __MRFSTR_EQUAL_T));
        data->str1 = s1block;
        data->str2 = s2block;
        data->size = size;
        data->res = &res;

        s1block += size;
        s2block += size;

        pthread_create(threads + i, NULL, mrfstr_equal_threaded, data);
    }

    mrfstr_bool_t rres = !memcmp((mrfstr_data_ct)s2block, (mrfstr_data_ct)s1block, rem);
    if (!rres)
        res = MRFSTR_FALSE;

    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    return res;
#endif
}

#if MRFSTR_THREADING
void *mrfstr_equal_threaded(void *args)
{
    mrfstr_equal_t data = (mrfstr_equal_t)args;

    mrfstr_size_t i;
    while (data->size > 65536)
    {
        if (!*data->res)
        {
            __mrstr_free_una(data);
            return NULL;
        }

        for (i = 0; i < 65536; data->str1++, data->str2++, i++)
            if (mrfstr_simd_cmpneq_func(*data->str1, *data->str2))
            {
                *data->res = MRFSTR_FALSE;

                __mrstr_free_una(data);
                return NULL;
            }

        data->size -= 65536;
    }

    if (!*data->res)
    {
        __mrstr_free_una(data);
        return NULL;
    }

    for (; data->size; data->str1++, data->str2++, data->size--)
        if (mrfstr_simd_cmpneq_func(*data->str1, *data->str2))
        {
            *data->res = MRFSTR_FALSE;

            __mrstr_free_una(data);
            return NULL;
        }

    __mrstr_free_una(data);
    return NULL;
}
#endif
