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

#include <stdio.h>
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
#define MRFSTR_GCC_CLANG
#endif

#ifdef MRFSTR_GCC_CLANG
#include <cpuid.h>
#elif defined(_MSC_VER)
#include <intrin.h>
#else
#error Your compiler is not supported yet
#endif

int main(void)
{
    FILE *header = fopen("heads/simddef.h", "w");
    if (!header)
        return EXIT_FAILURE;

    fputs("#ifndef __MRFSTR_SIMDDEF__\n"
        "#define __MRFSTR_SIMDDEF__\n\n", header);

    int data[4];

#ifdef MRFSTR_GCC_CLANG
    __cpuid(0, data[0], data[1], data[2], data[3]);
#elif defined(_MSC_VER)
    __cpuid(data, 0);
#endif

    int nid = data[0];
    if (nid >= 1)
    {
        __cpuidex(data, 1, 0);

        int reg = data[2];
        if (reg & 0x200)
            fputs("#ifndef __SSSE3__\n"
                "#define __SSSE3__\n"
                "#endif\n\n", header);
        if (reg & 0x80000)
            fputs("#ifndef __SSE4_1__\n"
                "#define __SSE4_1__\n"
                "#endif\n\n", header);
        if (reg & 0x10000000)
            fputs("#ifndef __AVX__\n"
                "#define __AVX__\n"
                "#endif\n\n", header);

        reg = data[3];
        if (reg & 0x4000000)
            fputs("#ifndef __SSE2__\n"
                "#define __SSE2__\n"
                "#endif\n\n", header);

        if (nid >= 7)
        {
            __cpuidex(data, 7, 0);

            reg = data[1];
            if (reg & 0x20)
                fputs("#ifndef __AVX2__\n"
                    "#define __AVX2__\n"
                    "#endif\n\n", header);
            if (reg & 0x10000)
                fputs("#ifndef __AVX512F__\n"
                    "#define __AVX512F__\n"
                    "#endif\n\n", header);
            if (reg & 0x40000000)
                fputs("#ifndef __AVX512BW__\n"
                    "#define __AVX512BW__\n"
                    "#endif\n\n", header);
            if (reg & 0x80000000)
                fputs("#ifndef __AVX512VL__\n"
                    "#define __AVX512VL__\n"
                    "#endif\n\n", header);

            reg = data[2];
            if (reg & 2)
                fputs("#ifndef __AVX512VBMI__\n"
                    "#define __AVX512VBMI__\n"
                    "#endif\n\n", header);
        }
    }

    fputs("#endif\n", header);

    fclose(header);
    return EXIT_SUCCESS;
}
