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

#define MRFSTR_BLIB_CONFIG MRFSTR_CONFIG_FUNC_COMPARE

#define MRFSTR_BLIB_CSTR_PRE(size) mrfstr_bool_t res
#define MRFSTR_BLIB_CSTR(size) res = (mrfstr_bool_t)memcmp(cstr1, cstr2, size)
#define MRFSTR_BLIB_CSTR_POST(size) _count += res

#define MRFSTR_BLIB_PRE(size)     \
    do                            \
    {                             \
        MRFSTR_SIZE(str1) = size; \
        MRFSTR_SIZE(str2) = size; \
    } while (0)
#define MRFSTR_BLIB_OBJ(size) mrfstr_equal(str1, str2)
#define MRFSTR_BLIB_POST(size)

int main(
    int argc, mrfstr_data_ct *argv)
{
    mrfstr_t str1, str2;
    mrfstr_data_t cstr1, cstr2;
    MRFSTR_BLIB_VARS;

    MRFSTR_BLIB_ARGS;
    MRFSTR_BLIB_FIRST;
    MRFSTR_BLIB_INIT_STR(cstr1,);
    MRFSTR_BLIB_INIT_STR(cstr2, free(cstr1));
    memset(cstr1, '0', end);
    memset(cstr2, '0', end);

    MRFSTR_BLIB_ROUND_CSTR;
    free(cstr1);
    free(cstr2);

    MRFSTR_BLIB_INIT(str1,);
    MRFSTR_BLIB_INIT(str2, mrfstr_free(str1));
    mrfstr_repeat_chr(str1, '0', end);
    mrfstr_repeat_chr(str2, '0', end);

    if (mrfstr_config_func(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_AVX512, MRFSTR_CONFIG_SIMD_AVX512) == MRFSTR_RES_NOERROR)
        MRFSTR_BLIB_ROUND("AVX512");
    if (mrfstr_config_func(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_AVX, MRFSTR_CONFIG_SIMD_AVX) == MRFSTR_RES_NOERROR)
        MRFSTR_BLIB_ROUND("AVX   ");
    if (mrfstr_config_func(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_SSE, MRFSTR_CONFIG_SIMD_SSE) == MRFSTR_RES_NOERROR)
        MRFSTR_BLIB_ROUND("SSE   ");

    mrfstr_config_func(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_INT64, MRFSTR_CONFIG_SIMD_INT64);
    MRFSTR_BLIB_ROUND("INT64 ");

    mrfstr_free(str1);
    mrfstr_free(str2);
    MRFSTR_BLIB_RETURN;
}
