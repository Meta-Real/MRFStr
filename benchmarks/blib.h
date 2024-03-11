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
#include <math.h>

#pragma pack(push, 1)
struct __MRFSTR_BLIB_TEST_T
{
    mrfstr_data_ct label;
    mrfstr_float_t label_size;
    mrfstr_size_t size;
    mrfstr_double_t ctest;
};
#pragma pack(pop)
typedef struct __MRFSTR_BLIB_TEST_T mrfstr_blib_test_t;

enum __MRFSTR_BLIB_FORMAT_ENUM
{
    MRFSTR_BLIB_FORMAT_NORM,
    MRFSTR_BLIB_FORMAT_CSV
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
        if (mrfstr_alloc(x, tests[nsec - 1].size)) \
        {                                          \
            mrfstr_free(x);                        \
            fr;                                    \
            MRFSTR_BLIB_ERROR;                     \
        }                                          \
    } while (0)

#define MRFSTR_BLIB_INIT_STR(x, fr)                              \
    do                                                           \
    {                                                            \
        x = malloc(tests[nsec - 1].size * sizeof(mrfstr_chr_t)); \
        if (!x)                                                  \
        {                                                        \
            fr;                                                  \
            MRFSTR_BLIB_ERROR;                                   \
        }                                                        \
    } while (0)

