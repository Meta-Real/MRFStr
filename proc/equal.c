/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

#define mrfstr_equal_rem        \
    for (; rem; rem--)          \
        if (*str1++ != *str2++) \
            return MRFSTR_FALSE

struct __MRFSTR_EQUAL_T
{
    mrfstr_data_ct str1;
    mrfstr_data_ct str2;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_EQUAL_T *mrfstr_equal_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_equal_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_equal_threaded(
    LPVOID args);
#endif

mrfstr_bool_t __mrfstr_equal(
    mrfstr_data_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        for (; size; size--)
            if (*str1++ != *str2++)
                return MRFSTR_FALSE;
        return MRFSTR_TRUE;
    }

    if (mrfstr_config.thread_count == 1 || size < MRFSTR_TLIMIT)
    {
        mrfstr_byte_t rem = (uintptr_t)str1 % mrfstr_config.nequal_size;
        if (rem)
        {
            rem = mrfstr_config.nequal_size - rem;
            size -= rem;
            mrfstr_equal_rem;
        }

        rem = size % mrfstr_config.nequal_size;
        size -= rem;

        if (!mrfstr_config.nequal_sub(str1, str2, size / mrfstr_config.nequal_size))
            return MRFSTR_FALSE;
        str1 += size;
        str2 += size;

        mrfstr_equal_rem;
        return MRFSTR_TRUE;
    }

    mrfstr_byte_t tcount;
    if (size > mrfstr_config.thread_count * MRFSTR_TSIZE)
        tcount = mrfstr_config.thread_count;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_short_t rem = (uintptr_t)str1 & mrfstr_config.tequal_size;
    if (rem)
    {
        rem = mrfstr_config.tequal_size - rem;
        size -= rem;
        mrfstr_equal_rem;
    }

    mrfstr_short_t factor = mrfstr_config.tequal_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * mrfstr_config.tequal_size;

    volatile mrfstr_bool_t res = MRFSTR_TRUE;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_equal_t data;
        for (i = 0; i < nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_EQUAL_T));
            if (!data)
            {
                if (!i)
                {
                    if (!mrfstr_config.nequal_sub(str1, str2, size * tcount))
                        return MRFSTR_FALSE;
                    inc *= tcount;
                    str1 += inc;
                    str2 += inc;

                    mrfstr_equal_rem;
                    return MRFSTR_TRUE;
                }
                break;
            }

            data->str1 = str1;
            data->str2 = str2;
            data->size = size;
            data->res = &res;

            str1 += inc;
            str2 += inc;

            mrfstr_create_thread(__mrfstr_equal_threaded)
            {
                str1 -= inc;
                str2 -= inc;
                free(data);

                if (!i)
                {
                    if (!mrfstr_config.nequal_sub(str1, str2, size * tcount))
                        return MRFSTR_FALSE;
                    inc *= tcount;
                    str1 += inc;
                    str2 += inc;

                    mrfstr_equal_rem;
                    return MRFSTR_TRUE;
                }
                break;
            }
        }

        tcount -= i;
    }

    mrfstr_config.tequal_sub(&res, str1, str2, size * tcount);
    if (!res)
    {
        mrfstr_close_threads;
        free(threads);
        return MRFSTR_FALSE;
    }

    inc *= tcount;
    str1 += inc;
    str2 += inc;
    mrfstr_equal_rem;

    mrfstr_close_threads;
    free(threads);
    return res;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_equal_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_equal_threaded(
    LPVOID args)
#endif
{
    mrfstr_equal_t data = (mrfstr_equal_t)args;
    mrfstr_config.tequal_sub(data->res, data->str1, data->str2, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
