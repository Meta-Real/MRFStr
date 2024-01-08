/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

#ifdef __AVX512F__
mrfstr_config_t _mrfstr_config =
{
    1,
    __mrfstr_avx512_copy, __mrfstr_avx512_fill, 64,
    __mrfstr_avx512_copy, __mrfstr_avx512_fill, 64,
#ifdef __AVX512BW__
    __mrfstr_avx512_rev, __mrfstr_avx512_rev2, 64,
    __mrfstr_avx512_rev, __mrfstr_avx512_rev2, 64,
    __mrfstr_avx512_replchr, __mrfstr_avx512_replchr2, 64,
    __mrfstr_avx512_replchr, __mrfstr_avx512_replchr2, 64,
#else
    __mrfstr_avx_rev, __mrfstr_avx_rev2, 32,
    __mrfstr_avx_rev, __mrfstr_avx_rev2, 32,
    __mrfstr_avx_replchr, __mrfstr_avx_replchr2, 32,
    __mrfstr_avx_replchr, __mrfstr_avx_replchr2, 32,
#endif
    __mrfstr_avx512_equal, 64,
    __mrfstr_avx512_tequal, 64,
    __mrfstr_avx512_contchr, __mrfstr_avx512_findchr, 64,
    __mrfstr_avx512_tcontchr, __mrfstr_avx512_tfindchr, 64,
    __mrfstr_avx512_strlen, 64
};
#elif defined(__AVX__)
mrfstr_config_t _mrfstr_config =
{
    1,
    __mrfstr_avx_copy, __mrfstr_avx_fill, 32,
    __mrfstr_avx_copy, __mrfstr_avx_fill, 32,
#ifdef __AVX2__
    __mrfstr_avx_rev, __mrfstr_avx_rev2, 32,
    __mrfstr_avx_rev, __mrfstr_avx_rev2, 32,
    __mrfstr_avx_replchr, __mrfstr_avx_replchr2, 32,
    __mrfstr_avx_replchr, __mrfstr_avx_replchr2, 32,
    __mrfstr_avx_equal, 32,
    __mrfstr_avx_tequal, 32,
    __mrfstr_avx_contchr, __mrfstr_avx_findchr, 32,
    __mrfstr_avx_tcontchr, __mrfstr_avx_tfindchr, 32,
    __mrfstr_avx_strlen, 32
#else
    __mrfstr_sse_rev, __mrfstr_sse_rev2, 16,
    __mrfstr_sse_rev, __mrfstr_sse_rev2, 16,
    __mrfstr_sse_replchr, __mrfstr_sse_replchr2, 16,
    __mrfstr_sse_replchr, __mrfstr_sse_replchr2, 16,
    __mrfstr_sse_equal, 16,
    __mrfstr_sse_tequal, 16,
    __mrfstr_sse_contchr, __mrfstr_sse_findchr, 16,
    __mrfstr_sse_tcontchr, __mrfstr_sse_tfindchr, 16,
    __mrfstr_sse_strlen, 16
#endif
};
#elif defined(__SSE2__)
mrfstr_config_t _mrfstr_config =
{
    1,
    __mrfstr_sse_copy, __mrfstr_sse_fill, 16,
    __mrfstr_sse_copy, __mrfstr_sse_fill, 16,
#ifdef __SSSE3__
    __mrfstr_sse_rev, __mrfstr_sse_rev2, 16,
    __mrfstr_sse_rev, __mrfstr_sse_rev2, 16,
#else
    __mrfstr_base_rev, __mrfstr_base_rev2, 8,
    __mrfstr_base_rev, __mrfstr_base_rev2, 8,
#endif
#ifdef __SSE4_1__
    __mrfstr_sse_replchr, __mrfstr_sse_replchr2, 16,
    __mrfstr_sse_replchr, __mrfstr_sse_replchr2, 16,
#else
    __mrfstr_base_replchr, __mrfstr_base_replchr2, 8,
    __mrfstr_base_replchr, __mrfstr_base_replchr2, 8,
#endif
    __mrfstr_sse_equal, 16,
    __mrfstr_sse_tequal, 16,
    __mrfstr_sse_contchr, __mrfstr_sse_findchr, 16,
    __mrfstr_sse_tcontchr, __mrfstr_sse_tfindchr, 16,
    __mrfstr_sse_strlen, 16
};
#else
mrfstr_config_t _mrfstr_config =
{
    1,
    __mrfstr_base_copy, __mrfstr_base_fill, 8,
    __mrfstr_base_copy, __mrfstr_base_fill, 8,
    __mrfstr_base_rev, __mrfstr_base_rev2, 8,
    __mrfstr_base_rev, __mrfstr_base_rev2, 8,
    __mrfstr_base_replchr, __mrfstr_base_replchr2, 8,
    __mrfstr_base_replchr, __mrfstr_base_replchr2, 8,
    __mrfstr_base_equal, 8,
    __mrfstr_base_tequal, 8,
    __mrfstr_base_contchr, __mrfstr_base_findchr, 8,
    __mrfstr_base_tcontchr, __mrfstr_base_tfindchr, 8,
    __mrfstr_base_strlen, 8
};
#endif

