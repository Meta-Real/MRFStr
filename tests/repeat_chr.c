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

#include "tlib.h"

#define MRFSTR_TLIB_CONFIG MRFSTR_CONFIG_TYPE_MEMORY

#define MRFSTR_TLIB_EXPR(size) \
    mrfstr_repeat_chr(res, '0', size)

#define MRFSTR_TLIB_OBJ(size) \
    obj = MRFSTR_TLIB_MEMCMP(res, str, size)

#define MRFSTR_TLIB_FREE  \
    do                    \
    {                     \
        mrfstr_free(res); \
        free(str);        \
    } while (0)

int main(void)
{
    mrfstr_config_tcount(5);

    mrfstr_t res;
    MRFSTR_TLIB_INIT(res,);

    mrfstr_data_t str;
    MRFSTR_TLIB_INIT_STR(str, mrfstr_free(res));
    memset(str, '0', TEST4_SIZE);

    mrfstr_config(MRFSTR_TLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_AVX512, MRFSTR_CONFIG_SIMD_AVX512);

    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);

    mrfstr_config(MRFSTR_TLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_AVX, MRFSTR_CONFIG_SIMD_AVX);

    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);

    mrfstr_config(MRFSTR_TLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_SSE, MRFSTR_CONFIG_SIMD_SSE);

    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);

    mrfstr_config(MRFSTR_TLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_NONE, MRFSTR_CONFIG_SIMD_NONE);

    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);

    MRFSTR_TLIB_FREE;
    return EXIT_SUCCESS;
}
