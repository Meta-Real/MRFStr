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

#ifndef __MRFSTR_BLIB__
#define __MRFSTR_BLIB__

#include <mrfstr.h>
#include <string.h>
#include <stdlib.h>

mrfstr_longlong_t mrfstr_test_sizes[7] =
{
    0x40, 0x400, 0x4000, 0x40000,
    0x400000, 0x4000000, 0x40000000
};

mrfstr_data_ct mrfstr_test_labels[7] =
{
    "64B", "1KB", "16KB", "256KB",
    "4MB", "64MB", "1GB"
};

#define MRFSTR_BLIB_ERROR                                          \
    do                                                             \
    {                                                              \
        fputs("Benchmark Error in File \"" __FILE__ "\"", stderr); \
        fprintf(stderr, ", Line %d\n", __LINE__);                  \
        return EXIT_FAILURE;                                       \
    } while (0)

#define MRFSTR_BLIB_INIT(x, fr)                    \
    do                                             \
    {                                              \
        x = mrfstr_init();                         \
        if (!x)                                    \
        {                                          \
            fr;                                    \
            MRFSTR_BLIB_ERROR;                     \
        }                                          \
                                                   \
        if (mrfstr_alloc(x, mrfstr_test_sizes[6])) \
        {                                          \
            mrfstr_free(x);                        \
            fr;                                    \
            MRFSTR_BLIB_ERROR;                     \
        }                                          \
    } while (0)

#define MRFSTR_BLIB_INIT_STR(x, fr)                              \
    do                                                           \
    {                                                            \
        x = malloc(mrfstr_test_sizes[6] * sizeof(mrfstr_chr_t)); \
        if (!x)                                                  \
        {                                                        \
            fr;                                                  \
            MRFSTR_BLIB_ERROR;                                   \
        }                                                        \
    } while (0)

#define MRFSTR_BLIB_FIRST                      \
    do                                         \
    {                                          \
        QueryPerformanceFrequency(&freq);      \
        mrfstr_config_thread_count(6);         \
        mrfstr_config_thread_limit(0x2000000); \
    } while (0)

#define MRFSTR_BLIB_ROUND_CSTR                                                          \
    do                                                                                  \
    {                                                                                   \
        LARGE_INTEGER start, end, total;                                                \
        mrfstr_size_t count;                                                            \
        mrfstr_byte_t i;                                                                \
                                                                                        \
        for (i = 0; i < 7; i++)                                                         \
        {                                                                               \
            total.QuadPart = 0;                                                         \
            count = 0;                                                                  \
            while (total.QuadPart < freq.QuadPart)                                      \
            {                                                                           \
                QueryPerformanceCounter(&start);                                        \
                MRFSTR_BLIB_CSTR(mrfstr_test_sizes[i]);                                 \
                QueryPerformanceCounter(&end);                                          \
                                                                                        \
                total.QuadPart += end.QuadPart - start.QuadPart;                        \
                count++;                                                                \
            }                                                                           \
                                                                                        \
            cstr[i] = ((mrfstr_double_t)total.QuadPart / count) * 1000 / freq.QuadPart; \
            printf("CSTR    %s: %lf ms"                                                 \
                "\tspeed: %lf GB/s\t(%zu times)\n",                                     \
                mrfstr_test_labels[i], cstr[i],                                         \
                mrfstr_test_sizes[i] / (cstr[i] * 1073741.824), count);                 \
        }                                                                               \
                                                                                        \
        puts("---------------------------------------");                                \
    } while (0)

#define MRFSTR_BLIB_ROUND(name)                                                     \
    do                                                                              \
    {                                                                               \
        LARGE_INTEGER start, end, total;                                            \
        mrfstr_size_t count;                                                        \
        mrfstr_double_t msc;                                                        \
        mrfstr_byte_t i;                                                            \
                                                                                    \
        for (i = 0; i < 7; i++)                                                     \
        {                                                                           \
            total.QuadPart = 0;                                                     \
            count = 0;                                                              \
            while (total.QuadPart < freq.QuadPart)                                  \
            {                                                                       \
                MRFSTR_BLIB_PRE(mrfstr_test_sizes[i]);                              \
                                                                                    \
                QueryPerformanceCounter(&start);                                    \
                MRFSTR_BLIB_OBJ(mrfstr_test_sizes[i]);                              \
                QueryPerformanceCounter(&end);                                      \
                                                                                    \
                total.QuadPart += end.QuadPart - start.QuadPart;                    \
                count++;                                                            \
            }                                                                       \
                                                                                    \
            msc = ((mrfstr_double_t)total.QuadPart / count) * 1000 / freq.QuadPart; \
            printf(name "  %s: %lf ms"                                              \
                "\tspeed: %lf GB/s\t(%zu times)"                                    \
                "       \timprovement: %lfx\n",                                     \
                mrfstr_test_labels[i], msc,                                         \
                mrfstr_test_sizes[i] / (msc * 1073741.824), count,                  \
                cstr[i] / msc);                                                     \
        }                                                                           \
                                                                                    \
        puts("---------------------------------------");                            \
    } while (0)

#endif