void mrfstr_config_tcount(
    mrfstr_byte_t tcount)
{
    _mrfstr_config.tcount = tcount + 1;
}

void mrfstr_config(
    mrfstr_config_type_enum_t type,
    mrfstr_config_simd_enum_t normal,
    mrfstr_config_simd_enum_t threaded)
{
    switch (type)
    {
    case MRFSTR_CONFIG_TYPE_MEMORY:
        switch (normal)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512F__
            _mrfstr_config.ncopy_sub = __mrfstr_avx512_copy;
            _mrfstr_config.nfill_sub = __mrfstr_avx512_fill;
            _mrfstr_config.nmem_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX__
            _mrfstr_config.ncopy_sub = __mrfstr_avx_copy;
            _mrfstr_config.nfill_sub = __mrfstr_avx_fill;
            _mrfstr_config.nmem_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE2__
            _mrfstr_config.ncopy_sub = __mrfstr_sse_copy;
            _mrfstr_config.nfill_sub = __mrfstr_sse_fill;
            _mrfstr_config.nmem_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.ncopy_sub = __mrfstr_base_copy;
            _mrfstr_config.nfill_sub = __mrfstr_base_fill;
            _mrfstr_config.nmem_size = 8;
            break;
        }
        switch (threaded)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512F__
            _mrfstr_config.tcopy_sub = __mrfstr_avx512_copy;
            _mrfstr_config.tfill_sub = __mrfstr_avx512_fill;
            _mrfstr_config.tmem_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX__
            _mrfstr_config.tcopy_sub = __mrfstr_avx_copy;
            _mrfstr_config.tfill_sub = __mrfstr_avx_fill;
            _mrfstr_config.tmem_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE2__
            _mrfstr_config.tcopy_sub = __mrfstr_sse_copy;
            _mrfstr_config.tfill_sub = __mrfstr_sse_fill;
            _mrfstr_config.tmem_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.tcopy_sub = __mrfstr_base_copy;
            _mrfstr_config.tfill_sub = __mrfstr_base_fill;
            _mrfstr_config.tmem_size = 8;
            break;
        }
        break;
    case MRFSTR_CONFIG_TYPE_REV:
        switch (normal)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512BW__
            _mrfstr_config.nrev_sub = __mrfstr_avx512_rev;
            _mrfstr_config.nrev2_sub = __mrfstr_avx512_rev2;
            _mrfstr_config.nrev_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.nrev_sub = __mrfstr_avx_rev;
            _mrfstr_config.nrev2_sub = __mrfstr_avx_rev2;
            _mrfstr_config.nrev_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE4_1__
            _mrfstr_config.nrev_sub = __mrfstr_sse_rev;
            _mrfstr_config.nrev2_sub = __mrfstr_sse_rev2;
            _mrfstr_config.nrev_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.nrev_sub = __mrfstr_base_rev;
            _mrfstr_config.nrev2_sub = __mrfstr_base_rev2;
            _mrfstr_config.nrev_size = 8;
            break;
        }
        switch (threaded)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512BW__
            _mrfstr_config.trev_sub = __mrfstr_avx512_rev;
            _mrfstr_config.trev2_sub = __mrfstr_avx512_rev2;
            _mrfstr_config.trev_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.trev_sub = __mrfstr_avx_rev;
            _mrfstr_config.trev2_sub = __mrfstr_avx_rev2;
            _mrfstr_config.trev_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE4_1__
            _mrfstr_config.trev_sub = __mrfstr_sse_rev;
            _mrfstr_config.trev2_sub = __mrfstr_sse_rev2;
            _mrfstr_config.trev_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.trev_sub = __mrfstr_base_rev;
            _mrfstr_config.trev2_sub = __mrfstr_base_rev2;
            _mrfstr_config.trev_size = 8;
            break;
        }
        break;
    case MRFSTR_CONFIG_TYPE_REPLACE:
        switch (normal)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512BW__
            _mrfstr_config.nreplchr_sub = __mrfstr_avx512_replchr;
            _mrfstr_config.nreplchr2_sub = __mrfstr_avx512_replchr2;
            _mrfstr_config.nrepl_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.nreplchr_sub = __mrfstr_avx_replchr;
            _mrfstr_config.nreplchr2_sub = __mrfstr_avx_replchr2;
            _mrfstr_config.nrepl_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE4_1__
            _mrfstr_config.nreplchr_sub = __mrfstr_sse_replchr;
            _mrfstr_config.nreplchr2_sub = __mrfstr_sse_replchr2;
            _mrfstr_config.nrepl_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.nreplchr_sub = __mrfstr_base_replchr;
            _mrfstr_config.nreplchr2_sub = __mrfstr_base_replchr2;
            _mrfstr_config.nrepl_size = 8;
            break;
        }
        switch (threaded)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512BW__
            _mrfstr_config.treplchr_sub = __mrfstr_avx512_replchr;
            _mrfstr_config.treplchr2_sub = __mrfstr_avx512_replchr2;
            _mrfstr_config.trepl_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.treplchr_sub = __mrfstr_avx_replchr;
            _mrfstr_config.treplchr2_sub = __mrfstr_avx_replchr2;
            _mrfstr_config.trepl_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE4_1__
            _mrfstr_config.treplchr_sub = __mrfstr_sse_replchr;
            _mrfstr_config.treplchr2_sub = __mrfstr_sse_replchr2;
            _mrfstr_config.trepl_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.treplchr_sub = __mrfstr_base_replchr;
            _mrfstr_config.treplchr2_sub = __mrfstr_base_replchr2;
            _mrfstr_config.trepl_size = 8;
            break;
        }
        break;
    case MRFSTR_CONFIG_TYPE_CMP:
        switch (normal)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512F__
            _mrfstr_config.nequal_sub = __mrfstr_avx512_equal;
            _mrfstr_config.ncmp_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.nequal_sub = __mrfstr_avx_equal;
            _mrfstr_config.ncmp_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE2__
            _mrfstr_config.nequal_sub = __mrfstr_sse_equal;
            _mrfstr_config.ncmp_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.nequal_sub = __mrfstr_base_equal;
            _mrfstr_config.ncmp_size = 8;
            break;
        }
        switch (threaded)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512F__
            _mrfstr_config.tequal_sub = __mrfstr_avx512_tequal;
            _mrfstr_config.tcmp_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.tequal_sub = __mrfstr_avx_tequal;
            _mrfstr_config.tcmp_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE2__
            _mrfstr_config.tequal_sub = __mrfstr_sse_tequal;
            _mrfstr_config.tcmp_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.tequal_sub = __mrfstr_base_tequal;
            _mrfstr_config.tcmp_size = 8;
            break;
        }
        break;
    case MRFSTR_CONFIG_TYPE_SEARCH:
        switch (normal)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512F__
            _mrfstr_config.ncontchr_sub = __mrfstr_avx512_contchr;
            _mrfstr_config.nfindchr_sub = __mrfstr_avx512_findchr;
            _mrfstr_config.nsearch_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.ncontchr_sub = __mrfstr_avx_contchr;
            _mrfstr_config.nfindchr_sub = __mrfstr_avx_findchr;
            _mrfstr_config.nsearch_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE2__
            _mrfstr_config.ncontchr_sub = __mrfstr_sse_contchr;
            _mrfstr_config.nfindchr_sub = __mrfstr_sse_findchr;
            _mrfstr_config.nsearch_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.ncontchr_sub = __mrfstr_base_contchr;
            _mrfstr_config.nfindchr_sub = __mrfstr_base_findchr;
            _mrfstr_config.nsearch_size = 8;
            break;
        }
        switch (threaded)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512F__
            _mrfstr_config.tcontchr_sub = __mrfstr_avx512_tcontchr;
            _mrfstr_config.tfindchr_sub = __mrfstr_avx512_tfindchr;
            _mrfstr_config.tsearch_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.tcontchr_sub = __mrfstr_avx_tcontchr;
            _mrfstr_config.tfindchr_sub = __mrfstr_avx_tfindchr;
            _mrfstr_config.tsearch_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE2__
            _mrfstr_config.tcontchr_sub = __mrfstr_sse_tcontchr;
            _mrfstr_config.tfindchr_sub = __mrfstr_sse_tfindchr;
            _mrfstr_config.tsearch_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.tcontchr_sub = __mrfstr_base_tcontchr;
            _mrfstr_config.tfindchr_sub = __mrfstr_base_tfindchr;
            _mrfstr_config.tsearch_size = 8;
            break;
        }
        break;
    }
}

void mrfstr_config_str(
    mrfstr_config_type_enum_t type,
    mrfstr_config_simd_enum_t simd)
{
    switch (type)
    {
    case MRFSTR_CONFIG_TYPE_STRLEN:
        switch (simd)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512F__
            _mrfstr_config.strlen_sub = __mrfstr_avx512_strlen;
            _mrfstr_config.strlen_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.strlen_sub = __mrfstr_avx_strlen;
            _mrfstr_config.strlen_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE2__
            _mrfstr_config.strlen_sub = __mrfstr_sse_strlen;
            _mrfstr_config.strlen_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.strlen_sub = __mrfstr_base_strlen;
            _mrfstr_config.strlen_size = 8;
            break;
        }
        break;
    }
}
