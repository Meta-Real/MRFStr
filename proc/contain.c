/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

#define mrfstr_contain_rem \
    for (; rem; rem--)     \
        if (chr == *str++) \
            return MRFSTR_TRUE

struct __MRFSTR_CONTAIN_T
{
    mrfstr_data_ct str;
    mrfstr_chr_t chr;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
typedef struct __MRFSTR_CONTAIN_T *mrfstr_contain_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_contain_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_contain_threaded(
    LPVOID args);
#endif

mrfstr_bool_t __mrfstr_contain(mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        for (; size; size--)
            if (chr == *str++)
                return MRFSTR_TRUE;
        return MRFSTR_FALSE;
    }

    if (mrfstr_config.thread_count == 1 || size < MRFSTR_TLIMIT)
    {
        mrfstr_byte_t rem = (uintptr_t)str % mrfstr_config.ncontain_size;
        if (rem)
        {
            rem = mrfstr_config.ncontain_size - rem;
            size -= rem;
            mrfstr_contain_rem;
        }

        rem = size % mrfstr_config.ncontain_size;
        size -= rem;

        if (mrfstr_config.ncontain_sub(str, chr, size / mrfstr_config.ncontain_size))
            return MRFSTR_TRUE;
        str += size;

        mrfstr_contain_rem;
        return MRFSTR_FALSE;
    }

    mrfstr_byte_t tcount;
    if (size > mrfstr_config.thread_count * MRFSTR_TSIZE)
        tcount = mrfstr_config.thread_count;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_short_t rem = (uintptr_t)str % mrfstr_config.tcontain_size;
    if (rem)
    {
        rem = mrfstr_config.tcontain_size - rem;
        size -= rem;
        mrfstr_contain_rem;
    }

    mrfstr_short_t factor = mrfstr_config.tcontain_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * mrfstr_config.tcontain_size;

    volatile mrfstr_bool_t res = MRFSTR_FALSE;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_contain_t data;
        for (i = 0; i < nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_CONTAIN_T));
            if (!data)
            {
                if (!i)
                {
                    if (mrfstr_config.ncontain_sub(str, chr, size * tcount))
                        return MRFSTR_TRUE;
                    str += inc * tcount;

                    mrfstr_contain_rem;
                    return MRFSTR_FALSE;
                }
                break;
            }

            data->str = str;
            data->chr = chr;
            data->size = size;
            data->res = &res;

            str += inc;

            mrfstr_create_thread(__mrfstr_contain_threaded)
            {
                str -= inc;
                free(data);

                if (!i)
                {
                    if (mrfstr_config.ncontain_sub(str, chr, size * tcount))
                        return MRFSTR_TRUE;
                    str += inc * tcount;

                    mrfstr_contain_rem;
                    return MRFSTR_FALSE;
                }
                break;
            }
        }

        tcount -= i;
    }

    mrfstr_config.tcontain_sub(&res, str, chr, size * tcount);
    str += inc * tcount;

    mrfstr_contain_rem;

    if (i)
        mrfstr_close_threads;
    free(threads);
    return res;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_contain_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_contain_threaded(
    LPVOID args)
#endif
{
    mrfstr_contain_t data = (mrfstr_contain_t)args;
    mrfstr_config.tcontain_sub(data->res, data->str, data->chr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