#define MRFSTR_BLIB_ARGS                                                        \
    do                                                                          \
    {                                                                           \
        mrfstr_data_t ptr;                                                      \
        mrfstr_size_t start, end, step;                                         \
        mrfstr_short_t i;                                                       \
        mrfstr_bool_t exponential;                                              \
                                                                                \
        start = 64;                                                             \
        end = 0x10000000;                                                       \
        step = 4;                                                               \
        exponential = MRFSTR_TRUE;                                              \
        ntime = 1;                                                              \
        ncount = 2;                                                             \
        format = MRFSTR_BLIB_FORMAT_NORM;                                       \
        file = NULL;                                                            \
        for (i = 1; i < argc; i++)                                              \
        {                                                                       \
            ptr = (mrfstr_data_t)argv[i];                                       \
            if (!memcmp(ptr, "start=", 6))                                      \
            {                                                                   \
                start = (mrfstr_size_t)strtoull(ptr + 6, &ptr, 10);             \
                switch (*ptr)                                                   \
                {                                                               \
                case '\0':                                                      \
                case 'B':                                                       \
                    break;                                                      \
                case 'K':                                                       \
                    start *= 1024;                                              \
                    break;                                                      \
                case 'M':                                                       \
                    start *= 0x100000;                                          \
                    break;                                                      \
                case 'G':                                                       \
                    start *= 0x40000000;                                        \
                    break;                                                      \
                }                                                               \
                                                                                \
                continue;                                                       \
            }                                                                   \
                                                                                \
            if (!memcmp(ptr, "end=", 4))                                        \
            {                                                                   \
                end = (mrfstr_size_t)strtoull(ptr + 4, &ptr, 10);               \
                switch (*ptr)                                                   \
                {                                                               \
                case '\0':                                                      \
                case 'B':                                                       \
                case 'b':                                                       \
                    break;                                                      \
                case 'K':                                                       \
                case 'k':                                                       \
                    end *= 1024;                                                \
                    break;                                                      \
                case 'M':                                                       \
                case 'm':                                                       \
                    end *= 0x100000;                                            \
                    break;                                                      \
                case 'G':                                                       \
                case 'g':                                                       \
                    end *= 0x40000000;                                          \
                    break;                                                      \
                }                                                               \
                continue;                                                       \
            }                                                                   \
                                                                                \
            if (!memcmp(ptr, "step=", 5))                                       \
            {                                                                   \
                step = (mrfstr_size_t)strtoull(ptr + 5, &ptr, 10);              \
                switch (*ptr)                                                   \
                {                                                               \
                case '\0':                                                      \
                case 'B':                                                       \
                case 'b':                                                       \
                    break;                                                      \
                case 'K':                                                       \
                case 'k':                                                       \
                    step *= 1024;                                               \
                    break;                                                      \
                case 'M':                                                       \
                case 'm':                                                       \
                    step *= 0x100000;                                           \
                    break;                                                      \
                case 'G':                                                       \
                case 'g':                                                       \
                    step *= 0x40000000;                                         \
                    break;                                                      \
                }                                                               \
                continue;                                                       \
            }                                                                   \
                                                                                \
            if (!memcmp(ptr, "exp=on", 6))                                      \
            {                                                                   \
                exponential = MRFSTR_TRUE;                                      \
                continue;                                                       \
            }                                                                   \
                                                                                \
            if (!memcmp(ptr, "exp=off", 7))                                     \
            {                                                                   \
                exponential = MRFSTR_FALSE;                                     \
                continue;                                                       \
            }                                                                   \
                                                                                \
            if (!memcmp(ptr, "time=", 5))                                       \
            {                                                                   \
                ntime = (mrfstr_short_t)strtoull(ptr + 5, &ptr, 10);            \
                switch (*ptr)                                                   \
                {                                                               \
                case '\0':                                                      \
                case 'S':                                                       \
                case 's':                                                       \
                    break;                                                      \
                case 'M':                                                       \
                case 'm':                                                       \
                    ntime *= 60;                                                \
                    break;                                                      \
                case 'H':                                                       \
                case 'h':                                                       \
                    ntime *= 3600;                                              \
                    break;                                                      \
                }                                                               \
                continue;                                                       \
            }                                                                   \
                                                                                \
            if (!memcmp(ptr, "count=", 6))                                      \
            {                                                                   \
                ncount = (mrfstr_short_t)strtoull(ptr + 6, NULL, 10);           \
                continue;                                                       \
            }                                                                   \
                                                                                \
            if (!memcmp(ptr, "format=csv", 10))                                 \
            {                                                                   \
                format = MRFSTR_BLIB_FORMAT_CSV;                                \
                continue;                                                       \
            }                                                                   \
                                                                                \
            if (!memcmp(ptr, "format=norm", 11))                                \
            {                                                                   \
                format = MRFSTR_BLIB_FORMAT_NORM;                               \
                continue;                                                       \
            }                                                                   \
        }                                                                       \
                                                                                \
        if (exponential)                                                        \
            nsec = (mrfstr_short_t)(log2((mrfstr_double_t)end / start) /        \
                log2((mrfstr_double_t)step) + 1);                               \
        else                                                                    \
            nsec = (mrfstr_short_t)((end - start) / step + 1);                  \
                                                                                \
        tests = (mrfstr_blib_test_t*)malloc(nsec * sizeof(mrfstr_blib_test_t)); \
        if (!tests)                                                             \
            MRFSTR_BLIB_ERROR;                                                  \
                                                                                \
        if (format == MRFSTR_BLIB_FORMAT_CSV)                                   \
            file = fopen("benchmarks", "wb");                                   \
                                                                                \
        for (i = 0; i < nsec; i++)                                              \
        {                                                                       \
            tests[i].size = start;                                              \
            if (start < 1024)                                                   \
            {                                                                   \
                tests[i].label = "B";                                           \
                tests[i].label_size = (mrfstr_float_t)start;                    \
            }                                                                   \
            else if (start < 0x100000)                                          \
            {                                                                   \
                tests[i].label = "KB";                                          \
                tests[i].label_size = (mrfstr_float_t)start / 1024;             \
            }                                                                   \
            else if (start < 0x40000000)                                        \
            {                                                                   \
                tests[i].label = "MB";                                          \
                tests[i].label_size = (mrfstr_float_t)start / 0x100000;         \
            }                                                                   \
            else                                                                \
            {                                                                   \
                tests[i].label = "GB";                                          \
                tests[i].label_size = (mrfstr_float_t)start / 0x40000000;       \
            }                                                                   \
                                                                                \
            if (exponential)                                                    \
                start *= step;                                                  \
            else                                                                \
                start += step;                                                  \
        }                                                                       \
    } while (0)

#define MRFSTR_BLIB_RETURN                    \
    do                                        \
    {                                         \
        free(tests);                          \
                                              \
        if (format == MRFSTR_BLIB_FORMAT_CSV) \
            fclose(file);                     \
        return 0;                             \
    } while (0)

#ifdef MRFSTR_BUILD_UNIX
#include <sys/time.h>

#define MRFSTR_BLIB_VARS                \
    mrfstr_short_t nsec, ntime, ncount; \
    mrfstr_byte_t format;               \
    FILE *file;                         \
    mrfstr_blib_test_t *tests

#define MRFSTR_BLIB_FIRST mrfstr_config(MRFSTR_CONFIG_LEVEL_EXTREME)

