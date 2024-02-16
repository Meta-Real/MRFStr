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
#include <cpuid/cpuid.h>

int main(void)
{
    FILE *header = fopen("heads/simddef.h", "w");
    if (!header)
        return 1;

    fputs("#ifndef __MRFSTR_SIMDDEF__\n"
        "#define __MRFSTR_SIMDDEF__\n\n", header);

    mrfstr_cpuid_cputype(NULL, NULL);
    mrfstr_cpuid_funccnt(NULL);

    mrfstr_byte_t simd = mrfstr_cpuid_simdset();
    if (simd >= 1)
        fputs("#ifndef __SSE2__\n"
            "#define __SSE2__\n"
            "#endif\n\n", header);
    if (simd >= 2)
        fputs("#ifndef __SSSE3__\n"
            "#define __SSSE3__\n"
            "#endif\n\n", header);
    if (simd >= 3)
        fputs("#ifndef __SSE4_1__\n"
            "#define __SSE4_1__\n"
            "#endif\n\n", header);
    if (simd >= 4)
        fputs("#ifndef __AVX__\n"
            "#define __AVX__\n"
            "#endif\n\n", header);
    if (simd >= 5)
        fputs("#ifndef __AVX2__\n"
            "#define __AVX2__\n"
            "#endif\n\n", header);
    if (simd >= 6)
        fputs("#ifndef __AVX512F__\n"
            "#define __AVX512F__\n"
            "#endif\n\n", header);
    if (simd >= 7)
    {
        fputs("#ifndef __AVX512BW__\n"
            "#define __AVX512BW__\n"
            "#endif\n\n", header);
        fputs("#ifndef __AVX512VL__\n"
            "#define __AVX512VL__\n"
            "#endif\n\n", header);
    }
    if (simd >= 8)
        fputs("#ifndef __AVX512VBMI__\n"
            "#define __AVX512VBMI__\n"
            "#endif\n\n", header);

    fputs("#endif\n", header);

    fclose(header);
    return 0;
}
