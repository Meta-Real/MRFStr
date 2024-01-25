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

#ifndef __MRFSTR_TLIB__
#define __MRFSTR_TLIB__

#include <mrfstr.h>
#include <string.h>
#include <stdlib.h>

#define TEST1_SIZE 0x100
#define TEST2_SIZE 0x100000
#define TEST3_SIZE 0x1000000
#define TEST4_SIZE 0x10000000

#define MRFSTR_TLIB_ERROR                                     \
    do                                                        \
    {                                                         \
        fputs("Test Error in File \"" __FILE__ "\"", stderr); \
        fprintf(stderr, ", Line %d\n", __LINE__);             \
        return EXIT_FAILURE;                                  \
    } while (0)

#define MRFSTR_TLIB_INIT(x, fr)          \
    do                                   \
    {                                    \
        x = mrfstr_init();               \
        if (!x)                          \
        {                                \
            fr;                          \
            MRFSTR_TLIB_ERROR;           \
        }                                \
                                         \
        if (mrfstr_alloc(x, TEST4_SIZE)) \
        {                                \
            mrfstr_free(x);              \
            fr;                          \
            MRFSTR_TLIB_ERROR;           \
        }                                \
    } while (0)

#define MRFSTR_TLIB_INIT_STR(x, fr)                    \
    do                                                 \
    {                                                  \
        x = malloc(TEST4_SIZE * sizeof(mrfstr_chr_t)); \
        if (!x)                                        \
        {                                              \
            fr;                                        \
            MRFSTR_TLIB_ERROR;                         \
        }                                              \
    } while (0)

#define MRFSTR_TLIB_ROUND(size)           \
    do                                    \
    {                                     \
        MRFSTR_TLIB_EXPR(size);           \
                                          \
        mrfstr_bool_t obj = MRFSTR_FALSE; \
        MRFSTR_TLIB_OBJ(size);            \
        if (!obj)                         \
        {                                 \
            MRFSTR_TLIB_FREE;             \
            MRFSTR_TLIB_ERROR;            \
        }                                 \
    } while (0)

#define MRFSTR_TLIB_MEMCPY(x, str, size) \
    memcpy(MRFSTR_DATA(x), str, size)

#define MRFSTR_TLIB_MEMSET(x, chr, size) \
    memset(MRFSTR_DATA(x), chr, size)

#define MRFSTR_TLIB_MEMCMP(x, str, size) \
    (MRFSTR_SIZE(x) == size && !memcmp(MRFSTR_DATA(x), str, size))

#endif