#define MRFSTR_BLIB_ROUND_CSTR                                             \
    do                                                                     \
    {                                                                      \
        struct timeval _start, _end;                                       \
        mrfstr_longlong_t _total;                                          \
        mrfstr_size_t _count;                                              \
        mrfstr_byte_t _i;                                                  \
                                                                           \
        if (format == MRFSTR_BLIB_FORMAT_CSV)                              \
        {                                                                  \
            fputc('\t', file);                                             \
            for (_i = 0; _i < nsec; _i++)                                  \
                fprintf(file, "%.2f%s\t",                                  \
                    tests[_i].label_size, tests[_i].label);                \
            fputs("\nCSTR\t", file);                                       \
        }                                                                  \
        for (_i = 0; _i < nsec; _i++)                                      \
        {                                                                  \
            _total = 0;                                                    \
            _count = 0;                                                    \
            while (_total < ntime * 1000000 || _count < ncount)            \
            {                                                              \
                MRFSTR_BLIB_CSTR_PRE(tests[_i].size);                      \
                                                                           \
                gettimeofday(&_start, NULL);                               \
                MRFSTR_BLIB_CSTR(tests[_i].size);                          \
                gettimeofday(&_end, NULL);                                 \
                                                                           \
                MRFSTR_BLIB_CSTR_POST(tests[_i].size);                     \
                                                                           \
                _total += (_end.tv_sec - _start.tv_sec) * 1000000 +        \
                    (_end.tv_usec - _start.tv_usec);                       \
                _count++;                                                  \
            }                                                              \
                                                                           \
            tests[_i].ctest = (mrfstr_double_t)_total / _count / 1000;     \
                                                                           \
            if (format == MRFSTR_BLIB_FORMAT_CSV)                          \
                fprintf(file, "%lf\t",                                     \
                    tests[_i].size / (tests[_i].ctest * 1073741.824));     \
                                                                           \
            printf("CSTR    %.2f%s: %lf ms"                                \
                "\tspeed: %lf GB/s\t(%zu times)\n",                        \
                tests[_i].label_size, tests[_i].label, tests[_i].ctest,    \
                tests[_i].size / (tests[_i].ctest * 1073741.824), _count); \
        }                                                                  \
                                                                           \
        if (format == MRFSTR_BLIB_FORMAT_CSV)                              \
            fputc('\n', file);                                             \
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
        if (format == MRFSTR_BLIB_FORMAT_CSV)                       \
            fprintf(file, "%s\t", name);                            \
        for (_i = 0; _i < nsec; _i++)                               \
        {                                                           \
            _total = 0;                                             \
            _count = 0;                                             \
            while (_total < ntime * 1000000 || _count < ncount)     \
            {                                                       \
                MRFSTR_BLIB_PRE(tests[_i].size);                    \
                                                                    \
                gettimeofday(&_start, NULL);                        \
                MRFSTR_BLIB_OBJ(tests[_i].size);                    \
                gettimeofday(&_end, NULL);                          \
                                                                    \
                MRFSTR_BLIB_POST(tests[_i].size);                   \
                                                                    \
                _total += (_end.tv_sec - _start.tv_sec) * 1000000 + \
                    (_end.tv_usec - _start.tv_usec);                \
                _count++;                                           \
            }                                                       \
                                                                    \
            _msc = (mrfstr_double_t)_total / _count / 1000;         \
                                                                    \
            if (format == MRFSTR_BLIB_FORMAT_CSV)                   \
                fprintf(file, "%lf\t",                              \
                    tests[_i].size / (_msc * 1073741.824));         \
                                                                    \
            printf(name "  %.2f%s: %lf ms"                          \
                "\tspeed: %lf GB/s\t(%zu times)"                    \
                "       \timprovement: %lfx\n",                     \
                tests[_i].label_size, tests[_i].label, _msc,        \
                tests[_i].size / (_msc * 1073741.824), _count,      \
                tests[_i].ctest / _msc);                            \
        }                                                           \
                                                                    \
        if (format == MRFSTR_BLIB_FORMAT_CSV)                       \
            fputc('\n', file);                                      \
        puts("---------------------------------------");            \
    } while (0)

#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define MRFSTR_BLIB_VARS                \
    LARGE_INTEGER freq;                 \
    mrfstr_short_t nsec, ntime, ncount; \
    mrfstr_byte_t format;               \
    FILE *file;                         \
    mrfstr_blib_test_t *tests

#define MRFSTR_BLIB_FIRST                           \
    do                                              \
    {                                               \
        QueryPerformanceFrequency(&freq);           \
        mrfstr_config(MRFSTR_CONFIG_LEVEL_EXTREME); \
    } while (0)

