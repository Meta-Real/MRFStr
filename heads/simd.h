/*
MIT License

Copyright (c) 2023 MetaReal

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/

#ifndef __MRFSTR_SIMD__
#define __MRFSTR_SIMD__

#include <mrfstr.h>

/* copy functions */

void __mrfstr_avx512f_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_avx512f_ntcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_avx512f_rcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_avx512f_ntrcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);

void __mrfstr_avx_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_avx_ntcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_avx_rcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_avx_ntrcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);

void __mrfstr_sse2_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_sse2_ntcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_sse2_rcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_sse2_ntrcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);

void __mrfstr_i64_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_i64_rcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);

/* fill functions */

void __mrfstr_avx512f_fill(
    mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size);
void __mrfstr_avx512f_ntfill(
    mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size);

void __mrfstr_avx_fill(
    mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size);
void __mrfstr_avx_ntfill(
    mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size);

void __mrfstr_avx2_fill(
    mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size);
void __mrfstr_avx2_ntfill(
    mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size);

void __mrfstr_sse2_fill(
    mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size);
void __mrfstr_sse2_ntfill(
    mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size);

void __mrfstr_i64_fill(
    mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size);

/* reverse functions */

void __mrfstr_avx512vbmi_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size);
void __mrfstr_avx512vbmi_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size);
void __mrfstr_avx512vbmi_ntrev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size);

void __mrfstr_avx512bw_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size);
void __mrfstr_avx512bw_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size);
void __mrfstr_avx512bw_ntrev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size);

void __mrfstr_avx2_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size);
void __mrfstr_avx2_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size);
void __mrfstr_avx2_ntrev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size);

void __mrfstr_ssse3_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size);
void __mrfstr_ssse3_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size);
void __mrfstr_ssse3_ntrev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size);

void __mrfstr_i64_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size);
void __mrfstr_i64_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size);

#endif
