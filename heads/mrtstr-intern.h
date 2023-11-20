/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#ifndef __MRTSTR_INTERN__
#define __MRTSTR_INTERN__

#include <mrtstr.h>
#include <immintrin.h>

#ifdef __AVX512BW__

typedef __m512i mrtstr_simd_block_t;

#define MRTSTR_SIMD_OFF 64
#define MRTSTR_SIMD_MASK 63
#define MRTSTR_SIMD_SHIFT 6

#define MRTSTR_THREAD_LIMIT (4194368 * MRTSTR_THREAD_COUNT - 1)
#define MRTSTR_THREAD_CHUNK (MRTSTR_SIMD_OFF * MRTSTR_THREAD_COUNT)

#define mrtstr_simd_set1_func _mm512_set1_epi8
#define mrtstr_simd_stream_func _mm512_stream_si512
#define mrtstr_simd_cmpneq_func _mm512_cmpneq_epi64_mask
#define mrtstr_simd_cmpeq_mask_func _mm512_cmpeq_epi8_mask

#else
#ifdef __AVX2__

typedef __m256i mrtstr_simd_block_t;

#define MRTSTR_SIMD_OFF 32
#define MRTSTR_SIMD_MASK 31
#define MRTSTR_SIMD_SHIFT 5

#define MRTSTR_THREAD_LIMIT (2097184 * MRTSTR_THREAD_COUNT - 1)
#define MRTSTR_THREAD_CHUNK (MRTSTR_SIMD_OFF * MRTSTR_THREAD_COUNT)

#define mrtstr_simd_set1_func _mm256_set1_epi8
#define mrtstr_simd_stream_func _mm256_stream_si256
#define mrtstr_simd_cmpneq_func(x, y) (~_mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y)))
#define mrtstr_simd_cmpeq_mask_func(x, y) _mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y))

#else

typedef __m128i mrtstr_simd_block_t;

#define MRTSTR_SIMD_OFF 16
#define MRTSTR_SIMD_MASK 15
#define MRTSTR_SIMD_SHIFT 4

#define MRTSTR_THREAD_LIMIT (1048592 * MRTSTR_THREAD_COUNT - 1)
#define MRTSTR_THREAD_CHUNK (MRTSTR_SIMD_OFF * MRTSTR_THREAD_COUNT)

#define mrtstr_simd_set1_func _mm_set1_epi8
#define mrtstr_simd_stream_func _mm_stream_si128
#define mrtstr_simd_cmpneq_func(x, y) (~_mm_movemask_epi8(_mm_cmpeq_epi8(x, y)))
#define mrtstr_simd_cmpeq_mask_func(x, y) _mm_movemask_epi8(_mm_cmpeq_epi8(x, y))

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

mrtstr_bool_t mrtstr_locked(mrtstr_ct str);

#endif /* __MRTSTR_INTERN__ */
