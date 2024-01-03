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
    mrfstr_avx512_memcpy_sub, 64,
    mrfstr_avx512_memcpy_sub, 64
};
#elif defined(__AVX__)
mrfstr_config_t mrfstr_config =
{
    1,
    mrfstr_avx_memcpy_sub, 32,
    mrfstr_avx_memcpy_sub, 32
};
#elif defined(__SSE2__)
mrfstr_config_t mrfstr_config =
{
    1,
    mrfstr_sse_memcpy_sub, 16,
    mrfstr_sse_memcpy_sub, 16
};
#else
mrfstr_config_t mrfstr_config =
{
    1,
    memcpy, 1,
    memcpy, 1
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
        mrfstr_config.ncopy_sub = mrfstr_avx512_memcpy_sub;
        mrfstr_config.ncopy_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX__
        mrfstr_config.ncopy_sub = mrfstr_avx_memcpy_sub;
        mrfstr_config.ncopy_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.ncopy_sub = mrfstr_sse_memcpy_sub;
        mrfstr_config.ncopy_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.ncopy_sub = memcpy;
        mrfstr_config.ncopy_size = 1;
        break;
    }

    switch (threaded)
    {
    case MRFSTR_SIMD_CONFIG_AVX512:
#ifdef __AVX512F__
        mrfstr_config.tcopy_sub = mrfstr_avx512_memcpy_sub;
        mrfstr_config.tcopy_size = 64;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_AVX:
#ifdef __AVX__
        mrfstr_config.tcopy_sub = mrfstr_avx_memcpy_sub;
        mrfstr_config.tcopy_size = 32;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_SSE:
#ifdef __SSE2__
        mrfstr_config.tcopy_sub = mrfstr_sse_memcpy_sub;
        mrfstr_config.tcopy_size = 16;
        break;
#endif
    case MRFSTR_SIMD_CONFIG_NONE:
        mrfstr_config.tcopy_sub = memcpy;
        mrfstr_config.tcopy_size = 1;
        break;
    }
}
