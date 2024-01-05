/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

#define mrfstr_fill_rem \
    for (; rem; rem--)  \
        *res++ = chr

struct __MRFSTR_FILL_T
{
    mrfstr_data_t res;
    mrfstr_chr_t chr;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_FILL_T *mrfstr_fill_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_fill_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_fill_threaded(
    LPVOID args);
#endif

void __mrfstr_fill(
    mrfstr_data_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        for (; size; size--)
            *res++ = chr;
        return;
    }

    if (mrfstr_config.thread_count == 1 || size < MRFSTR_TLIMIT)
    {
        mrfstr_byte_t rem = (uintptr_t)res % mrfstr_config.nfill_size;
        if (rem)
        {
            rem = mrfstr_config.nfill_size - rem;
            size -= rem;
            mrfstr_fill_rem;
        }

        rem = size % mrfstr_config.nfill_size;
        size -= rem;

        mrfstr_config.nfill_sub(res, chr, size / mrfstr_config.nfill_size);
        res += size;

        mrfstr_fill_rem;
        return;
    }

    mrfstr_byte_t tcount;
    if (size > mrfstr_config.thread_count * MRFSTR_TSIZE)
        tcount = mrfstr_config.thread_count;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_short_t rem = (uintptr_t)res % mrfstr_config.tfill_size;
    if (rem)
    {
        rem = mrfstr_config.tfill_size - rem;
        size -= rem;
        mrfstr_fill_rem;
    }

    mrfstr_short_t factor = mrfstr_config.tfill_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * mrfstr_config.tfill_size;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_fill_t data;
        for (i = 0; i < nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_FILL_T));
            if (!data)
                break;

            data->res = res;
            data->chr = chr;
            data->size = size;

            res += inc;

            mrfstr_create_thread(__mrfstr_fill_threaded)
            {
                res -= inc;

                free(data);
                break;
            }
        }

        tcount -= i;
    }

    mrfstr_config.tfill_sub(res, chr, size * tcount);
    res += inc * tcount;

    mrfstr_fill_rem;

    if (i)
        mrfstr_close_threads;
    free(threads);
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_fill_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_fill_threaded(
    LPVOID args)
#endif
{
    mrfstr_fill_t data = (mrfstr_fill_t)args;
    mrfstr_config.tfill_sub(data->res, data->chr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
