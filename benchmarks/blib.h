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

#include <mrfstr-intern.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MRFSTR_BLIB_DEF 7

mrfstr_longlong_t mrfstr_blib_sizes[MRFSTR_BLIB_DEF] =
{
    0x40, 0x400, 0x4000, 0x40000,
    0x400000, 0x4000000, 0x40000000
};

mrfstr_short_t mrfstr_blib_size_labels[MRFSTR_BLIB_DEF] =
{
    64, 1, 16, 256, 4, 64, 1
};

mrfstr_data_ct mrfstr_blib_labels[MRFSTR_BLIB_DEF] =
{
    "B", "KB", "KB", "KB", "MB", "MB", "GB"
};

#define MRFSTR_BLIB_ERROR                                          \
    do                                                             \
    {                                                              \
        fputs("Benchmark Error in File \"" __FILE__ "\"", stderr); \
        fprintf(stderr, ", Line %d\n", __LINE__);                  \
        return EXIT_FAILURE;                                       \
    } while (0)

#define MRFSTR_BLIB_INIT(x, fr)                        \
    do                                                 \
    {                                                  \
        x = mrfstr_init();                             \
        if (!x)                                        \
        {                                              \
            fr;                                        \
            MRFSTR_BLIB_ERROR;                         \
        }                                              \
                                                       \
        if (mrfstr_alloc(x, mrfstr_sizes[scount - 1])) \
        {                                              \
            mrfstr_free(x);                            \
            fr;                                        \
            MRFSTR_BLIB_ERROR;                         \
        }                                              \
    } while (0)

#define MRFSTR_BLIB_INIT_STR(x, fr)                                  \
    do                                                               \
    {                                                                \
        x = malloc(mrfstr_sizes[scount - 1] * sizeof(mrfstr_chr_t)); \
        if (!x)                                                      \
        {                                                            \
            fr;                                                      \
            MRFSTR_BLIB_ERROR;                                       \
        }                                                            \
    } while (0)

#define MRFSTR_BLIB_ARGS                                                             \
    do                                                                               \
    {                                                                                \
        mrfstr_sizes = mrfstr_blib_sizes;                                            \
        mrfstr_labels = mrfstr_blib_labels;                                          \
        mrfstr_size_labels = mrfstr_blib_size_labels;                                \
                                                                                     \
        switch (argc)                                                                \
        {                                                                            \
        case 1:                                                                      \
        case 3:                                                                      \
            scount = MRFSTR_BLIB_DEF;                                                \
            tcount = 1;                                                              \
            break;                                                                   \
        case 2:                                                                      \
            scount = MRFSTR_BLIB_DEF;                                                \
            tcount = (mrfstr_byte_t)atoi(argv[1]);                                   \
            break;                                                                   \
        case 4:                                                                      \
            scount = (mrfstr_byte_t)atoi(argv[3]);                                   \
            tcount = 1;                                                              \
            break;                                                                   \
        case 5:                                                                      \
            scount = (mrfstr_byte_t)atoi(argv[3]);                                   \
            tcount = (mrfstr_byte_t)atoi(argv[4]);                                   \
            break;                                                                   \
        default:                                                                     \
            abort();                                                                 \
        }                                                                            \
                                                                                     \
        if (argc >= 3)                                                               \
        {                                                                            \
            mrfstr_byte_t start, end, shift, i;                                      \
                                                                                     \
            mrfstr_sizes = malloc(scount * sizeof(mrfstr_size_t));                   \
            if (!mrfstr_sizes)                                                       \
                MRFSTR_BLIB_ERROR;                                                   \
                                                                                     \
            mrfstr_labels = malloc(scount * sizeof(mrfstr_data_ct));                 \
            if (!mrfstr_labels)                                                      \
                MRFSTR_BLIB_ERROR;                                                   \
                                                                                     \
            mrfstr_size_labels = malloc(scount * sizeof(mrfstr_short_t));            \
            if (!mrfstr_size_labels)                                                 \
                MRFSTR_BLIB_ERROR;                                                   \
                                                                                     \
            start = (mrfstr_byte_t)atoi(argv[1]);                                    \
            end = (mrfstr_byte_t)atoi(argv[2]);                                      \
            shift = (end - start) / (scount - 1);                                    \
                                                                                     \
            *mrfstr_sizes = (mrfstr_size_t)(1 << start);                             \
            for (i = 1; i < scount; i++)                                             \
                mrfstr_sizes[i] = mrfstr_sizes[i - 1] << shift;                      \
                                                                                     \
            for (i = 0; i < scount; i++)                                             \
            {                                                                        \
                if (mrfstr_sizes[i] < 1024)                                          \
                {                                                                    \
                    mrfstr_labels[i] = "B";                                          \
                    mrfstr_size_labels[i] = (mrfstr_short_t)mrfstr_sizes[i];         \
                    continue;                                                        \
                }                                                                    \
                                                                                     \
                if (mrfstr_sizes[i] < 1024 * 1024)                                   \
                {                                                                    \
                    mrfstr_labels[i] = "KB";                                         \
                    mrfstr_size_labels[i] = (mrfstr_short_t)(mrfstr_sizes[i] >> 10); \
                    continue;                                                        \
                }                                                                    \
                                                                                     \
                if (mrfstr_sizes[i] < 1024 * 1024 * 1024)                            \
                {                                                                    \
                    mrfstr_labels[i] = "MB";                                         \
                    mrfstr_size_labels[i] = (mrfstr_short_t)(mrfstr_sizes[i] >> 20); \
                    continue;                                                        \
                }                                                                    \
                                                                                     \
                mrfstr_labels[i] = "GB";                                             \
                mrfstr_size_labels[i] = (mrfstr_short_t)(mrfstr_sizes[i] >> 30);     \
            }                                                                        \
        }                                                                            \
                                                                                     \
        benchmark = malloc(scount * sizeof(mrfstr_double_t));                        \
        if (!benchmark)                                                              \
            MRFSTR_BLIB_ERROR;                                                       \
    } while (0)

