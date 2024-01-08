/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <stdio.h>
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#elif defined(_MSC_VER)
#include <intrin.h>
#else
#error Your compiler is not supported yet
#endif

int main(void)
{
#if defined(__GNUC__) || defined(__clang__)
    FILE *header = fopen("heads/simddef.h", "w");
#elif defined(_MSC_VER)
    FILE *header;
    fopen_s(&header, "heads/simddef.h", "w");
#endif

    if (!header)
        return EXIT_FAILURE;

    fputs("/*\n"
        "\tMRFStr Library version 1.0.0\n"
        "\tMetaReal Fast String Library\n"
        "*/\n\n"
        "#ifndef __MR_SIMDDEF__\n"
        "#define __MR_SIMDDEF__\n\n", header);

    int data[4];

#if defined(__GNUC__) || defined(__clang__)
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
