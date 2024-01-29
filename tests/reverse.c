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

#define MRFSTR_TLIB_CONFIG MRFSTR_CONFIG_TYPE_REVERSE

#define MRFSTR_TLIB_OBJ(size)                           \
    do                                                  \
    {                                                   \
        MRFSTR_TLIB_MEMCPY(str1, str, size);            \
        MRFSTR_SIZE(str1) = size;                       \
                                                        \
        if (first)                                      \
            mrfstr_reverse(str1, str1);                 \
        else                                            \
            mrfstr_reverse(str2, str1);                 \
                                                        \
        if (first)                                      \
            obj = MRFSTR_TLIB_MEMCMP(str1, strr, size); \
        else                                            \
            obj = MRFSTR_TLIB_MEMCMP(str2, strr, size); \
    } while (0)

#define MRFSTR_TLIB_FREE   \
    do                     \
    {                      \
        mrfstr_free(str1); \
        mrfstr_free(str2); \
        free(str);         \
        free(strr);        \
    } while (0)

int main(void)
{
    mrfstr_t str1, str2;
    mrfstr_data_t str, strr, ptr;
    mrfstr_size_t i;
    mrfstr_short_t j;
    mrfstr_bool_t first;

    mrfstr_config_thread_count(5);

    MRFSTR_TLIB_INIT(str1,);
    MRFSTR_TLIB_INIT(str2, mrfstr_free(str1));
    MRFSTR_TLIB_INIT_STR(str,
        mrfstr_free(str1); mrfstr_free(str2));
    MRFSTR_TLIB_INIT_STR(strr,
        mrfstr_free(str1); mrfstr_free(str2); free(str));

    ptr = str;
    for (i = 0; i != TEST4_SIZE; i += 0x100)
        for (j = 0; j != 0x100; j++)
            *ptr++ = (mrfstr_chr_t)j;

    ptr = strr;
    for (i = 0; i != TEST4_SIZE; i += 0x100)
        for (j = 0xff; j != 0xffff; j--)
            *ptr++ = (mrfstr_chr_t)j;

    mrfstr_config(MRFSTR_TLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_AVX512, MRFSTR_CONFIG_SIMD_AVX512);

    first = MRFSTR_TRUE;
    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);
    first = MRFSTR_FALSE;
    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);

    mrfstr_config(MRFSTR_TLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_AVX, MRFSTR_CONFIG_SIMD_AVX);

    first = MRFSTR_TRUE;
    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);
    first = MRFSTR_FALSE;
    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);

    mrfstr_config(MRFSTR_TLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_SSE, MRFSTR_CONFIG_SIMD_SSE);

    first = MRFSTR_TRUE;
    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);
    first = MRFSTR_FALSE;
    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);

    mrfstr_config(MRFSTR_TLIB_CONFIG,
        MRFSTR_CONFIG_SIMD_NONE, MRFSTR_CONFIG_SIMD_NONE);

    first = MRFSTR_TRUE;
    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);
    first = MRFSTR_FALSE;
    MRFSTR_TLIB_ROUND(TEST1_SIZE);
    MRFSTR_TLIB_ROUND(TEST2_SIZE);
    MRFSTR_TLIB_ROUND(TEST3_SIZE);
    MRFSTR_TLIB_ROUND(TEST4_SIZE);

    MRFSTR_TLIB_FREE;
    return EXIT_SUCCESS;
}