#ifdef MRFSTR_BUILD_UNIX
#include <sys/time.h>

#define MRFSTR_BLIB_FIRST                      \
    do                                         \
    {                                          \
        mrfstr_config_thread_count_max();      \
        mrfstr_config_thread_limit(0x2000000); \
    } while (0)

#define MRFSTR_BLIB_ROUND_CSTR                                             \
    do                                                                     \
    {                                                                      \
        struct timeval _start, _end;                                       \
        mrfstr_longlong_t _total;                                          \
        mrfstr_size_t _count;                                              \
        mrfstr_byte_t _i;                                                  \
                                                                           \
        for (_i = 0; _i < scount; _i++)                                    \
        {                                                                  \
            _total = 0;                                                    \
            _count = 0;                                                    \
            while (_total < tcount * 1000000)                              \
            {                                                              \
                MRFSTR_BLIB_CSTR_PRE(mrfstr_sizes[_i]);                    \
                                                                           \
                gettimeofday(&_start, NULL);                               \
                MRFSTR_BLIB_CSTR(mrfstr_sizes[_i]);                        \
                gettimeofday(&_end, NULL);                                 \
                                                                           \
                MRFSTR_BLIB_CSTR_POST(mrfstr_sizes[_i]);                   \
                                                                           \
                _total += (_end.tv_sec - _start.tv_sec) * 1000000 +        \
                    (_end.tv_usec - _start.tv_usec);                       \
                _count++;                                                  \
            }                                                              \
                                                                           \
            benchmark[_i] = (mrfstr_double_t)_total / _count / 1000;       \
            printf("CSTR    %hu%s: %lf ms"                                 \
                "\tspeed: %lf GB/s\t(%zu times)\n",                        \
                mrfstr_size_labels[_i], mrfstr_labels[_i], benchmark[_i],  \
                mrfstr_sizes[_i] / (benchmark[_i] * 1073741.824), _count); \
        }                                                                  \
                                                                           \
        puts("---------------------------------------");                   \
    } while (0)

