/*
MIT License

Copyright (c) 2023 MetaReal

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/

#ifndef __MRFSTR_INTERN__
#define __MRFSTR_INTERN__

#include <mrfstr.h>
#include <stdlib.h>

#define MRFSTR_ALLOC(x, size)                                                \
    do                                                                       \
    {                                                                        \
        MRFSTR_DATA(x) = (mrfstr_data_t)malloc(size * sizeof(mrfstr_chr_t)); \
        if (!MRFSTR_DATA(x))                                                 \
            return MRFSTR_RES_MEM_ERROR;                                     \
                                                                             \
        MRFSTR_CAPA(x) = size;                                               \
    } while (0)

#define MRFSTR_REALLOC(x, size)                                                      \
    do                                                                               \
    {                                                                                \
        mrfstr_data_t block;                                                         \
                                                                                     \
        block = (mrfstr_data_t)realloc(MRFSTR_DATA(x), size * sizeof(mrfstr_chr_t)); \
        if (!block)                                                                  \
            return MRFSTR_RES_MEM_ERROR;                                             \
                                                                                     \
        MRFSTR_DATA(x) = block;                                                      \
        MRFSTR_CAPA(x) = size;                                                       \
    } while (0)

#define MRFSTR_CLEAR_REALLOC(x, size)                                        \
    do                                                                       \
    {                                                                        \
        if (MRFSTR_CAPA(x))                                                  \
            free(MRFSTR_DATA(x));                                            \
                                                                             \
        MRFSTR_DATA(x) = (mrfstr_data_t)malloc(size * sizeof(mrfstr_chr_t)); \
        if (!MRFSTR_DATA(x))                                                 \
        {                                                                    \
            MRFSTR_SIZE(x) = 0;                                              \
            MRFSTR_CAPA(x) = 0;                                              \
            return MRFSTR_RES_MEM_ERROR;                                     \
        }                                                                    \
                                                                             \
        MRFSTR_CAPA(x) = size;                                               \
    } while (0)

#define MRFSTR_FREE(x)            \
    do                            \
    {                             \
        if (MRFSTR_CAPA(x))       \
        {                         \
            free(MRFSTR_DATA(x)); \
                                  \
            MRFSTR_SIZE(x) = 0;   \
            MRFSTR_CAPA(x) = 0;   \
        }                         \
    } while (0)

enum __MRFSTR_SIMD_ENUM
{
    MRFSTR_SIMD_INT64,
    MRFSTR_SIMD_SSE2,
    MRFSTR_SIMD_SSSE3,
    MRFSTR_SIMD_SSE4_1,
    MRFSTR_SIMD_AVX,
    MRFSTR_SIMD_AVX2,
    MRFSTR_SIMD_AVX512F,
    MRFSTR_SIMD_AVX512BW,
    MRFSTR_SIMD_AVX512VL = 7,
    MRFSTR_SIMD_AVX512VBMI
};

#define MRFSTR_SUPPORTS_SIMD(x) (simdset >= x)

#define MRFSTR_SLIMIT 0x800
#define MRFSTR_ALIGN_SIZE 64
#define MRFSTR_ALIGN_MASK (MRFSTR_ALIGN_SIZE - 1)

#pragma pack(push, 1)
struct __MRFSTR_CONFIG_T
{
    /* mem functions */

    void (*copy_func)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);
    void (*rcopy_func)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);
    void (*fill_func)(
        mrfstr_ptr_t, mrfstr_chr_t, mrfstr_size_t);

    void (*copy_tfunc)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);
    void (*rcopy_tfunc)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);
    void (*fill_tfunc)(
        mrfstr_ptr_t, mrfstr_chr_t, mrfstr_size_t);

    mrfstr_size_t mem_tlimit;

    /* rev functions */

    void (*rev_func)(
        mrfstr_ptr_t, mrfstr_ptr_t, mrfstr_size_t);
    void (*rev2_func)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);

    void (*rev_tfunc)(
        mrfstr_ptr_t, mrfstr_ptr_t, mrfstr_size_t);
    void (*rev2_tfunc)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);

    mrfstr_size_t rev_tlimit;

    /* general */

    mrfstr_byte_t tcount;

    mrfstr_sbyte_t tprior;
    mrfstr_short_t stdalloc;
};
#pragma pack(pop)
typedef struct __MRFSTR_CONFIG_T mrfstr_config_t;
extern mrfstr_config_t _mrfstr_config;

