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
    mrfstr_avx512_copy_sub, 64,
    mrfstr_avx512_copy_sub, 64,
    mrfstr_avx512_fill_sub, 64,
    mrfstr_avx512_fill_sub, 64,
    mrfstr_avx512_cmp_sub, 64,
    mrfstr_avx512_tcmp_sub, 64
};
#elif defined(__AVX__)
mrfstr_config_t mrfstr_config =
{
    1,
    mrfstr_avx_copy_sub, 32,
    mrfstr_avx_copy_sub, 32,
    mrfstr_avx_fill_sub, 32,
    mrfstr_avx_fill_sub, 32,
#ifdef __AVX2__
    mrfstr_avx_cmp_sub, 32,
    mrfstr_avx_tcmp_sub, 32
#elif defined(__SSE2__)
    mrfstr_sse_cmp_sub, 16,
    mrfstr_sse_tcmp_sub, 16
#else
    NULL, 0,
    NULL, 0
#endif
};
#elif defined(__SSE2__)
mrfstr_config_t mrfstr_config =
{
    1,
    mrfstr_sse_copy_sub, 16,
    mrfstr_sse_copy_sub, 16,
    mrfstr_sse_fill_sub, 16,
    mrfstr_sse_fill_sub, 16,
    mrfstr_sse_cmp_sub, 16,
    mrfstr_sse_tcmp_sub, 16
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
        mrfstr_config.ncopy_sub = mrfstr_avx512_copy_sub;
        mrfstr_config.ncopy_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX__
        mrfstr_config.ncopy_sub = mrfstr_avx_copy_sub;
        mrfstr_config.ncopy_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.ncopy_sub = mrfstr_sse_copy_sub;
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
        mrfstr_config.tcopy_sub = mrfstr_avx512_copy_sub;
        mrfstr_config.tcopy_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX__
        mrfstr_config.tcopy_sub = mrfstr_avx_copy_sub;
        mrfstr_config.tcopy_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.tcopy_sub = mrfstr_sse_copy_sub;
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
        mrfstr_config.nfill_sub = mrfstr_avx512_fill_sub;
        mrfstr_config.nfill_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX__
        mrfstr_config.nfill_sub = mrfstr_avx_fill_sub;
        mrfstr_config.nfill_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.nfill_sub = mrfstr_sse_fill_sub;
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
        mrfstr_config.tfill_sub = mrfstr_avx512_fill_sub;
        mrfstr_config.tfill_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX__
        mrfstr_config.tfill_sub = mrfstr_avx_fill_sub;
        mrfstr_config.tfill_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.tfill_sub = mrfstr_sse_fill_sub;
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
        mrfstr_config.ncmp_sub = mrfstr_avx512_cmp_sub;
        mrfstr_config.ncmp_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#if  defined(__AVX__) && defined(__AVX2__)
        mrfstr_config.ncmp_sub = mrfstr_avx_cmp_sub;
        mrfstr_config.ncmp_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.ncmp_sub = mrfstr_sse_cmp_sub;
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
        mrfstr_config.tcmp_sub = mrfstr_avx512_tcmp_sub;
        mrfstr_config.tcmp_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#if defined(__AVX__) && defined(__AVX2__)
        mrfstr_config.tcmp_sub = mrfstr_avx_tcmp_sub;
        mrfstr_config.tcmp_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.tcmp_sub = mrfstr_sse_tcmp_sub;
        mrfstr_config.tcmp_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.tcmp_sub = NULL;
        break;
    }
}
