/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

struct __MRFSTR_COPY_T
{
    mrfstr_data_t dst;
    mrfstr_data_ct src;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_COPY_T *mrfstr_copy_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_copy_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_copy_threaded(
    LPVOID args);
#endif

void __mrfstr_copy(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
#ifndef MRFSTR_NOSIMD
    if (size < MRFSTR_SLIMIT)
#else
    if (mrfstr_config.thread_count == 1 || size < MRFSTR_TLIMIT)
#endif
    {
        memcpy(dst, src, size);
        return;
    }

#ifndef MRFSTR_NOSIMD
    if (mrfstr_config.thread_count == 1 || size < MRFSTR_TLIMIT)
    {
        if (!mrfstr_config.ncopy_sub)
        {
            memcpy(dst, src, size);
            return;
        }

        mrfstr_byte_t align = (uintptr_t)dst % mrfstr_config.ncopy_size;
        if (align)
        {
            align = mrfstr_config.ncopy_size - align;
            memcpy(dst, src, align);

            dst += align;
            src += align;
            size -= align;
        }

        mrfstr_byte_t rem = size % mrfstr_config.ncopy_size;
        size -= rem;

        mrfstr_config.ncopy_sub(dst, src, size / mrfstr_config.ncopy_size);
        dst += size;
        src += size;

        memcpy(dst, src, rem);
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
    if (mrfstr_config.tcopy_sub)
    {
        mrfstr_byte_t align = (uintptr_t)dst % mrfstr_config.tcopy_size;
        if (align)
        {
            align = mrfstr_config.tcopy_size - align;
            memcpy(dst, src, align);

            dst += align;
            src += align;
            size -= align;
        }

        mrfstr_short_t factor = mrfstr_config.tcopy_size * tcount;
        rem = size % factor;
        size /= factor;
        inc = size * mrfstr_config.tcopy_size;
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
        mrfstr_copy_t data;
        for (; i < nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_COPY_T));
            if (!data)
                break;

            data->dst = dst;
            data->src = src;
            data->size = size;

            dst += inc;
            src += inc;

            mrfstr_create_thread(__mrfstr_copy_threaded)
            {
                dst -= inc;
                src -= inc;

                free(data);
                break;
            }
        }

        tcount -= i;
    }

#ifndef MRFSTR_NOSIMD
    inc *= tcount;
    if (mrfstr_config.tcopy_sub)
    {
        mrfstr_config.tcopy_sub(dst, src, size * tcount);
        dst += inc;
        src += inc;

        memcpy(dst, src, rem);
    }
    else
#endif
        memcpy(dst, src, inc + rem);

    if (i)
        mrfstr_close_threads;
    free(threads);
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_copy_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_copy_threaded(
    LPVOID args)
#endif
{
    mrfstr_copy_t data = args;

#ifndef MRFSTR_NOSIMD
    if (mrfstr_config.tcopy_sub)
        mrfstr_config.tcopy_sub(data->dst, data->src, data->size);
    else
#endif
        memcpy(data->dst, data->src, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
