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

#define MRFSTR_BLIB_CONFIG MRFSTR_CONFIG_FUNC_MEMORY

#define MRFSTR_BLIB_CSTR_PRE(size)
#define MRFSTR_BLIB_CSTR(size) memcpy(cdst, csrc, size)
#define MRFSTR_BLIB_CSTR_POST(size)

#define MRFSTR_BLIB_PRE(size) MRFSTR_SIZE(src) = size
#define MRFSTR_BLIB_OBJ(size) mrfstr_set(dst, src)
#define MRFSTR_BLIB_POST(size)

int main(
    int argc, mrfstr_data_ct *argv)
{
    mrfstr_t src, dst;
    mrfstr_data_t csrc, cdst;
    MRFSTR_BLIB_VARS;

    MRFSTR_BLIB_ARGS;
    MRFSTR_BLIB_FIRST;
    MRFSTR_BLIB_INIT_STR(csrc,);
    MRFSTR_BLIB_INIT_STR(cdst, free(csrc));
    memset(csrc, '0', end);

    MRFSTR_BLIB_ROUND_CSTR;
    free(csrc);
    free(cdst);

    MRFSTR_BLIB_INIT(src,);
    MRFSTR_BLIB_INIT(dst, mrfstr_free(src));
    mrfstr_repeat_chr(src, '0', end);

    if (mrfstr_config_func(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_AVX512, MRFSTR_CONFIG_SIMD_AVX512) == MRFSTR_RES_NOERROR)
        MRFSTR_BLIB_ROUND("AVX512");
    if (mrfstr_config_func(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_AVX, MRFSTR_CONFIG_SIMD_AVX) == MRFSTR_RES_NOERROR)
        MRFSTR_BLIB_ROUND("AVX   ");
    if (mrfstr_config_func(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_SSE, MRFSTR_CONFIG_SIMD_SSE) == MRFSTR_RES_NOERROR)
        MRFSTR_BLIB_ROUND("SSE   ");

    mrfstr_config_func(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_INT64, MRFSTR_CONFIG_SIMD_INT64);
    MRFSTR_BLIB_ROUND("INT64 ");

    mrfstr_free(src);
    mrfstr_free(dst);
    MRFSTR_BLIB_RETURN;
}