#define MRFSTR_BLIB_ROUND(name)                                     \
    do                                                              \
    {                                                               \
        struct timeval _start, _end;                                \
        mrfstr_longlong_t _total;                                   \
        mrfstr_size_t _count;                                       \
        mrfstr_byte_t _i;                                           \
        mrfstr_double_t _msc;                                       \
                                                                    \
        for (_i = 0; _i < scount; _i++)                             \
        {                                                           \
            _total = 0;                                             \
            _count = 0;                                             \
            while (_total < tcount * 1000000)                       \
            {                                                       \
                MRFSTR_BLIB_PRE(mrfstr_sizes[_i]);                  \
                                                                    \
                gettimeofday(&_start, NULL);                        \
                MRFSTR_BLIB_OBJ(mrfstr_sizes[_i]);                  \
                gettimeofday(&_end, NULL);                          \
                                                                    \
                MRFSTR_BLIB_POST(mrfstr_sizes[_i]);                 \
                                                                    \
                _total += (_end.tv_sec - _start.tv_sec) * 1000000 + \
                    (_end.tv_usec - _start.tv_usec);                \
                _count++;                                           \
            }                                                       \
                                                                    \
            _msc = (mrfstr_double_t)_total / _count / 1000;         \
            printf(name "  %hu%s: %lf ms"                           \
                "\tspeed: %lf GB/s\t(%zu times)"                    \
                "       \timprovement: %lfx\n",                     \
                mrfstr_size_labels[_i], mrfstr_labels[_i], _msc,    \
                mrfstr_sizes[_i] / (_msc * 1073741.824), _count,    \
                benchmark[_i] / _msc);                              \
        }                                                           \
                                                                    \
        puts("---------------------------------------");            \
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

#define MRFSTR_BLIB_ROUND_CSTR                                             \
    do                                                                     \
    {                                                                      \
        LARGE_INTEGER _start, _end, _total;                                \
        mrfstr_size_t _count;                                              \
        mrfstr_byte_t _i;                                                  \
                                                                           \
        for (_i = 0; _i < scount; _i++)                                    \
        {                                                                  \
            _total.QuadPart = 0;                                           \
            _count = 0;                                                    \
            while (_total.QuadPart < tcount * freq.QuadPart)               \
            {                                                              \
                MRFSTR_BLIB_CSTR_PRE(mrfstr_sizes[_i]);                    \
                                                                           \
                QueryPerformanceCounter(&_start);                          \
                MRFSTR_BLIB_CSTR(mrfstr_sizes[_i]);                        \
                QueryPerformanceCounter(&_end);                            \
                                                                           \
                MRFSTR_BLIB_CSTR_POST(mrfstr_sizes[_i]);                   \
                                                                           \
                _total.QuadPart += _end.QuadPart - _start.QuadPart;        \
                _count++;                                                  \
            }                                                              \
                                                                           \
            benchmark[_i] = (mrfstr_double_t)_total.QuadPart / _count *    \
                1000 / freq.QuadPart;                                      \
            printf("CSTR    %hu%s: %lf ms"                                 \
                "\tspeed: %lf GB/s\t(%zu times)\n",                        \
                mrfstr_size_labels[_i], mrfstr_labels[_i], benchmark[_i],  \
                mrfstr_sizes[_i] / (benchmark[_i] * 1073741.824), _count); \
        }                                                                  \
                                                                           \
        puts("---------------------------------------");                   \
    } while (0)

#define MRFSTR_BLIB_ROUND(name)                                     \
    do                                                              \
    {                                                               \
        LARGE_INTEGER _start, _end, _total;                         \
        mrfstr_size_t _count;                                       \
        mrfstr_double_t _msc;                                       \
        mrfstr_byte_t _i;                                           \
                                                                    \
        for (_i = 0; _i < scount; _i++)                             \
        {                                                           \
            _total.QuadPart = 0;                                    \
            _count = 0;                                             \
            while (_total.QuadPart < tcount * freq.QuadPart)        \
            {                                                       \
                MRFSTR_BLIB_PRE(mrfstr_sizes[_i]);                  \
                                                                    \
                QueryPerformanceCounter(&_start);                   \
                MRFSTR_BLIB_OBJ(mrfstr_sizes[_i]);                  \
                QueryPerformanceCounter(&_end);                     \
                                                                    \
                MRFSTR_BLIB_POST(mrfstr_sizes[_i]);                 \
                                                                    \
                _total.QuadPart += _end.QuadPart - _start.QuadPart; \
                _count++;                                           \
            }                                                       \
                                                                    \
            _msc = (mrfstr_double_t)_total.QuadPart / _count *      \
                1000 / freq.QuadPart;                               \
            printf(name "  %hu%s: %lf ms"                           \
                "\tspeed: %lf GB/s\t(%zu times)"                    \
                "       \timprovement: %lfx\n",                     \
                mrfstr_size_labels[_i], mrfstr_labels[_i], _msc,    \
                mrfstr_sizes[_i] / (_msc * 1073741.824), _count,    \
                benchmark[_i] / _msc);                              \
        }                                                           \
                                                                    \
        puts("---------------------------------------");            \
    } while (0)

#endif

#endif
