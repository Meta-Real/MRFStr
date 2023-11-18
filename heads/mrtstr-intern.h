/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#ifndef __MRTSTR_INTERN__
#define __MRTSTR_INTERN__

#include <mrtstr.h>
#include <immintrin.h>

#ifdef __AVX512F__

typedef __m512i mrtstr_simd_block_t;

#define MRTSTR_SIMD_CHAR_MASK 63
#define MRTSTR_SIMD_CHAR_SHIFT 6

#define MRTSTR_THREAD_LIMIT (4194368 * MRTSTR_THREAD_COUNT - 1)
#define MRTSTR_THREAD_CHUNK (64 * MRTSTR_THREAD_COUNT)

#define mrtstr_simd_stream_func _mm512_stream_si512
#define mrtstr_simd_cmpneq_func _mm512_cmpneq_epi64_mask

#else
#ifdef __AVX2__

typedef __m256i mrtstr_simd_block_t;

#define MRTSTR_SIMD_CHAR_MASK 31
#define MRTSTR_SIMD_CHAR_SHIFT 5

#define MRTSTR_THREAD_LIMIT (2097184 * MRTSTR_THREAD_COUNT - 1)
#define MRTSTR_THREAD_CHUNK (32 * MRTSTR_THREAD_COUNT)

#define mrtstr_simd_stream_func _mm256_stream_si256
#define mrtstr_simd_cmpneq_func _mm256_cmpneq_epi64_mask

#else

typedef __m128i mrtstr_simd_block_t;

#define MRTSTR_SIMD_CHAR_MASK 15
#define MRTSTR_SIMD_CHAR_SHIFT 4

#define MRTSTR_THREAD_LIMIT (1048592 * MRTSTR_THREAD_COUNT - 1)
#define MRTSTR_THREAD_CHUNK (16 * MRTSTR_THREAD_COUNT)

#define mrtstr_simd_stream_func _mm_stream_si128
#define mrtstr_simd_cmpneq_func _mm_cmpneq_epi64_mask

#endif
#endif

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

    mrtstr_bool_t
    mrtstr_locked(mrtstr_ct str);

#endif /* __MRTSTR_INTERN__ */
