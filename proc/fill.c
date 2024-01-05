/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

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

#include <stdio.h>
void __mrfstr_fill(
    mrfstr_data_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
#ifndef MRFSTR_NOSIMD
    if (size < MRFSTR_SLIMIT)
#else
    if (mrfstr_config.thread_count == 1 || size < MRFSTR_TLIMIT)
#endif
    {
        memset(res, chr, size);
        return;
    }

#ifndef MRFSTR_NOSIMD
    if (mrfstr_config.thread_count == 1 || size < MRFSTR_TLIMIT)
    {
        if (!mrfstr_config.nfill_sub)
        {
            memset(res, chr, size);
            return;
        }

        mrfstr_byte_t align = (uintptr_t)res % mrfstr_config.nfill_size;
        if (align)
        {
            align = mrfstr_config.nfill_size - align;
            memset(res, chr, align);

            res += align;
            size -= align;
        }

        mrfstr_byte_t rem = size % mrfstr_config.nfill_size;
        size -= rem;

        mrfstr_config.nfill_sub(res, chr, size / mrfstr_config.nfill_size);
        res += size;

        memset(res, chr, rem);
        return;
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
    if (mrfstr_config.tfill_sub)
    {
        mrfstr_byte_t align = (uintptr_t)res % mrfstr_config.tfill_size;
        if (align)
        {
            align = mrfstr_config.tfill_size - align;
            memset(res, chr, align);

            res += align;
            size -= align;
        }

        mrfstr_short_t factor = mrfstr_config.tfill_size * tcount;
        rem = size % factor;
        size /= factor;
        inc = size * mrfstr_config.tfill_size;
    }
    else
    {
        rem = size % tcount;
        size /= tcount;
        inc = size;
    }
#else
    mrfstr_byte_t rem = size % tcount;
    size /= tcount;
    inc = size;
#endif

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

#ifndef MRFSTR_NOSIMD
    inc *= tcount;
    if (mrfstr_config.tfill_sub)
    {
        mrfstr_config.tfill_sub(res, chr, size * tcount);
        res += inc;

        memset(res, chr, rem);
    }
    else
#endif
        memset(res, chr, inc + rem);

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

#ifndef MRFSTR_NOSIMD
    if (mrfstr_config.tfill_sub)
        mrfstr_config.tfill_sub(data->res, data->chr, data->size);
    else
#endif
        memset(data->res, data->chr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
