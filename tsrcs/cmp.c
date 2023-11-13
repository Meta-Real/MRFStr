/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <immintrin.h>
#include <string.h>

struct __MRTSTR_MEMCMP_T
{
    __m512i *str1;
    __m512i *str2;
    mrtstr_size_t size;

    mrtstr_lock_t *s1lock;
    pthread_mutex_t *s1mutex;

    mrtstr_lock_t *s2lock;
    pthread_mutex_t *s2mutex;

    mrtstr_cmpres_t *res;
};
typedef struct __MRTSTR_MEMCMP_T *mrtstr_memcmp_t;

#define init_memcmp_struct()                                        \
    do                                                              \
    {                                                               \
        data = __mrstr_alloc_una(sizeof(struct __MRTSTR_MEMCMP_T)); \
        data->size = size;                                          \
        data->s1lock = str1->lock + i;                              \
        data->s1mutex = &str1->mutex;                               \
        data->s2lock = str2->lock + i;                              \
        data->s2mutex = &str2->mutex;                               \
        data->res = res;                                            \
    } while (0)

void mrtstr_equal_threaded(void *args);

void mrtstr_equal(mrtstr_cmpres_t *res, mrtstr_ct str1, mrtstr_ct str2)
{
    res->lock = 0;
    res->mutex = PTHREAD_MUTEX_INITIALIZER;

    if (str1->size != str2->size)
    {
        res->res = MRTSTR_FALSE;
        return;
    }

    if (!str1->size || str1->data == str2->data)
    {
        res->res = MRTSTR_TRUE;
        return;
    }

    __m512i *s1block = (__m512i*)str1->data;
    __m512i *s2block = (__m512i*)str2->data;

    mrtstr_size_t size = str1->size;
    if (size <= 25166207)
    {
        mrtstr_size_t rem = size & 63;
        size >>= 6;

        for (; size; s1block++, s2block++, size--)
            if (_mm512_cmpneq_epi64_mask(*s1block, *s2block))
            {
                res->res = MRTSTR_FALSE;
                return;
            }

        res->res = !memcmp((mrtstr_data_ct)s1block, (mrtstr_data_ct)s2block, rem);
        return;
    }

    res->res = MRTSTR_TRUE;

    mrtstr_size_t rem = size % 384;
    size /= 384;

    mrtstr_bit_t i;
    mrtstr_size_t j;
    mrtstr_memcmp_t data;
    if (mrtstr_locked(str1) && str1->forced)
    {
        if (mrtstr_locked(str2) && str2->forced)
            for (i = 0; i < 6; i++)
            {
                for (; str1->lock[i];);
                for (; str2->lock[i];);

                init_memcmp_struct();

                while (!mrtstr_threads.free_threads && data->size > 65536)
                {
                    for (j = 0; j < 65536; s1block++, s2block++, j++)
                        if (_mm512_cmpneq_epi64_mask(*s1block, *s2block))
                        {
                            res->res = MRTSTR_FALSE;

                            __mrstr_free_una(data);
                            return;
                        }

                    data->size -= 65536;
                }

                if (data->size <= 65536)
                {
                    for (; data->size; s1block++, s2block++, data->size--)
                        if (_mm512_cmpneq_epi64_mask(*s1block, *s2block))
                        {
                            res->res = MRTSTR_FALSE;

                            __mrstr_free_una(data);
                            return;
                        }

                    __mrstr_free_una(data);
                }
                else
                {
                    data->str1 = s1block;
                    data->str2 = s2block;

                    s1block += data->size;
                    s2block += data->size;

                    lock_inc(str1->lock[i], &str1->mutex);
                    lock_inc(str2->lock[i], &str2->mutex);
                    lock_inc(res->lock, &res->mutex);
                    mrtstr_load_threads(mrtstr_equal_threaded, data);
                }
            }
        else
            for (i = 0; i < 6; i++)
            {
                for (; str1->lock[i];);

                init_memcmp_struct();

                while (!mrtstr_threads.free_threads && data->size > 65536)
                {
                    for (j = 0; j < 65536; s1block++, s2block++, j++)
                        if (_mm512_cmpneq_epi64_mask(*s1block, *s2block))
                        {
                            res->res = MRTSTR_FALSE;

                            __mrstr_free_una(data);
                            return;
                        }

                    data->size -= 65536;
                }

                if (data->size <= 65536)
                {
                    for (; data->size; s1block++, s2block++, data->size--)
                        if (_mm512_cmpneq_epi64_mask(*s1block, *s2block))
                        {
                            res->res = MRTSTR_FALSE;

                            __mrstr_free_una(data);
                            return;
                        }

                    __mrstr_free_una(data);
                }
                else
                {
                    data->str1 = s1block;
                    data->str2 = s2block;

                    s1block += data->size;
                    s2block += data->size;

                    lock_inc(str1->lock[i], &str1->mutex);
                    lock_inc(str2->lock[i], &str2->mutex);
                    lock_inc(res->lock, &res->mutex);
                    mrtstr_load_threads(mrtstr_equal_threaded, data);
                }
            }
    }
    else if (mrtstr_locked(str2) && str2->forced)
        for (i = 0; i < 6; i++)
        {
            for (; str2->lock[i];);

            init_memcmp_struct();

            while (!mrtstr_threads.free_threads && data->size > 65536)
            {
                for (j = 0; j < 65536; s1block++, s2block++, j++)
                    if (_mm512_cmpneq_epi64_mask(*s1block, *s2block))
                    {
                        res->res = MRTSTR_FALSE;

                        __mrstr_free_una(data);
                        return;
                    }

                data->size -= 65536;
            }

            if (data->size <= 65536)
            {
                for (; data->size; s1block++, s2block++, data->size--)
                    if (_mm512_cmpneq_epi64_mask(*s1block, *s2block))
                    {
                        res->res = MRTSTR_FALSE;

                        __mrstr_free_una(data);
                        return;
                    }

                __mrstr_free_una(data);
            }
            else
            {
                data->str1 = s1block;
                data->str2 = s2block;

                s1block += data->size;
                s2block += data->size;

                lock_inc(str1->lock[i], &str1->mutex);
                lock_inc(str2->lock[i], &str2->mutex);
                lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_equal_threaded, data);
            }
        }
    else
        for (i = 0; i < 6; i++)
        {
            init_memcmp_struct();

            while (!mrtstr_threads.free_threads && data->size > 65536)
            {
                for (j = 0; j < 65536; s1block++, s2block++, j++)
                    if (_mm512_cmpneq_epi64_mask(*s1block, *s2block))
                    {
                        res->res = MRTSTR_FALSE;
                        return;
                    }

                data->size -= 65536;
            }

            if (data->size <= 65536)
            {
                for (; data->size; s1block++, s2block++, data->size--)
                    if (_mm512_cmpneq_epi64_mask(*s1block, *s2block))
                    {
                        res->res = MRTSTR_FALSE;
                        return;
                    }

                __mrstr_free_una(data);
            }
            else
            {
                data->str1 = s1block;
                data->str2 = s2block;

                s1block += data->size;
                s2block += data->size;

                lock_inc(str1->lock[i], &str1->mutex);
                lock_inc(str2->lock[i], &str2->mutex);
                lock_inc(res->lock, &res->mutex);
                mrtstr_load_threads(mrtstr_equal_threaded, data);
            }
        }

    str1->forced = MRTSTR_FALSE;
    str2->forced = MRTSTR_FALSE;

    mrtstr_bool_t rres = !memcmp((mrtstr_data_ct)s2block, (mrtstr_data_ct)s1block, rem);
    if (!rres)
        res->res = MRTSTR_FALSE;
}

