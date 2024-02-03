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

#define MRFSTR_BLIB_CONFIG MRFSTR_CONFIG_TYPE_MEMORY

#define MRFSTR_BLIB_CSTR_PRE(size)
#define MRFSTR_BLIB_CSTR(size)               \
    do                                       \
    {                                        \
        mrfstr_size_t i;                     \
                                             \
        for (i = 0; i < size; i += 8)        \
            memcpy(cstr + i, "00000000", 8); \
    } while (0)
#define MRFSTR_BLIB_CSTR_POST(size)

#define MRFSTR_BLIB_PRE(size)            \
    mrfstr_size_t count;                 \
                                         \
    mrfstr_set_nstr(str, "00000000", 8); \
    count = size / 8
#define MRFSTR_BLIB_OBJ(size) mrfstr_repeat(str, str, count)
#define MRFSTR_BLIB_POST(size)

int main(int argc, mrfstr_data_ct *argv)
{
    mrfstr_t str;
    mrfstr_data_t cstr;
    MRFSTR_BLIB_VARS;

    MRFSTR_BLIB_ARGS;
    MRFSTR_BLIB_FIRST;

    MRFSTR_BLIB_INIT_STR(cstr,);

    MRFSTR_BLIB_ROUND_CSTR;
    free(cstr);

    MRFSTR_BLIB_INIT(str,);

#ifdef __AVX512F__
    mrfstr_config(MRFSTR_BLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_AVX512, MRFSTR_CONFIG_SIMD_AVX512);
    MRFSTR_BLIB_ROUND("AVX512");
#endif

#ifdef __AVX__
    mrfstr_config(MRFSTR_BLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_AVX, MRFSTR_CONFIG_SIMD_AVX);
    MRFSTR_BLIB_ROUND("AVX   ");
#endif

#ifdef __SSE2__
    mrfstr_config(MRFSTR_BLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_SSE, MRFSTR_CONFIG_SIMD_SSE);
    MRFSTR_BLIB_ROUND("SSE   ");
#endif

    mrfstr_config(MRFSTR_BLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_NONE, MRFSTR_CONFIG_SIMD_NONE);
    MRFSTR_BLIB_ROUND("INT64 ");

    mrfstr_free(str);
    MRFSTR_BLIB_RETURN;
}
