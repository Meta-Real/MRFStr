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
    __mrfstr_avx512_tequal, 64
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
    __mrfstr_avx_tequal, 32
#elif defined(__SSE2__)
    __mrfstr_sse_equal, 16,
    __mrfstr_sse_tequal, 16
#else
    NULL, 0,
    NULL, 0
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
    __mrfstr_sse_tequal, 16
};
#else
mrfstr_config_t mrfstr_config =
{
    1,
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0,
    NULL, 0
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
        mrfstr_config.ncopy_sub = NULL;
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
        mrfstr_config.tcopy_sub = NULL;
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
        mrfstr_config.nfill_sub = NULL;
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
        mrfstr_config.tfill_sub = NULL;
        break;
    }
}

void mrfstr_config_cmp(
    mrfstr_simd_config_enum_t normal,
    mrfstr_simd_config_enum_t threaded)
{
    switch (normal)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.ncmp_sub = __mrfstr_avx512_equal;
        mrfstr_config.ncmp_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#if  defined(__AVX__) && defined(__AVX2__)
        mrfstr_config.ncmp_sub = __mrfstr_avx_equal;
        mrfstr_config.ncmp_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.ncmp_sub = __mrfstr_sse_equal;
        mrfstr_config.ncmp_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.ncmp_sub = NULL;
        break;
    }

    switch (threaded)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.tcmp_sub = __mrfstr_avx512_tequal;
        mrfstr_config.tcmp_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#if defined(__AVX__) && defined(__AVX2__)
        mrfstr_config.tcmp_sub = __mrfstr_avx_tequal;
        mrfstr_config.tcmp_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.tcmp_sub = __mrfstr_sse_tequal;
        mrfstr_config.tcmp_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.tcmp_sub = NULL;
        break;
    }
}
