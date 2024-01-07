/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

#define mrfstr_replace_chr_rem \
    for (; rem; rem--, str++)  \
        if (*str == old)       \
            *str = new

#define mrfstr_replace_chr2_rem \
    for (; rem; rem--, str++)   \
        *res++ = *str == old ? new : *str

#pragma pack(push, 1)
struct __MRFSTR_REPLACE_CHR_T
{
    mrfstr_data_t str;
    mrfstr_size_t size;

    mrfstr_chr_t old;
    mrfstr_chr_t new;
};
#pragma pack(pop)
typedef struct __MRFSTR_REPLACE_CHR_T *mrfstr_replace_chr_t;

#pragma pack(push, 1)
struct __MRFSTR_REPLACE_CHR2_T
{
    mrfstr_data_t res;
    mrfstr_data_ct str;
    mrfstr_size_t size;

    mrfstr_chr_t old;
    mrfstr_chr_t new;
};
#pragma pack(pop)
typedef struct __MRFSTR_REPLACE_CHR2_T *mrfstr_replace_chr2_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_replace_chr_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replace_chr_threaded(
    LPVOID args);
#endif

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_replace_chr2_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replace_chr2_threaded(
    LPVOID args);
#endif

void __mrfstr_replace_chr(
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
        mrfstr_byte_t rem = (uintptr_t)str % _mrfstr_config.nreplace_chr_size;
        if (rem)
        {
            rem = _mrfstr_config.nreplace_chr_size - rem;
            size -= rem;
            mrfstr_replace_chr_rem;
        }

        rem = size % _mrfstr_config.nreplace_chr_size;
        size -= rem;

        _mrfstr_config.nreplace_chr_sub(
            str, old, new, size / _mrfstr_config.nreplace_chr_size);
        str += size;

        mrfstr_replace_chr_rem;
        return;
    }

    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * MRFSTR_TSIZE)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_short_t rem = (uintptr_t)str & _mrfstr_config.treplace_chr_size;
    if (rem)
    {
        rem = _mrfstr_config.treplace_chr_size - rem;
        size -= rem;
        mrfstr_replace_chr_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.treplace_chr_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.treplace_chr_size;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_replace_chr_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_REPLACE_CHR_T));
            if (!data)
                break;

            data->str = str;
            data->size = size;
            data->old = old;
            data->new = new;

            str += inc;

            mrfstr_create_thread(__mrfstr_replace_chr_threaded)
            {
                str -= inc;
                free(data);
                break;
            }
        }

        tcount -= i;
    }

    _mrfstr_config.treplace_chr_sub(str, old, new, size * tcount);

    inc *= tcount;
    str += inc;
    mrfstr_replace_chr_rem;

    mrfstr_close_threads;
    free(threads);
}

void __mrfstr_replace_chr2(
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
        mrfstr_byte_t rem = (uintptr_t)str % _mrfstr_config.nreplace_chr_size;
        if (rem)
        {
            rem = _mrfstr_config.nreplace_chr_size - rem;
            size -= rem;
            mrfstr_replace_chr2_rem;
        }

        rem = size % _mrfstr_config.nreplace_chr_size;
        size -= rem;

        _mrfstr_config.nreplace_chr2_sub(
            res, str, old, new, size / _mrfstr_config.nreplace_chr_size);
        res += size;
        str += size;

        mrfstr_replace_chr2_rem;
        return;
    }

    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * MRFSTR_TSIZE)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_short_t rem = (uintptr_t)res & _mrfstr_config.treplace_chr_size;
    if (rem)
    {
        rem = _mrfstr_config.treplace_chr_size - rem;
        size -= rem;
        mrfstr_replace_chr2_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.treplace_chr_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.treplace_chr_size;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_replace_chr2_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_REPLACE_CHR2_T));
            if (!data)
                break;

            data->res = res;
            data->str = str;
            data->size = size;
            data->old = old;
            data->new = new;

            res += inc;
            str += inc;

            mrfstr_create_thread(__mrfstr_replace_chr2_threaded)
            {
                res -= inc;
                str -= inc;
                free(data);
                break;
            }
        }

        tcount -= i;
    }

    _mrfstr_config.treplace_chr2_sub(res, str, old, new, size * tcount);

    inc *= tcount;
    res += inc;
    str += inc;
    mrfstr_replace_chr2_rem;

    mrfstr_close_threads;
    free(threads);
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_replace_chr_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replace_chr_threaded(
    LPVOID args)
#endif
{
    mrfstr_replace_chr_t data = (mrfstr_replace_chr_t)args;
    _mrfstr_config.treplace_chr_sub(data->str, data->old, data->new, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_replace_chr2_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_replace_chr2_threaded(
    LPVOID args)
#endif
{
    mrfstr_replace_chr2_t data = (mrfstr_replace_chr2_t)args;
    _mrfstr_config.treplace_chr2_sub(
        data->res, data->str, data->old, data->new, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
