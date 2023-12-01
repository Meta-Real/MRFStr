/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#ifndef __MRFSTR_INTERN__
#define __MRFSTR_INTERN__

#include <mrfstr.h>
#include <immintrin.h>

#ifdef __AVX512F__
#define MRFSTR_SIMD_SIZE 64
#elif defined(__AVX__)
#define MRFSTR_SIMD_SIZE 32
#elif defined(__SSE2__)
#define MRFSTR_SIMD_SIZE 16
#else
#define MRFSTR_SIMD_SIZE 8
#endif

void mrfstr_memcpy(mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size);
mrfstr_bool_t mrfstr_memcmp(mrfstr_data_ct str1, mrfstr_data_ct str2, mrfstr_size_t size);
mrfstr_bool_t mrfstr_memchr(mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
void mrfstr_memset(mrfstr_data_t res, mrfstr_chr_t chr, mrfstr_size_t size);

#endif /* __MRFSTR_INTERN__ */
