/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

#define mrfstr_replchr_rem     \
    for (; rem; rem--, str++)  \
        if (*str == old)       \
            *str = new

#define mrfstr_replchr2_rem   \
    for (; rem; rem--, str++) \
        *res++ = *str == old ? new : *str

#pragma pack(push, 1)
struct __MRFSTR_REPLCHR_T
{
    mrfstr_data_t str;
    mrfstr_size_t size;

    mrfstr_chr_t old;
    mrfstr_chr_t new;
};
#pragma pack(pop)
typedef struct __MRFSTR_REPLCHR_T *mrfstr_replchr_t;

#pragma pack(push, 1)
struct __MRFSTR_REPLCHR2_T
{
    mrfstr_data_t res;
    mrfstr_data_ct str;
    mrfstr_size_t size;

    mrfstr_chr_t old;
    mrfstr_chr_t new;
};
#pragma pack(pop)
typedef struct __MRFSTR_REPLCHR2_T *mrfstr_replchr2_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_replchr_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replchr_threaded(
    LPVOID args);
#endif

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_replchr2_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replchr2_threaded(
    LPVOID args);
#endif

void __mrfstr_replchr(
    mrfstr_data_t str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        for (; size; size--, str++)
            if (*str == old)
                *str = new;
        return;
    }

    if (_mrfstr_config.tcount == 1 || size < MRFSTR_TLIMIT)
    {
        mrfstr_byte_t rem = (uintptr_t)str % _mrfstr_config.nrepl_size;
        if (rem)
        {
            rem = _mrfstr_config.nrepl_size - rem;
            size -= rem;
            mrfstr_replchr_rem;
        }

        rem = size % _mrfstr_config.nrepl_size;
        size -= rem;

        _mrfstr_config.nreplchr_sub(
            str, old, new, size / _mrfstr_config.nrepl_size);
        str += size;

        mrfstr_replchr_rem;
        return;
    }

    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * MRFSTR_TSIZE)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_short_t rem = (uintptr_t)str & _mrfstr_config.trepl_size;
    if (rem)
    {
        rem = _mrfstr_config.trepl_size - rem;
        size -= rem;
        mrfstr_replchr_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.trepl_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.trepl_size;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_replchr_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_REPLCHR_T));
            if (!data)
                break;

            data->str = str;
            data->size = size;
            data->old = old;
            data->new = new;

            str += inc;

            mrfstr_create_thread(__mrfstr_replchr_threaded)
            {
                str -= inc;
                free(data);
                break;
            }
        }

        tcount -= i;
    }

    _mrfstr_config.treplchr_sub(str, old, new, size * tcount);

    inc *= tcount;
    str += inc;
    mrfstr_replchr_rem;

    if (i)
        mrfstr_close_threads;
    free(threads);
}

void __mrfstr_replchr2(
    mrfstr_data_t res, mrfstr_data_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        for (; size; size--, str++)
            *res++ = *str == old ? new : *str;
        return;
    }

    if (_mrfstr_config.tcount == 1 || size < MRFSTR_TLIMIT)
    {
        mrfstr_byte_t rem = (uintptr_t)str % _mrfstr_config.nrepl_size;
        if (rem)
        {
            rem = _mrfstr_config.nrepl_size - rem;
            size -= rem;
            mrfstr_replchr2_rem;
        }

        rem = size % _mrfstr_config.nrepl_size;
        size -= rem;

        _mrfstr_config.nreplchr2_sub(
            res, str, old, new, size / _mrfstr_config.nrepl_size);
        res += size;
        str += size;

        mrfstr_replchr2_rem;
        return;
    }

    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * MRFSTR_TSIZE)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_short_t rem = (uintptr_t)res & _mrfstr_config.trepl_size;
    if (rem)
    {
        rem = _mrfstr_config.trepl_size - rem;
        size -= rem;
        mrfstr_replchr2_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.trepl_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.trepl_size;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_replchr2_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_REPLCHR2_T));
            if (!data)
                break;

            data->res = res;
            data->str = str;
            data->size = size;
            data->old = old;
            data->new = new;

            res += inc;
            str += inc;

            mrfstr_create_thread(__mrfstr_replchr2_threaded)
            {
                res -= inc;
                str -= inc;
                free(data);
                break;
            }
        }

        tcount -= i;
    }

    _mrfstr_config.treplchr2_sub(res, str, old, new, size * tcount);

    inc *= tcount;
    res += inc;
    str += inc;
    mrfstr_replchr2_rem;

    if (i)
        mrfstr_close_threads;
    free(threads);
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_replchr_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replchr_threaded(
    LPVOID args)
#endif
{
    mrfstr_replchr_t data = (mrfstr_replchr_t)args;
    _mrfstr_config.treplchr_sub(data->str, data->old, data->new, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_replchr2_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replchr2_threaded(
    LPVOID args)
#endif
{
    mrfstr_replchr2_t data = (mrfstr_replchr2_t)args;
    _mrfstr_config.treplchr2_sub(
        data->res, data->str, data->old, data->new, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
