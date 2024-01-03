/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#ifndef __MR_SSE__
#define __MR_SSE__

#ifdef _MSC_VER
#include <simddef.h>
#endif

#ifdef __SSE2__
#include <immintrin.h>
#include <mrfstr.h>

#define MRFSTR_SSE_TCMP_LOAD 0x40000

void mrfstr_sse_copy_sub(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void mrfstr_sse_fill_sub(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size);

mrfstr_bool_t mrfstr_sse_cmp_sub(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size);
void mrfstr_sse_tcmp_sub(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size);

#endif

#endif
