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

#define MRFSTR_TLIB_CONFIG MRFSTR_CONFIG_FUNC_REPLACE_CHR

#define MRFSTR_TLIB_OBJ(size)                          \
    do                                                 \
    {                                                  \
        MRFSTR_TLIB_MEMSET(res1, '0', size);           \
        MRFSTR_SIZE(res1) = size;                      \
                                                       \
        if (first)                                     \
            mrfstr_replace_chr(res1, res1, '0', '1');  \
        else                                           \
            mrfstr_replace_chr(res2, res1, '0', '1');  \
                                                       \
        if (first)                                     \
            obj = MRFSTR_TLIB_MEMCMP(res1, str, size); \
        else                                           \
            obj = MRFSTR_TLIB_MEMCMP(res2, str, size); \
    } while (0)

#define MRFSTR_TLIB_FREE   \
    do                     \
    {                      \
        mrfstr_free(res1); \
        mrfstr_free(res2); \
        free(str);         \
    } while (0)

int main(void)
{
    mrfstr_t res1, res2;
    mrfstr_data_t str;
    mrfstr_bool_t first;

    mrfstr_config(MRFSTR_CONFIG_LEVEL_EXTREME);
    MRFSTR_TLIB_INIT(res1,);
    MRFSTR_TLIB_INIT(res2, mrfstr_free(res1));
    MRFSTR_TLIB_INIT_STR(str, mrfstr_free(res1); mrfstr_free(res2));
    memset(str, '1', MRFSTR_TEST4_SIZE);

    if (mrfstr_config_func(MRFSTR_TLIB_CONFIG,
            MRFSTR_CONFIG_SIMD_AVX512, MRFSTR_CONFIG_SIMD_AVX512) == MRFSTR_RES_NOERROR)
    {
        first = MRFSTR_TRUE;
        MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);
        first = MRFSTR_FALSE;
        MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);
    }

    if (mrfstr_config_func(MRFSTR_TLIB_CONFIG,
            MRFSTR_CONFIG_SIMD_AVX, MRFSTR_CONFIG_SIMD_AVX) == MRFSTR_RES_NOERROR)
    {
        first = MRFSTR_TRUE;
        MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);
        first = MRFSTR_FALSE;
        MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);
    }

    if (mrfstr_config_func(MRFSTR_TLIB_CONFIG,
            MRFSTR_CONFIG_SIMD_SSE, MRFSTR_CONFIG_SIMD_SSE) == MRFSTR_RES_NOERROR)
    {
        first = MRFSTR_TRUE;
        MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);
        first = MRFSTR_FALSE;
        MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
        MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);
    }

    mrfstr_config_func(MRFSTR_TLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_INT64, MRFSTR_CONFIG_SIMD_INT64);

    first = MRFSTR_TRUE;
    MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
    MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
    MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
    MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);
    first = MRFSTR_FALSE;
    MRFSTR_TLIB_ROUND(MRFSTR_TEST1_SIZE);
    MRFSTR_TLIB_ROUND(MRFSTR_TEST2_SIZE);
    MRFSTR_TLIB_ROUND(MRFSTR_TEST3_SIZE);
    MRFSTR_TLIB_ROUND(MRFSTR_TEST4_SIZE);

    MRFSTR_TLIB_FREE;
    return EXIT_SUCCESS;
}
