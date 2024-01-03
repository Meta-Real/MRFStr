/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

#define MRFSTR_NOSIMD_TCMP_LOAD 0x80000

#define mrfstr_ncmp_sub(size, inc)                     \
    do                                                 \
    {                                                  \
        if (!mrfstr_config.ncmp_sub(str1, str2, size)) \
            return MRFSTR_FALSE;                       \
                                                       \
        str1 += inc;                                   \
        str2 += inc;                                   \
        return !memcmp(str1, str2, rem);               \
    } while (0)

#define mrfstr_tcmp_sub(res, size, ret)             \
    do                                              \
    {                                               \
        mrfstr_size_t nsize;                        \
        while (size >= MRFSTR_NOSIMD_TCMP_LOAD)     \
        {                                           \
            if (!res)                               \
            {                                       \
                ret;                                \
            }                                       \
                                                    \
            nsize = size - MRFSTR_NOSIMD_TCMP_LOAD; \
            for (; size != nsize; size--)           \
                if (*s1block++ != *s2block++)       \
                {                                   \
                    res = MRFSTR_FALSE;             \
                    ret;                            \
                }                                   \
        }                                           \
                                                    \
        if (!res)                                   \
        {                                           \
            ret;                                    \
        }                                           \
                                                    \
        for (; size; size--)                        \
            if (*s1block++ != *s2block++)           \
            {                                       \
                res = MRFSTR_FALSE;                 \
                ret;                                \
            }                                       \
    } while (0)

struct __MRFSTR_CMP_T
{
    mrfstr_data_ct str1;
    mrfstr_data_ct str2;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_CMP_T *mrfstr_cmp_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t mrfstr_cmp_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI mrfstr_cmp_threaded(
    LPVOID args);
#endif

mrfstr_bool_t mrfstr_cmp(
    mrfstr_data_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
#ifndef MRFSTR_NOSIMD
    if (size < MRFSTR_SLIMIT)
#else
    if (mrfstr_config.thread_count == 1 || size < MRFSTR_TLIMIT)
#endif
        return !memcmp(str1, str2, size);

#ifndef MRFSTR_NOSIMD
    if (mrfstr_config.thread_count == 1 || size < MRFSTR_TLIMIT)
    {
        if (!mrfstr_config.ncmp_sub)
            return !memcmp(str1, str2, size);

        mrfstr_byte_t align = (uintptr_t)str1 % mrfstr_config.ncmp_size;
        if (align)
        {
            align = mrfstr_config.ncmp_size - align;
            if (memcmp(str1, str2, align))
                return MRFSTR_FALSE;

            str1 += align;
            str2 += align;
            size -= align;
        }

        mrfstr_byte_t rem = size % mrfstr_config.ncmp_size;
        size -= rem;
        mrfstr_ncmp_sub(size / mrfstr_config.ncmp_size, size);
    }
#endif

    mrfstr_byte_t tcount;
    if (size > mrfstr_config.thread_count * MRFSTR_TSIZE)
        tcount = mrfstr_config.thread_count;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_size_t inc;
#ifndef MRFSTR_NOSIMD
    mrfstr_short_t rem;
    if (mrfstr_config.tcmp_sub)
    {
        mrfstr_byte_t align = (uintptr_t)str1 % mrfstr_config.tcmp_size;
        if (align)
        {
            align = mrfstr_config.tcmp_size - align;
            if (memcmp(str1, str2, align))
                return MRFSTR_FALSE;

            str1 += align;
            str2 += align;
            size -= align;
        }

        mrfstr_short_t factor = mrfstr_config.tcmp_size * tcount;
        rem = size % factor;
        inc = (size /= factor) * mrfstr_config.tcmp_size;
    }
    else
    {
        mrfstr_short_t factor = sizeof(mrfstr_size_t) * tcount;
        rem = size % factor;
        inc = (size /= factor) * sizeof(mrfstr_size_t);
    }
#else
    mrfstr_short_t factor = sizeof(mrfstr_size_t) * tcount;
    mrfstr_short_t rem = size % factor;
    inc = (size /= factor) * sizeof(mrfstr_size_t);
#endif

    volatile mrfstr_bool_t res = MRFSTR_TRUE;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_cmp_t data;
        for (i = 0; i < nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_CMP_T));
            if (!data)
            {
                if (!i)
                {
                    inc *= tcount;
#ifndef MRFSTR_NOSIMD
                    if (mrfstr_config.tcmp_sub)
                        mrfstr_ncmp_sub(size * tcount, inc);
                    else
#endif
                        return !memcmp(str1, str2, inc + rem);
                }
                break;
            }

            data->str1 = str1;
            data->str2 = str2;
            data->size = size;
            data->res = &res;

            str1 += inc;
            str2 += inc;

            mrfstr_create_thread(mrfstr_cmp_threaded)
            {
                str1 -= size;
                str2 -= size;
                free(data);

                if (!i)
                {
                    inc *= tcount;
#ifndef MRFSTR_NOSIMD
                    if (mrfstr_config.tcmp_sub)
                        mrfstr_ncmp_sub(size * tcount, inc);
                    else
#endif
                        return !memcmp(str1, str2, inc + rem);
                }
                break;
            }
        }

        tcount -= i;
    }

#ifndef MRFSTR_NOSIMD
    inc *= tcount;
    if (mrfstr_config.tcmp_sub)
    {
        mrfstr_config.tcmp_sub(&res, str1, str2, size * tcount);
        if (!res)
        {
            mrfstr_close_threads;
            free(threads);
            return MRFSTR_FALSE;
        }

        str1 += inc;
        str2 += inc;
        if (memcmp(str1, str2, rem))
            res = MRFSTR_FALSE;
    }
    else
#endif
    {
        mrfstr_size_t *s1block = (mrfstr_size_t*)str1;
        mrfstr_size_t *s2block = (mrfstr_size_t*)str2;

        mrfstr_tcmp_sub(res, size,
            mrfstr_close_threads; free(threads); return res);

        if (memcmp(s1block, s2block, rem))
            res = MRFSTR_FALSE;
    }

    mrfstr_close_threads;
    free(threads);
    return res;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t mrfstr_cmp_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI mrfstr_cmp_threaded(
    LPVOID args)
#endif
{
    mrfstr_cmp_t data = (mrfstr_cmp_t)args;

    if (mrfstr_config.tcmp_sub)
        mrfstr_config.tcmp_sub(data->res, data->str1, data->str2, data->size);
    else
    {
        mrfstr_size_t *s1block = (mrfstr_size_t*)data->str1;
        mrfstr_size_t *s2block = (mrfstr_size_t*)data->str2;

        mrfstr_tcmp_sub(*data->res, data->size,
            free(data); return MRFSTR_TFUNC_RET);
    }

    free(data);
    return MRFSTR_TFUNC_RET;
}
