/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#ifndef __MR_AVX512__
#define __MR_AVX512__

#ifdef _MSC_VER
#include <simddef.h>
#endif

#ifdef __AVX512F__
#include <immintrin.h>
#include <mrfstr.h>

typedef __m512i mrfstr_avx512_t;

mrfstr_ptr_t mrfstr_avx512_memcpy_sub(mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);

#endif

#endif
