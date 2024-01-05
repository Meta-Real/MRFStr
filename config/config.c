/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

#ifdef __AVX512F__
mrfstr_config_t mrfstr_config =
{
    1,
    __mrfstr_avx512_copy, 64,
    __mrfstr_avx512_copy, 64,
    __mrfstr_avx512_fill, 64,
    __mrfstr_avx512_fill, 64,
    __mrfstr_avx512_equal, 64,
    __mrfstr_avx512_tequal, 64,
    __mrfstr_avx512_contain, 64,
    __mrfstr_avx512_tcontain, 64
};
#elif defined(__AVX__)
mrfstr_config_t mrfstr_config =
{
    1,
    __mrfstr_avx_copy, 32,
    __mrfstr_avx_copy, 32,
    __mrfstr_avx_fill, 32,
    __mrfstr_avx_fill, 32,
#ifdef __AVX2__
    __mrfstr_avx_equal, 32,
    __mrfstr_avx_tequal, 32,
    __mrfstr_avx_contain, 32,
    __mrfstr_avx_tcontain, 32
#else
    __mrfstr_sse_equal, 16,
    __mrfstr_sse_tequal, 16,
    __mrfstr_sse_contain, 16,
    __mrfstr_sse_tcontain, 16
#endif
};
#elif defined(__SSE2__)
mrfstr_config_t mrfstr_config =
{
    1,
    __mrfstr_sse_copy, 16,
    __mrfstr_sse_copy, 16,
    __mrfstr_sse_fill, 16,
    __mrfstr_sse_fill, 16,
    __mrfstr_sse_equal, 16,
    __mrfstr_sse_tequal, 16,
    __mrfstr_sse_contain, 16,
    __mrfstr_sse_tcontain, 16
};
#else
mrfstr_config_t mrfstr_config =
{
    1,
    __mrfstr_base_copy, 8,
    __mrfstr_base_copy, 8,
    __mrfstr_base_fill, 8,
    __mrfstr_base_fill, 8,
    __mrfstr_base_equal, 8,
    __mrfstr_base_tequal, 8,
    __mrfstr_base_contain, 8,
    __mrfstr_base_tcontain, 8
};
#endif

void mrfstr_config_thread_count(
    mrfstr_byte_t thread_count)
{
    mrfstr_config.thread_count = thread_count + 1;
}

void mrfstr_config_copy(
    mrfstr_simd_config_enum_t normal,
    mrfstr_simd_config_enum_t threaded)
{
    switch (normal)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.ncopy_sub = __mrfstr_avx512_copy;
        mrfstr_config.ncopy_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX__
        mrfstr_config.ncopy_sub = __mrfstr_avx_copy;
        mrfstr_config.ncopy_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.ncopy_sub = __mrfstr_sse_copy;
        mrfstr_config.ncopy_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.ncopy_sub = __mrfstr_base_copy;
        mrfstr_config.ncopy_size = 8;
        break;
    }

    switch (threaded)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.tcopy_sub = __mrfstr_avx512_copy;
        mrfstr_config.tcopy_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX__
        mrfstr_config.tcopy_sub = __mrfstr_avx_copy;
        mrfstr_config.tcopy_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.tcopy_sub = __mrfstr_sse_copy;
        mrfstr_config.tcopy_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.tcopy_sub = __mrfstr_base_copy;
        mrfstr_config.tcopy_size = 8;
        break;
    }
}

void mrfstr_config_fill(
    mrfstr_simd_config_enum_t normal,
    mrfstr_simd_config_enum_t threaded)
{
    switch (normal)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.nfill_sub = __mrfstr_avx512_fill;
        mrfstr_config.nfill_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX__
        mrfstr_config.nfill_sub = __mrfstr_avx_fill;
        mrfstr_config.nfill_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.nfill_sub = __mrfstr_sse_fill;
        mrfstr_config.nfill_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.nfill_sub = __mrfstr_base_fill;
        mrfstr_config.nfill_size = 8;
        break;
    }

    switch (threaded)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.tfill_sub = __mrfstr_avx512_fill;
        mrfstr_config.tfill_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX__
        mrfstr_config.tfill_sub = __mrfstr_avx_fill;
        mrfstr_config.tfill_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.tfill_sub = __mrfstr_sse_fill;
        mrfstr_config.tfill_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.tfill_sub = __mrfstr_base_fill;
        mrfstr_config.tfill_size = 8;
        break;
    }
}

void mrfstr_config_equal(
    mrfstr_simd_config_enum_t normal,
    mrfstr_simd_config_enum_t threaded)
{
    switch (normal)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.nequal_sub = __mrfstr_avx512_equal;
        mrfstr_config.nequal_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX2__
        mrfstr_config.nequal_sub = __mrfstr_avx_equal;
        mrfstr_config.nequal_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.nequal_sub = __mrfstr_sse_equal;
        mrfstr_config.nequal_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.nequal_sub = __mrfstr_base_equal;
        mrfstr_config.nequal_size = 8;
        break;
    }

    switch (threaded)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.tequal_sub = __mrfstr_avx512_tequal;
        mrfstr_config.tequal_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX2__
        mrfstr_config.tequal_sub = __mrfstr_avx_tequal;
        mrfstr_config.tequal_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.tequal_sub = __mrfstr_sse_tequal;
        mrfstr_config.tequal_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.tequal_sub = __mrfstr_base_tequal;
        mrfstr_config.tequal_size = 8;
        break;
    }
}

void mrfstr_config_contain(
    mrfstr_simd_config_enum_t normal,
    mrfstr_simd_config_enum_t threaded)
{
    switch (normal)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.ncontain_sub = __mrfstr_avx512_contain;
        mrfstr_config.ncontain_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX2__
        mrfstr_config.ncontain_sub = __mrfstr_avx_contain;
        mrfstr_config.ncontain_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.ncontain_sub = __mrfstr_sse_contain;
        mrfstr_config.ncontain_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.ncontain_sub = __mrfstr_base_contain;
        mrfstr_config.ncontain_size = 8;
        break;
    }

    switch (threaded)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.tcontain_sub = __mrfstr_avx512_tcontain;
        mrfstr_config.tcontain_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX2__
        mrfstr_config.tcontain_sub = __mrfstr_avx_tcontain;
        mrfstr_config.tcontain_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.tcontain_sub = __mrfstr_sse_tcontain;
        mrfstr_config.tcontain_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.tcontain_sub = __mrfstr_base_tcontain;
        mrfstr_config.tcontain_size = 8;
        break;
    }
}