#define MRFSTR_BLIB_ROUND_CSTR                                                 \
    do                                                                         \
    {                                                                          \
        LARGE_INTEGER _start, _end, _total;                                    \
        mrfstr_size_t _count;                                                  \
        mrfstr_byte_t _i;                                                      \
                                                                               \
        if (format == MRFSTR_BLIB_FORMAT_CSV)                                  \
        {                                                                      \
            fputc('\t', file);                                                 \
            for (_i = 0; _i < nsec; _i++)                                      \
                fprintf(file, "%.2f%s\t",                                      \
                    tests[_i].label_size, tests[_i].label);                    \
            fputs("\nCSTR\t", file);                                           \
        }                                                                      \
        for (_i = 0; _i < nsec; _i++)                                          \
        {                                                                      \
            _total.QuadPart = 0;                                               \
            _count = 0;                                                        \
            while (_total.QuadPart < ntime * freq.QuadPart || _count < ncount) \
            {                                                                  \
                MRFSTR_BLIB_CSTR_PRE(tests[_i].size);                          \
                                                                               \
                QueryPerformanceCounter(&_start);                              \
                MRFSTR_BLIB_CSTR(tests[_i].size);                              \
                QueryPerformanceCounter(&_end);                                \
                                                                               \
                MRFSTR_BLIB_CSTR_POST(tests[_i].size);                         \
                                                                               \
                _total.QuadPart += _end.QuadPart - _start.QuadPart;            \
                _count++;                                                      \
            }                                                                  \
                                                                               \
            tests[_i].ctest = (mrfstr_double_t)_total.QuadPart / _count *      \
                1000 / freq.QuadPart;                                          \
                                                                               \
            if (format == MRFSTR_BLIB_FORMAT_CSV)                              \
                fprintf(file, "%lf\t",                                         \
                    tests[_i].size / (tests[_i].ctest * 1073741.824));         \
                                                                               \
            printf("CSTR   %.2f%s: %lf ms"                                     \
                "\tspeed: %lf GB/s\t(%zu times)\n",                            \
                tests[_i].label_size, tests[_i].label, tests[_i].ctest,        \
                tests[_i].size / (tests[_i].ctest * 1073741.824), _count);     \
        }                                                                      \
                                                                               \
        if (format == MRFSTR_BLIB_FORMAT_CSV)                                  \
            fputc('\n', file);                                                 \
        puts("---------------------------------------");                       \
    } while (0)

#define MRFSTR_BLIB_ROUND(name)                                                \
    do                                                                         \
    {                                                                          \
        LARGE_INTEGER _start, _end, _total;                                    \
        mrfstr_size_t _count;                                                  \
        mrfstr_double_t _msc;                                                  \
        mrfstr_byte_t _i;                                                      \
                                                                               \
        if (format == MRFSTR_BLIB_FORMAT_CSV)                                  \
            fprintf(file, "%s\t", name);                                       \
        for (_i = 0; _i < nsec; _i++)                                          \
        {                                                                      \
            _total.QuadPart = 0;                                               \
            _count = 0;                                                        \
            while (_total.QuadPart < ntime * freq.QuadPart || _count < ncount) \
            {                                                                  \
                MRFSTR_BLIB_PRE(tests[_i].size);                               \
                                                                               \
                QueryPerformanceCounter(&_start);                              \
                MRFSTR_BLIB_OBJ(tests[_i].size);                               \
                QueryPerformanceCounter(&_end);                                \
                                                                               \
                MRFSTR_BLIB_POST(tests[_i].size);                              \
                                                                               \
                _total.QuadPart += _end.QuadPart - _start.QuadPart;            \
                _count++;                                                      \
            }                                                                  \
                                                                               \
            _msc = (mrfstr_double_t)_total.QuadPart / _count *                 \
                1000 / freq.QuadPart;                                          \
                                                                               \
            if (format == MRFSTR_BLIB_FORMAT_CSV)                              \
                fprintf(file, "%lf\t",                                         \
                    tests[_i].size / (_msc * 1073741.824));                    \
                                                                               \
            printf(name " %.2f%s: %lf ms"                                      \
                "\tspeed: %lf GB/s\t(%zu times)"                               \
                "       \timprovement: %lfx\n",                                \
                tests[_i].label_size, tests[_i].label, _msc,                   \
                tests[_i].size / (_msc * 1073741.824), _count,                 \
                tests[_i].ctest / _msc);                                       \
        }                                                                      \
                                                                               \
        if (format == MRFSTR_BLIB_FORMAT_CSV)                                  \
            fputc('\n', file);                                                 \
        puts("---------------------------------------");                       \
    } while (0)

#endif

#endif
