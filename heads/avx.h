/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#ifndef __MR_AVX__
#define __MR_AVX__

#ifdef _MSC_VER
#include <simddef.h>
#endif

#ifdef __AVX__
#include <immintrin.h>
#include <mrfstr.h>

void mrfstr_avx_copy_sub(mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void mrfstr_avx_fill_sub(mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size);

#endif

#endif
