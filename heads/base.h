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

#ifndef __MRFSTR_BASE__
#define __MRFSTR_BASE__

#include <mrfstr.h>

#define MRFSTR_BASE_TEQUAL_LOAD 0x80000
#define MRFSTR_BASE_TCONTCHR_LOAD 0x80000

void __mrfstr_base_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size);
void __mrfstr_base_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right,
    mrfstr_size_t size);

void __mrfstr_base_replchr(
    mrfstr_ptr_t str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size);
void __mrfstr_base_replchr2(
    mrfstr_ptr_t res, mrfstr_ptr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size);

mrfstr_bool_t __mrfstr_base_equal(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size);
void __mrfstr_base_tequal(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size);

mrfstr_bool_t __mrfstr_base_contchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
void __mrfstr_base_tcontchr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

mrfstr_idx_t __mrfstr_base_findchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
mrfstr_idx_t __mrfstr_base_tfindchr(
    volatile mrfstr_idx_t *res, mrfstr_size_t start,
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_short_t step);

mrfstr_size_t __mrfstr_base_countchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

mrfstr_size_t __mrfstr_base_strlen(
    mrfstr_data_ct str);

#endif
