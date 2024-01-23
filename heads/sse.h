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

#ifndef __MRFSTR_SSE__
#define __MRFSTR_SSE__

#ifdef _MSC_VER
#include <simddef.h>
#endif

#ifdef __SSE2__
#include <immintrin.h>
#include <mrfstr.h>

#define MRFSTR_SSE_TEQUAL_LOAD 0x40000
#define MRFSTR_SSE_TCONTCHR_LOAD 0x40000
#define MRFSTR_SSE_TFINDCHR_LOAD 0x40000

void __mrfstr_sse_bcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src,
    mrfstr_size_t size);
void __mrfstr_sse_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src,
    mrfstr_size_t size);

void __mrfstr_sse_brcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_sse_rcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);

void __mrfstr_sse_bfill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size);
void __mrfstr_sse_fill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size);

#ifdef __SSSE3__
void __mrfstr_sse_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size);
void __mrfstr_sse_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right,
    mrfstr_size_t size);
#endif

#ifdef __SSE4_1__
void __mrfstr_sse_replchr(
    mrfstr_ptr_t str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size);
void __mrfstr_sse_replchr2(
    mrfstr_ptr_t res, mrfstr_ptr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size);
#endif

mrfstr_bool_t __mrfstr_sse_equal(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size);
void __mrfstr_sse_tequal(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size);

mrfstr_bool_t __mrfstr_sse_contchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
void __mrfstr_sse_tcontchr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

mrfstr_idx_t __mrfstr_sse_findchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
mrfstr_idx_t __mrfstr_sse_tfindchr(
    volatile mrfstr_idx_t *res, mrfstr_idx_t start,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

mrfstr_size_t __mrfstr_sse_strlen(
    mrfstr_ptr_ct str);

#endif
#endif
