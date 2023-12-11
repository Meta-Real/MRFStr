/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#ifndef __MRTSTR_INTERN__
#define __MRTSTR_INTERN__

#include <mrtstr.h>
#include <immintrin.h>

#ifdef __AVX512F__
#define MRTSTR_SIMD_SIZE 64
#elif defined(__AVX__)
#define MRTSTR_SIMD_SIZE 32
#elif defined(__SSE2__)
#define MRTSTR_SIMD_SIZE 16
#else
#define MRTSTR_SIMD_SIZE 8
#endif

#define MRTSTR_SIMD_TCHK (MRTSTR_SIMD_SIZE * MRTSTR_THREAD_COUNT)
#define MRTSTR_SIMD_TLIMIT (0x10000 * MRTSTR_SIMD_TCHK - 1)

#define mrtstr_lock_inc(l, m)    \
    do                           \
    {                            \
        pthread_mutex_lock(m);   \
        (l)++;                   \
        pthread_mutex_unlock(m); \
    } while (0)

#define mrtstr_lock_dec(l, m)    \
    do                           \
    {                            \
        pthread_mutex_lock(m);   \
        (l)--;                   \
        pthread_mutex_unlock(m); \
    } while (0)

void mrtstr_load_threads(void (*func)(void*), void *args);

void mrtstr_memcpy(mrtstr_t dst, mrtstr_ct src, mrtstr_size_t size);
void mrtstr_memcpy2(mrtstr_t dst, mrtstr_data_ct src, mrtstr_size_t size);

void mrtstr_memcmp(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_ct str2, mrtstr_size_t size);
void mrtstr_memcmp2(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2, mrtstr_size_t size);

void mrtstr_memset(mrtstr_t str, mrtstr_chr_t chr, mrtstr_size_t size);

void mrtstr_memchr(mrtstr_bres_t *res, mrtstr_ct str, mrtstr_chr_t chr, mrtstr_size_t size);

#endif /* __MRTSTR_INTERN__ */
