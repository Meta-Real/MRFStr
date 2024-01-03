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

#define MRFSTR_AVX_TCMP_LOAD 0x20000

void mrfstr_avx_copy_sub(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void mrfstr_avx_fill_sub(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size);

#if defined(__AVX2__) || (defined(__AVX512F__) && defined(__AVX512VL__))
mrfstr_bool_t mrfstr_avx_cmp_sub(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size);
void mrfstr_avx_tcmp_sub(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size);
#endif

#endif

#endif
