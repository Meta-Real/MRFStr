/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#ifndef __MRFSTR_INTERN__
#define __MRFSTR_INTERN__

#include <mrfstr.h>

#ifdef _MSC_VER
#include <simddef.h>
#endif

#include <avx512.h>
#include <avx.h>
#include <sse.h>
#include <base.h>

#pragma pack(push, 1)
struct __MRFSTR_CONFIG_T
{
    mrfstr_byte_t tcount;

    /* memory functions */

    void (*ncopy_sub)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);
    void (*nfill_sub)(
        mrfstr_ptr_t, mrfstr_chr_t, mrfstr_size_t);
    mrfstr_byte_t nmem_size;

    void (*tcopy_sub)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);
    void (*tfill_sub)(
        mrfstr_ptr_t, mrfstr_chr_t, mrfstr_size_t);
    mrfstr_byte_t tmem_size;

    /* reverse function */

    void (*nrev_sub)(
        mrfstr_ptr_t, mrfstr_ptr_t, mrfstr_size_t);
    void (*nrev2_sub)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);
    mrfstr_byte_t nrev_size;

    void (*trev_sub)(
        mrfstr_ptr_t, mrfstr_ptr_t, mrfstr_size_t);
    void (*trev2_sub)(
        mrfstr_ptr_t, mrfstr_ptr_ct, mrfstr_size_t);
    mrfstr_byte_t trev_size;

    /* replace functions */

    void (*nreplchr_sub)(
        mrfstr_ptr_t,
        mrfstr_chr_t, mrfstr_chr_t,
        mrfstr_size_t);
    void (*nreplchr2_sub)(
        mrfstr_ptr_t, mrfstr_ptr_ct,
        mrfstr_chr_t, mrfstr_chr_t,
        mrfstr_size_t);
    mrfstr_byte_t nrepl_size;

    void (*treplchr_sub)(
        mrfstr_ptr_t,
        mrfstr_chr_t, mrfstr_chr_t,
        mrfstr_size_t);
    void (*treplchr2_sub)(
        mrfstr_ptr_t, mrfstr_ptr_ct,
        mrfstr_chr_t, mrfstr_chr_t,
        mrfstr_size_t);
    mrfstr_byte_t trepl_size;

    /* compare functions */

    mrfstr_bool_t (*nequal_sub)(
        mrfstr_ptr_ct, mrfstr_ptr_ct, mrfstr_size_t);
    mrfstr_byte_t ncmp_size;

    void (*tequal_sub)(
        volatile mrfstr_bool_t*,
        mrfstr_ptr_ct, mrfstr_ptr_ct, mrfstr_size_t);
    mrfstr_byte_t tcmp_size;

    /* search functions */

    mrfstr_bool_t (*ncontchr_sub)(
        mrfstr_ptr_ct, mrfstr_chr_t, mrfstr_size_t);
    mrfstr_idx_t (*nfindchr_sub)(
        mrfstr_ptr_ct, mrfstr_chr_t, mrfstr_size_t);
    mrfstr_byte_t nsearch_size;

    void (*tcontchr_sub)(
        volatile mrfstr_bool_t*,
        mrfstr_ptr_ct, mrfstr_chr_t, mrfstr_size_t);
    mrfstr_idx_t (*tfindchr_sub)(
        volatile mrfstr_idx_t*, mrfstr_idx_t,
        mrfstr_ptr_ct, mrfstr_chr_t, mrfstr_size_t);
    mrfstr_byte_t tsearch_size;

    /* str functions */

    mrfstr_size_t (*strlen_sub)(
        mrfstr_ptr_ct);
    mrfstr_byte_t strlen_size;
};
#pragma pack(pop)
typedef struct __MRFSTR_CONFIG_T mrfstr_config_t;
extern mrfstr_config_t _mrfstr_config;

#define MRFSTR_SLIMIT 0x800

#define MRFSTR_TSIZE 0x8000000ULL
#define MRFSTR_TLIMIT (MRFSTR_TSIZE << 1)

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <pthread.h>

typedef pthread_t mrfstr_thread_t;
typedef pthread_mutex_t mrfstr_mutex_t;
typedef mrfstr_mutex_t *mrfstr_mutex_p;

#define MRFSTR_TFUNC_RET NULL
#define MRFSTR_CAST_MUTEX(m) &m

#define mrfstr_create_thread(f) \
    if (pthread_create(threads + i, NULL, f, data))

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
#error Your os is not yet supported for multithreading
#endif

void __mrfstr_copy(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size);
void __mrfstr_fill(
    mrfstr_data_t res, mrfstr_chr_t chr, mrfstr_size_t size);

void __mrfstr_rev(
    mrfstr_data_t str, mrfstr_size_t size);
void __mrfstr_rev2(
    mrfstr_data_t left, mrfstr_data_ct right, mrfstr_size_t size);

void __mrfstr_replchr(
    mrfstr_data_t str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size);
void __mrfstr_replchr2(
    mrfstr_data_t res, mrfstr_data_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size);

mrfstr_bool_t __mrfstr_equal(
    mrfstr_data_ct str1, mrfstr_data_ct str2, mrfstr_size_t size);

mrfstr_bool_t __mrfstr_contchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
mrfstr_idx_t __mrfstr_findchr(
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

#endif
