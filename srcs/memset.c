/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <alloc.h>
#include <string.h>

#ifndef MRFSTR_NOSIMD
#define mrfstr_memset_sub(r, c, s) \
    for (; s; r++, s--)            \
        mrfstr_simd_stream(r, block)
#endif

#if MRFSTR_THREADING

struct __MRFSTR_MEMSET_T
{
    mrfstr_simd_t *res;
    mrfstr_chr_t chr;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_MEMSET_T *mrfstr_memset_t;

#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_memset_threaded(void *args);
#elif defined(_WIN32)
DWORD WINAPI mrfstr_memset_threaded(LPVOID args);
#endif

#endif

void mrfstr_memset(mrfstr_data_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    if (size < MRFSTR_SIMD_SLIMIT)
    {
        memset(res, chr, size);
        return;
    }

#ifndef MRFSTR_NOSIMD
    mrfstr_byte_t align = (uintptr_t)res & MRFSTR_SIMD_MASK;
    if (align)
    {
        align = MRFSTR_SIMD_SIZE - align;
        memset(res, chr, align);

        res += align;
        size -= align;
    }
#endif

#if !defined(MRFSTR_NOSIMD) || MRFSTR_THREADING
    mrfstr_simd_t *rblock = (mrfstr_simd_t*)res;
#endif

#if MRFSTR_THREADING
    if (size <= MRFSTR_SIMD_TLIMIT)
    {
#endif
#ifdef MRFSTR_NOSIMD
        memset(res, chr, size);
#else
        mrfstr_byte_t rem = size & MRFSTR_SIMD_MASK;
        size >>= MRFSTR_SIMD_SHIFT;

        mrfstr_simd_t block = mrfstr_simd_set(chr);
        mrfstr_memset_sub(rblock, block, size);
        memset(rblock, chr, rem);
#endif
#if MRFSTR_THREADING
        return;
    }

#ifdef MRFSTR_NOSIMD
    mrfstr_byte_t rem = size % MRFSTR_THREAD_COUNT;
    size /= MRFSTR_THREAD_COUNT;
#else
    mrfstr_short_t rem = size % MRFSTR_SIMD_TCHK;
    size /= MRFSTR_SIMD_TCHK;
#endif

    mrfstr_thread_t threads[MRFSTR_THREAD_COUNT];
    mrfstr_byte_t i;
    mrfstr_memset_t data;
    for (i = 0; i < MRFSTR_THREAD_COUNT; i++)
    {
        data = mrstr_alloc(sizeof(struct __MRFSTR_MEMSET_T));
        if (!data)
            goto rem;

        data->res = rblock;
        data->chr = chr;
        data->size = size;

        rblock += size;

        mrfstr_create_thread(mrfstr_memset_threaded)
        {
            rblock -= size;

            mrstr_free(data);
            goto rem;
        }
    }

    memset(rblock, chr, rem);

ret:
    mrfstr_close_threads;
    return;

rem:
    size *= MRFSTR_THREAD_COUNT - i;

#ifdef MRFSTR_NOSIMD
    memset(rblock, chr, size + rem);
#else
    mrfstr_simd_t block = mrfstr_simd_set(chr);
    mrfstr_memset_sub(rblock, block, size);
    memset(rblock, chr, rem);
#endif

    goto ret;
#endif
}

#if MRFSTR_THREADING
#if defined(unix) || defined(__unix) || defined(__unix__)
void *mrfstr_memset_threaded(void *args)
#elif defined(_WIN32)
DWORD WINAPI mrfstr_memset_threaded(LPVOID args)
#endif
{
    mrfstr_memset_t data = (mrfstr_memset_t)args;

#ifdef MRFSTR_NOSIMD
    memset(data->res, data->chr, data->size);
#else
    mrfstr_simd_t block = mrfstr_simd_set(data->chr);
    mrfstr_memset_sub(data->res, block, data->size);
#endif

    mrstr_free(data);
    return MRFSTR_TFUNC_RET;
}
#endif