void mrtstr_equal_threaded(void *args)
{
    mrtstr_memcmp_t data = (mrtstr_memcmp_t)args;

    mrtstr_size_t i;
    while (data->size > 65536)
    {
        if (!data->res->res)
        {
            lock_dec(data->s1lock, data->s1mutex);
            lock_dec(data->s2lock, data->s2mutex);
            lock_dec(data->res->lock, &data->res->mutex);

            __mrstr_free_una(data);
            return;
        }

        for (i = 0; i < 65536; data->str1++, data->str2++, i++)
            if (_mm512_cmpneq_epi64_mask(*data->str1, *data->str2))
            {
                data->res->res = MRTSTR_FALSE;

                lock_dec(data->s1lock, data->s1mutex);
                lock_dec(data->s2lock, data->s2mutex);
                lock_dec(data->res->lock, &data->res->mutex);

                __mrstr_free_una(data);
                return;
            }

        data->size -= 65536;
    }

    if (!data->res->res)
    {
        lock_dec(data->s1lock, data->s1mutex);
        lock_dec(data->s2lock, data->s2mutex);
        lock_dec(data->res->lock, &data->res->mutex);

        __mrstr_free_una(data);
        return;
    }

    for (; data->size; data->str1++, data->str2++, data->size--)
        if (_mm512_cmpneq_epi64_mask(*data->str1, *data->str2))
        {
            data->res->res = MRTSTR_FALSE;

            lock_dec(data->s1lock, data->s1mutex);
            lock_dec(data->s2lock, data->s2mutex);
            lock_dec(data->res->lock, &data->res->mutex);

            __mrstr_free_una(data);
            return;
        }

    lock_dec(data->s1lock, data->s1mutex);
    lock_dec(data->s2lock, data->s2mutex);
    lock_dec(data->res->lock, &data->res->mutex);

    __mrstr_free_una(data);
}
