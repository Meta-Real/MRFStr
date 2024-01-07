/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

#define mrfstr_contain_chr_rem \
    for (; rem; rem--)         \
        if (chr == *str++)     \
            return MRFSTR_TRUE

#pragma pack(push, 1)
struct __MRFSTR_CONTAIN_CHR_T
{
    mrfstr_data_ct str;
    mrfstr_chr_t chr;
    mrfstr_size_t size;

    volatile mrfstr_bool_t *res;
};
#pragma pack(pop)
typedef struct __MRFSTR_CONTAIN_CHR_T *mrfstr_contain_chr_t;

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_contain_chr_threaded(
    mrfstr_ptr_t args);
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_contain_chr_threaded(
    LPVOID args);
#endif

mrfstr_bool_t __mrfstr_contain_chr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
        return memchr(str, chr, size) != NULL;

    if (_mrfstr_config.tcount == 1 || size < MRFSTR_TLIMIT)
    {
        mrfstr_byte_t rem = (uintptr_t)str % _mrfstr_config.ncontain_chr_size;
        if (rem)
        {
            rem = _mrfstr_config.ncontain_chr_size - rem;
            size -= rem;
            mrfstr_contain_chr_rem;
        }

        rem = size % _mrfstr_config.ncontain_chr_size;
        size -= rem;

        if (_mrfstr_config.ncontain_chr_sub(str, chr, size / _mrfstr_config.ncontain_chr_size))
            return MRFSTR_TRUE;
        str += size;

        mrfstr_contain_chr_rem;
        return MRFSTR_FALSE;
    }

    mrfstr_byte_t tcount;
    if (size > _mrfstr_config.tcount * MRFSTR_TSIZE)
        tcount = _mrfstr_config.tcount;
    else
        tcount = (mrfstr_byte_t)(size / MRFSTR_TSIZE);

    mrfstr_short_t rem = (uintptr_t)str % _mrfstr_config.tcontain_chr_size;
    if (rem)
    {
        rem = _mrfstr_config.tcontain_chr_size - rem;
        size -= rem;
        mrfstr_contain_chr_rem;
    }

    mrfstr_short_t factor = _mrfstr_config.tcontain_chr_size * tcount;
    rem = size % factor;
    mrfstr_size_t inc = (size /= factor) * _mrfstr_config.tcontain_chr_size;

    volatile mrfstr_bool_t res = MRFSTR_FALSE;

    mrfstr_byte_t nthreads = tcount - 1;
    mrfstr_thread_t *threads = malloc(nthreads * sizeof(mrfstr_thread_t));
    mrfstr_byte_t i = 0;
    if (threads)
    {
        mrfstr_contain_chr_t data;
        for (i = 0; i != nthreads; i++)
        {
            data = malloc(sizeof(struct __MRFSTR_CONTAIN_CHR_T));
            if (!data)
            {
                if (!i)
                {
                    if (_mrfstr_config.ncontain_chr_sub(str, chr, size * tcount))
                        return MRFSTR_TRUE;
                    str += inc * tcount;

                    mrfstr_contain_chr_rem;
                    return MRFSTR_FALSE;
                }
                break;
            }

            data->str = str;
            data->chr = chr;
            data->size = size;
            data->res = &res;

            str += inc;

            mrfstr_create_thread(__mrfstr_contain_chr_threaded)
            {
                str -= inc;
                free(data);

                if (!i)
                {
                    if (_mrfstr_config.ncontain_chr_sub(str, chr, size * tcount))
                        return MRFSTR_TRUE;
                    str += inc * tcount;

                    mrfstr_contain_chr_rem;
                    return MRFSTR_FALSE;
                }
                break;
            }
        }

        tcount -= i;
    }

    _mrfstr_config.tcontain_chr_sub(&res, str, chr, size * tcount);
    str += inc * tcount;

    mrfstr_contain_chr_rem;

    if (i)
        mrfstr_close_threads;
    free(threads);
    return res;
}

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
mrfstr_ptr_t __mrfstr_contain_chr_threaded(
    mrfstr_ptr_t args)
#elif defined(_WIN32)
DWORD WINAPI __mrfstr_contain_chr_threaded(
    LPVOID args)
#endif
{
    mrfstr_contain_chr_t data = (mrfstr_contain_chr_t)args;
    _mrfstr_config.tcontain_chr_sub(data->res, data->str, data->chr, data->size);

    free(data);
    return MRFSTR_TFUNC_RET;
}
