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

#ifndef __MRFSTR_AVX512__
#define __MRFSTR_AVX512__

#ifdef _MSC_VER
#include <simddef.h>
#endif

#ifdef __AVX512F__
#include <immintrin.h>
#include <mrfstr.h>

#define MRFSTR_AVX512_TEQUAL_LOAD 0x10000
#define MRFSTR_AVX512_TCONTCHR_LOAD 0x10000

void __mrfstr_avx512_bcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src,
    mrfstr_size_t size);
void __mrfstr_avx512_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src,
    mrfstr_size_t size);

void __mrfstr_avx512_brcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);
void __mrfstr_avx512_rcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size);

void __mrfstr_avx512_bfill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size);
void __mrfstr_avx512_fill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size);

#ifdef __AVX512BW__
void __mrfstr_avx512_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size);
void __mrfstr_avx512_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right,
    mrfstr_size_t size);

void __mrfstr_avx512_replchr(
    mrfstr_ptr_t str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size);
void __mrfstr_avx512_replchr2(
    mrfstr_ptr_t res, mrfstr_ptr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size);
#endif

mrfstr_bool_t __mrfstr_avx512_equal(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size);
void __mrfstr_avx512_tequal(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size);

mrfstr_bool_t __mrfstr_avx512_contchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
void __mrfstr_avx512_tcontchr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

mrfstr_idx_t __mrfstr_avx512_findchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
mrfstr_idx_t __mrfstr_avx512_tfindchr(
    volatile mrfstr_idx_t *res, mrfstr_size_t start,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_short_t step);

mrfstr_size_t __mrfstr_avx512_countchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

mrfstr_size_t __mrfstr_avx512_strlen(
    mrfstr_ptr_ct str);

#endif
#endif
