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

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/time.h>

#define MRFSTR_BLIB_FIRST                      \
    do                                         \
    {                                          \
        mrfstr_config_thread_count_max();      \
        mrfstr_config_thread_limit(0x2000000); \
    } while (0)

#define MRFSTR_BLIB_ROUND_CSTR                                                  \
    do                                                                          \
    {                                                                           \
        struct timeval _start, _end;                                            \
        mrfstr_longlong_t _total;                                               \
        mrfstr_size_t _count;                                                   \
        mrfstr_byte_t _i;                                                       \
                                                                                \
        for (_i = 0; _i < 7; _i++)                                              \
        {                                                                       \
            _total = 0;                                                         \
            _count = 0;                                                         \
            while (_total < 1000000)                                            \
            {                                                                   \
                MRFSTR_BLIB_CSTR_PRE(mrfstr_test_sizes[_i]);                    \
                                                                                \
                gettimeofday(&_start, NULL);                                    \
                MRFSTR_BLIB_CSTR(mrfstr_test_sizes[_i]);                        \
                gettimeofday(&_end, NULL);                                      \
                                                                                \
                MRFSTR_BLIB_CSTR_POST(mrfstr_test_sizes[_i]);                   \
                                                                                \
                _total += (_end.tv_sec - _start.tv_sec) * 1000000 +             \
                    (_end.tv_usec - _start.tv_usec);                            \
                _count++;                                                       \
            }                                                                   \
                                                                                \
            benchmark[_i] = (mrfstr_double_t)_total / _count / 1000;            \
            printf("CSTR    %s: %lf ms"                                         \
                "\tspeed: %lf GB/s\t(%zu times)\n",                             \
                mrfstr_test_labels[_i], benchmark[_i],                          \
                mrfstr_test_sizes[_i] / (benchmark[_i] * 1073741.824), _count); \
        }                                                                       \
                                                                                \
        puts("---------------------------------------");                        \
    } while (0)

#define MRFSTR_BLIB_ROUND(name)                                       \
    do                                                                \
    {                                                                 \
        struct timeval _start, _end;                                  \
        mrfstr_longlong_t _total;                                     \
        mrfstr_size_t _count;                                         \
        mrfstr_byte_t _i;                                             \
        mrfstr_double_t _msc;                                         \
                                                                      \
        for (_i = 0; _i < 7; _i++)                                    \
        {                                                             \
            _total = 0;                                               \
            _count = 0;                                               \
            while (_total < 1000000)                                  \
            {                                                         \
                MRFSTR_BLIB_PRE(mrfstr_test_sizes[_i]);               \
                                                                      \
                gettimeofday(&_start, NULL);                          \
                MRFSTR_BLIB_OBJ(mrfstr_test_sizes[_i]);               \
                gettimeofday(&_end, NULL);                            \
                                                                      \
                MRFSTR_BLIB_POST(mrfstr_test_sizes[_i]);              \
                                                                      \
                _total += (_end.tv_sec - _start.tv_sec) * 1000000 +   \
                    (_end.tv_usec - _start.tv_usec);                  \
                _count++;                                             \
            }                                                         \
                                                                      \
            _msc = (mrfstr_double_t)_total / _count / 1000;           \
            printf(name "  %s: %lf ms"                                \
                "\tspeed: %lf GB/s\t(%zu times)"                      \
                "       \timprovement: %lfx\n",                       \
                mrfstr_test_labels[_i], _msc,                         \
                mrfstr_test_sizes[_i] / (_msc * 1073741.824), _count, \
                benchmark[_i] / _msc);                                \
        }                                                             \
                                                                      \
        puts("---------------------------------------");              \
    } while (0)

#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define MRFSTR_BLIB_FIRST                      \
    do                                         \
    {                                          \
        QueryPerformanceFrequency(&freq);      \
        mrfstr_config_thread_count_max();      \
        mrfstr_config_thread_limit(0x2000000); \
    } while (0)

#define MRFSTR_BLIB_ROUND_CSTR                                                  \
    do                                                                          \
    {                                                                           \
        LARGE_INTEGER _start, _end, _total;                                     \
        mrfstr_size_t _count;                                                   \
        mrfstr_byte_t _i;                                                       \
                                                                                \
        for (_i = 0; _i < 7; _i++)                                              \
        {                                                                       \
            _total.QuadPart = 0;                                                \
            _count = 0;                                                         \
            while (_total.QuadPart < freq.QuadPart)                             \
            {                                                                   \
                MRFSTR_BLIB_CSTR_PRE(mrfstr_test_sizes[_i]);                    \
                                                                                \
                QueryPerformanceCounter(&_start);                               \
                MRFSTR_BLIB_CSTR(mrfstr_test_sizes[_i]);                        \
                QueryPerformanceCounter(&_end);                                 \
                                                                                \
                MRFSTR_BLIB_CSTR_POST(mrfstr_test_sizes[_i]);                   \
                                                                                \
                _total.QuadPart += _end.QuadPart - _start.QuadPart;             \
                _count++;                                                       \
            }                                                                   \
                                                                                \
            benchmark[_i] = (mrfstr_double_t)_total.QuadPart / _count *         \
                1000 / freq.QuadPart;                                           \
            printf("CSTR    %s: %lf ms"                                         \
                "\tspeed: %lf GB/s\t(%zu times)\n",                             \
                mrfstr_test_labels[_i], benchmark[_i],                          \
                mrfstr_test_sizes[_i] / (benchmark[_i] * 1073741.824), _count); \
        }                                                                       \
                                                                                \
        puts("---------------------------------------");                        \
    } while (0)

#define MRFSTR_BLIB_ROUND(name)                                       \
    do                                                                \
    {                                                                 \
        LARGE_INTEGER _start, _end, _total;                           \
        mrfstr_size_t _count;                                         \
        mrfstr_double_t _msc;                                         \
        mrfstr_byte_t _i;                                             \
                                                                      \
        for (_i = 0; _i < 7; _i++)                                    \
        {                                                             \
            _total.QuadPart = 0;                                      \
            _count = 0;                                               \
            while (_total.QuadPart < freq.QuadPart)                   \
            {                                                         \
                MRFSTR_BLIB_PRE(mrfstr_test_sizes[_i]);               \
                                                                      \
                QueryPerformanceCounter(&_start);                     \
                MRFSTR_BLIB_OBJ(mrfstr_test_sizes[_i]);               \
                QueryPerformanceCounter(&_end);                       \
                                                                      \
                MRFSTR_BLIB_POST(mrfstr_test_sizes[_i]);              \
                                                                      \
                _total.QuadPart += _end.QuadPart - _start.QuadPart;   \
                _count++;                                             \
            }                                                         \
                                                                      \
            _msc = (mrfstr_double_t)_total.QuadPart / _count *        \
                1000 / freq.QuadPart;                                 \
            printf(name "  %s: %lf ms"                                \
                "\tspeed: %lf GB/s\t(%zu times)"                      \
                "       \timprovement: %lfx\n",                       \
                mrfstr_test_labels[_i], _msc,                         \
                mrfstr_test_sizes[_i] / (_msc * 1073741.824), _count, \
                benchmark[_i] / _msc);                                \
        }                                                             \
                                                                      \
        puts("---------------------------------------");              \
    } while (0)

#endif

#endif