#define mrfstr_set_tcount(tlimit)                   \
    do                                              \
    {                                               \
        tsize = tlimit >> 1;                        \
        if (size > _mrfstr_config.tcount * tsize)   \
            tcount = _mrfstr_config.tcount;         \
        else                                        \
            tcount = (mrfstr_byte_t)(size / tsize); \
    } while (0)

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define MRFSTR_BUILD_UNIX

#include <pthread.h>

typedef pthread_t mrfstr_thread_t;
typedef pthread_mutex_t mrfstr_mutex_t;
typedef mrfstr_mutex_t *mrfstr_mutex_p;

#define MRFSTR_TFUNC_RET NULL
#define MRFSTR_CAST_MUTEX(m) &m

#define mrfstr_create_thread(f) \
    if (pthread_create(threads + i, NULL, f, data))

#define mrfstr_thread_priority                                  \
    do                                                          \
    {                                                           \
        if (_mrfstr_config.tprior)                              \
        {                                                       \
            int policy;                                         \
            struct sched_param param;                           \
                                                                \
            pthread_getschedparam(threads[i], &policy, &param); \
            param.sched_priority = _mrfstr_config.tprior;       \
            pthread_setschedparam(threads[i], policy, &param);  \
        }                                                       \
    } while (0)

#define mrfstr_close_threads \
    while (i)                \
        pthread_join(threads[--i], NULL)

#define mrfstr_create_mutex(m) \
    if (pthread_mutex_init(&m, NULL))

#define mrfstr_close_mutex(m) pthread_mutex_destroy(&m)
#define mrfstr_lock_mutex(m) pthread_mutex_lock(m)
#define mrfstr_unlock_mutex(m) pthread_mutex_unlock(m)

#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef HANDLE mrfstr_thread_t;
typedef HANDLE mrfstr_mutex_t;
typedef HANDLE mrfstr_mutex_p;

#define MRFSTR_TFUNC_RET TRUE
#define MRFSTR_CAST_MUTEX(m) m

#define mrfstr_create_thread(f)                           \
    threads[i] = CreateThread(NULL, 0, f, data, 0, NULL); \
    if (!threads[i])

#define mrfstr_thread_priority                                    \
    do                                                            \
    {                                                             \
        if (_mrfstr_config.tprior != THREAD_PRIORITY_NORMAL)      \
            SetThreadPriority(threads[i], _mrfstr_config.tprior); \
    } while (0)

#define mrfstr_close_threads                                \
    do                                                      \
    {                                                       \
        WaitForMultipleObjects(i, threads, TRUE, INFINITE); \
        while (i)                                           \
            CloseHandle(threads[--i]);                      \
    } while (0)

#define mrfstr_create_mutex(m)          \
    m = CreateMutex(NULL, FALSE, NULL); \
    if (!m)

#define mrfstr_close_mutex CloseHandle
#define mrfstr_lock_mutex(m) WaitForSingleObject(m, INFINITE)
#define mrfstr_unlock_mutex ReleaseMutex

#else
#error Your os is not yet supported by MRFStr library
#endif

void __mrfstr_copy(
    mrfstr_data_t dst, mrfstr_data_ct src,
    mrfstr_size_t size);
void __mrfstr_move(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size);
void __mrfstr_rmove(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size);

void __mrfstr_fill(
    mrfstr_data_t res, mrfstr_chr_t chr, mrfstr_size_t size);

void __mrfstr_rev(
    mrfstr_data_t str, mrfstr_size_t size);
void __mrfstr_rev2(
    mrfstr_data_t left, mrfstr_data_ct right,
    mrfstr_size_t size);

void __mrfstr_replchr(
    mrfstr_data_t str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size);
void __mrfstr_replchr2(
    mrfstr_data_t res, mrfstr_data_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size);

mrfstr_bool_t __mrfstr_equal(
    mrfstr_data_ct str1, mrfstr_data_ct str2, mrfstr_size_t size);

mrfstr_bool_t __mrfstr_contchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
mrfstr_idx_t __mrfstr_findchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
mrfstr_size_t __mrfstr_countchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

#endif
