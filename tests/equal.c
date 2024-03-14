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

#define MRFSTR_TLIB_CONFIG MRFSTR_CONFIG_FUNC_COMPARE

#define MRFSTR_TLIB_OBJ(size)           \
    do                                  \
    {                                   \
        MRFSTR_SIZE(str1) = size;       \
        MRFSTR_SIZE(str2) = size;       \
                                        \
        obj = mrfstr_equal(str1, str2); \
    } while (0)

#define MRFSTR_TLIB_FREE   \
    do                     \
    {                      \
        mrfstr_free(str1); \
        mrfstr_free(str2); \
    } while (0)

int main(void)
{
    mrfstr_t str1, str2;

    mrfstr_config(MRFSTR_CONFIG_LEVEL_EXTREME);

    MRFSTR_TLIB_INIT(str1,);
    MRFSTR_TLIB_INIT(str2, mrfstr_free(str1));

    if (mrfstr_config_func(MRFSTR_TLIB_CONFIG,
            MRFSTR_CONFIG_SIMD_AVX512, MRFSTR_CONFIG_SIMD_AVX512) == MRFSTR_RES_NOERROR)
    {
        MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);
    }

    if (mrfstr_config_func(MRFSTR_TLIB_CONFIG,
            MRFSTR_CONFIG_SIMD_AVX, MRFSTR_CONFIG_SIMD_AVX) == MRFSTR_RES_NOERROR)
    {
        MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);
    }

    if (mrfstr_config_func(MRFSTR_TLIB_CONFIG,
            MRFSTR_CONFIG_SIMD_SSE, MRFSTR_CONFIG_SIMD_SSE) == MRFSTR_RES_NOERROR)
    {
        MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);
    }

    mrfstr_config_func(MRFSTR_TLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_INT64, MRFSTR_CONFIG_SIMD_INT64);

    MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
    MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
    MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
    MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);

    MRFSTR_TLIB_FREE;
    return EXIT_SUCCESS;
}
