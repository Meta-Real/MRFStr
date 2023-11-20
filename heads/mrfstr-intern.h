/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#ifndef __MRFSTR_INTERN__
#define __MRFSTR_INTERN__

#include <mrfstr.h>
#include <immintrin.h>

#ifdef __AVX512BW__

typedef __m512i mrfstr_simd_block_t;

#define MRFSTR_SIMD_OFF 64
#define MRFSTR_SIMD_MASK 63
#define MRFSTR_SIMD_SHIFT 6

#if MRFSTR_THREADING
#define MRFSTR_THREAD_LIMIT (4194368 * MRFSTR_THREAD_COUNT - 1)
#define MRFSTR_THREAD_CHUNK (MRFSTR_SIMD_OFF * MRFSTR_THREAD_COUNT)
#endif

#define mrfstr_simd_set1_func _mm512_set1_epi8
#define mrfstr_simd_stream_func _mm512_stream_si512
#define mrfstr_simd_cmpneq_func _mm512_cmpneq_epi64_mask
#define mrfstr_simd_cmpeq_mask_func _mm512_cmpeq_epi8_mask

#else
#ifdef __AVX2__

typedef __m256i mrfstr_simd_block_t;

#define MRFSTR_SIMD_OFF 32
#define MRFSTR_SIMD_MASK 31
#define MRFSTR_SIMD_SHIFT 5

#if MRFSTR_THREADING
#define MRFSTR_THREAD_LIMIT (2097184 * MRFSTR_THREAD_COUNT - 1)
#define MRFSTR_THREAD_CHUNK (MRFSTR_SIMD_OFF * MRFSTR_THREAD_COUNT)
#endif

#define mrfstr_simd_set1_func _mm256_set1_epi8
#define mrfstr_simd_stream_func _mm256_stream_si256
#define mrfstr_simd_cmpneq_func(x, y) (~_mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y)))
#define mrfstr_simd_cmpeq_mask_func(x, y) _mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y))

#else

typedef __m128i mrfstr_simd_block_t;

#define MRFSTR_SIMD_OFF 16
#define MRFSTR_SIMD_MASK 15
#define MRFSTR_SIMD_SHIFT 4

#if MRFSTR_THREADING
#define MRFSTR_THREAD_LIMIT (1048592 * MRFSTR_THREAD_COUNT - 1)
#define MRFSTR_THREAD_CHUNK (MRFSTR_SIMD_OFF * MRFSTR_THREAD_COUNT)
#endif

#define mrfstr_simd_set1_func _mm_set1_epi8
#define mrfstr_simd_stream_func _mm_stream_si128
#define mrfstr_simd_cmpneq_func(x, y) (~_mm_movemask_epi8(_mm_cmpeq_epi8(x, y)))
#define mrfstr_simd_cmpeq_mask_func(x, y) _mm_movemask_epi8(_mm_cmpeq_epi8(x, y))

#endif
#endif

#endif /* __MRFSTR_INTERN__ */
