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

#include "blib.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define MRFSTR_BLIB_CONFIG MRFSTR_CONFIG_TYPE_COMPARE

#define MRFSTR_BLIB_CSTR(size) cstr[i] = memcmp(cstr1, cstr2, size)

#define MRFSTR_BLIB_PRE(size)
#define MRFSTR_BLIB_OBJ(size) mrfstr_n_equal(str1, str2, size)

int main(void)
{
    mrfstr_t str1, str2;
    mrfstr_data_t cstr1, cstr2;
    LARGE_INTEGER freq;
    mrfstr_double_t cstr[7];

    MRFSTR_BLIB_FIRST;

    MRFSTR_BLIB_INIT_STR(cstr1,);
    MRFSTR_BLIB_INIT_STR(cstr2, free(cstr1));
    memset(cstr1, '0', mrfstr_test_sizes[6]);
    memset(cstr2, '0', mrfstr_test_sizes[6]);

    MRFSTR_BLIB_ROUND_CSTR;
    free(cstr1);
    free(cstr2);

    MRFSTR_BLIB_INIT(str1,);
    MRFSTR_BLIB_INIT(str2, mrfstr_free(str1));
    mrfstr_repeat_chr(str1, '0', mrfstr_test_sizes[6]);
    mrfstr_repeat_chr(str2, '0', mrfstr_test_sizes[6]);

    mrfstr_config(MRFSTR_BLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_AVX512, MRFSTR_CONFIG_SIMD_AVX512);
    MRFSTR_BLIB_ROUND("AVX512");

    mrfstr_config(MRFSTR_BLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_AVX, MRFSTR_CONFIG_SIMD_AVX);
    MRFSTR_BLIB_ROUND("AVX   ");

    mrfstr_config(MRFSTR_BLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_SSE, MRFSTR_CONFIG_SIMD_SSE);
    MRFSTR_BLIB_ROUND("SSE   ");

    mrfstr_config(MRFSTR_BLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_NONE, MRFSTR_CONFIG_SIMD_NONE);
    MRFSTR_BLIB_ROUND("INT64 ");

    mrfstr_free(str1);
    mrfstr_free(str2);
    return 0;
}