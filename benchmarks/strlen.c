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

#define MRFSTR_BLIB_CONFIG MRFSTR_CONFIG_TYPE_STRLEN

#define MRFSTR_BLIB_CSTR_PRE(size)  \
    mrfstr_size_t len;              \
                                    \
    str[size - 1] = '\0'
#define MRFSTR_BLIB_CSTR(size) len = strlen(str)
#define MRFSTR_BLIB_CSTR_POST(size) \
    _count += len - size + 1;       \
    str[size - 1] = '0'

#define MRFSTR_BLIB_PRE(size)       \
    mrfstr_size_t len;              \
                                    \
    str[size - 1] = '\0'
#define MRFSTR_BLIB_OBJ(size) len = mrfstr_strlen(str)
#define MRFSTR_BLIB_POST(size) \
    _count += len - size + 1;  \
    str[size - 1] = '0'

int main(int argc, mrfstr_data_ct *argv)
{
    mrfstr_data_t str;
    MRFSTR_BLIB_VARS;

    MRFSTR_BLIB_ARGS;
    MRFSTR_BLIB_FIRST;

    MRFSTR_BLIB_INIT_STR(str,);
    memset(str, '0', tests[nsec - 1].size);

    MRFSTR_BLIB_ROUND_CSTR;

#ifdef __AVX512F__
    mrfstr_config_str(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_AVX512);
    MRFSTR_BLIB_ROUND("AVX512");
#endif

#ifdef __AVX__
    mrfstr_config_str(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_AVX);
    MRFSTR_BLIB_ROUND("AVX   ");
#endif

#ifdef __SSE2__
    mrfstr_config_str(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_SSE);
    MRFSTR_BLIB_ROUND("SSE   ");
#endif

    mrfstr_config_str(MRFSTR_BLIB_CONFIG, MRFSTR_CONFIG_SIMD_NONE);
    MRFSTR_BLIB_ROUND("INT64 ");

    free(str);
    MRFSTR_BLIB_RETURN;
}
