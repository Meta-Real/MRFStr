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

#include <mrfstr-intern.h>
#include <cpuid/cpuid.h>
#include <string.h>

mrfstr_config_t _mrfstr_config =
{
    1, 0x200000, 0x8000000, 64,
#ifdef __AVX512F__
    __mrfstr_avx512_copy, __mrfstr_avx512_ntcopy,
    __mrfstr_avx512_brcopy, __mrfstr_avx512_rcopy,
    __mrfstr_avx512_bfill, __mrfstr_avx512_fill, 64,
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
    __mrfstr_avx512_contchr, __mrfstr_avx512_findchr,
    __mrfstr_avx512_countchr, 64,
    __mrfstr_avx512_tcontchr, __mrfstr_avx512_tfindchr,
    __mrfstr_avx512_countchr, 64,
    __mrfstr_avx512_strlen, 64,
#elif defined(__AVX__)
    __mrfstr_avx_copy, __mrfstr_avx_ntcopy,
    __mrfstr_avx_brcopy, __mrfstr_avx_rcopy,
    __mrfstr_avx_bfill, __mrfstr_avx_fill, 32,
    __mrfstr_avx_copy, __mrfstr_avx_fill, 32,
#ifdef __AVX2__
    __mrfstr_avx_rev, __mrfstr_avx_rev2, 32,
    __mrfstr_avx_rev, __mrfstr_avx_rev2, 32,
    __mrfstr_avx_replchr, __mrfstr_avx_replchr2, 32,
    __mrfstr_avx_replchr, __mrfstr_avx_replchr2, 32,
    __mrfstr_avx_equal, 32,
    __mrfstr_avx_tequal, 32,
    __mrfstr_avx_contchr, __mrfstr_avx_findchr,
    __mrfstr_avx_countchr, 32,
    __mrfstr_avx_tcontchr, __mrfstr_avx_tfindchr,
    __mrfstr_avx_countchr, 32,
    __mrfstr_avx_strlen, 32
#else
    __mrfstr_sse_rev, __mrfstr_sse_rev2, 16,
    __mrfstr_sse_rev, __mrfstr_sse_rev2, 16,
    __mrfstr_sse_replchr, __mrfstr_sse_replchr2, 16,
    __mrfstr_sse_replchr, __mrfstr_sse_replchr2, 16,
    __mrfstr_sse_equal, 16,
    __mrfstr_sse_tequal, 16,
    __mrfstr_sse_contchr, __mrfstr_sse_findchr,
    __mrfstr_sse_countchr, 16,
    __mrfstr_sse_tcontchr, __mrfstr_sse_tfindchr,
    __mrfstr_sse_countchr, 16,
    __mrfstr_sse_strlen, 16,
#endif
#elif defined(__SSE2__)
    __mrfstr_sse_copy, __mrfstr_sse_ntcopy,
    __mrfstr_sse_brcopy, __mrfstr_sse_rcopy,
    __mrfstr_sse_bfill, __mrfstr_sse_fill, 16,
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
    __mrfstr_sse_contchr, __mrfstr_sse_findchr,
    __mrfstr_sse_countchr, 16,
    __mrfstr_sse_tcontchr, __mrfstr_sse_tfindchr,
    __mrfstr_sse_countchr, 16,
    __mrfstr_sse_strlen, 16,
#else
    __mrfstr_base_copy, __mrfstr_base_copy,
    __mrfstr_base_rcopy, __mrfstr_base_rcopy,
    __mrfstr_base_fill, __mrfstr_base_fill, 8,
    __mrfstr_base_copy, __mrfstr_base_fill, 8,
    __mrfstr_base_rev, __mrfstr_base_rev2, 8,
    __mrfstr_base_rev, __mrfstr_base_rev2, 8,
    __mrfstr_base_replchr, __mrfstr_base_replchr2, 8,
    __mrfstr_base_replchr, __mrfstr_base_replchr2, 8,
    __mrfstr_base_equal, 8,
    __mrfstr_base_tequal, 8,
    __mrfstr_base_contchr, __mrfstr_base_findchr,
    __mrfstr_base_countchr, 8,
    __mrfstr_base_tcontchr, __mrfstr_base_tfindchr,
    __mrfstr_base_countchr, 8,
    __mrfstr_base_strlen, 8,
#endif
#ifdef MRFSTR_BUILD_UNIX
    0
#elif defined(_WIN32)
    THREAD_PRIORITY_NORMAL
#endif
};

void mrfstr_config_thread_count(
    mrfstr_byte_t tcount)
{
    if (tcount == 255)
        tcount = 254;
    _mrfstr_config.tcount = tcount + 1;
}

void mrfstr_config_normal_limit(
    mrfstr_size_t nlimit)
{
    if (nlimit < MRFSTR_SLIMIT)
        nlimit = MRFSTR_SLIMIT;

    _mrfstr_config.nlimit = nlimit;
    if (_mrfstr_config.tlimit < nlimit)
        _mrfstr_config.tlimit = nlimit;
}

void mrfstr_config_thread_limit(
    mrfstr_size_t tlimit)
{
    if (tlimit < MRFSTR_SLIMIT)
        tlimit = MRFSTR_SLIMIT;

    _mrfstr_config.tlimit = tlimit;
    if (_mrfstr_config.nlimit > tlimit)
        _mrfstr_config.nlimit = tlimit;
}

void mrfstr_config_stdalloc(
    mrfstr_short_t stdalloc)
{
    if (!stdalloc)
        stdalloc = 1;
    _mrfstr_config.stdalloc = stdalloc;
}

void mrfstr_config_thread_count_max(
    mrfstr_bool_t use_logical)
{
    if (use_logical)
        mrfstr_cpuid_proccnt(&_mrfstr_config.tcount);
    else
        _mrfstr_config.tcount = mrfstr_cpuid_proccnt(NULL);
}

mrfstr_res_t mrfstr_config_thread_priority(
    mrfstr_config_priority_t priority)
{
#ifdef MRFSTR_BUILD_UNIX
    int policy;
    mrfstr_byte_t pmax, pmin;
    pthread_attr_t attr;

    if (priority == MRFSTR_CONFIG_PRIORITY_NORMAL)
    {
        _mrfstr_config.tprior = 0;
        return MRFSTR_RES_NOERROR;
    }

    if (pthread_attr_init(&attr))
        return MRFSTR_RES_MEM_ERROR;

    policy = 0;
    pthread_attr_getschedpolicy(&attr, &policy);
    pthread_attr_destroy(&attr);

    pmin = sched_get_priority_max(policy);
    pmax = sched_get_priority_min(policy);

    switch (priority)
    {
    case MRFSTR_CONFIG_PRIORITY_LOWEST:
        _mrfstr_config.tprior = pmin;
        break;
    case MRFSTR_CONFIG_PRIORITY_LOW:
        _mrfstr_config.tprior = (mrfstr_byte_t)
            ((((mrfstr_short_t)pmin << 1) + pmax) / 3);
        break;
    case MRFSTR_CONFIG_PRIORITY_HIGH:
        _mrfstr_config.tprior = (mrfstr_byte_t)
            ((pmin + ((mrfstr_short_t)pmax << 1)) / 3);
        break;
    case MRFSTR_CONFIG_PRIORITY_HIGHEST:
        _mrfstr_config.tprior = pmax;
        break;
    }
#elif defined(_WIN32)
    switch (priority)
    {
    case MRFSTR_CONFIG_PRIORITY_LOWEST:
        _mrfstr_config.tprior = THREAD_PRIORITY_LOWEST;
        break;
    case MRFSTR_CONFIG_PRIORITY_LOW:
        _mrfstr_config.tprior = THREAD_PRIORITY_BELOW_NORMAL;
        break;
    case MRFSTR_CONFIG_PRIORITY_NORMAL:
        _mrfstr_config.tprior = THREAD_PRIORITY_NORMAL;
        break;
    case MRFSTR_CONFIG_PRIORITY_HIGH:
        _mrfstr_config.tprior = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    case MRFSTR_CONFIG_PRIORITY_HIGHEST:
        _mrfstr_config.tprior = THREAD_PRIORITY_HIGHEST;
        break;
    }
#endif

    return MRFSTR_RES_NOERROR;
}

mrfstr_size_t mrfstr_config_get(
    mrfstr_config_data_t type)
{
    switch (type)
    {
    case MRFSTR_CONFIG_DATA_THREAD_COUNT:
        return _mrfstr_config.tcount - 1;
    case MRFSTR_CONFIG_DATA_NORMAL_LIMIT:
        return _mrfstr_config.nlimit;
    case MRFSTR_CONFIG_DATA_THREAD_LIMIT:
        return _mrfstr_config.tlimit;
    case MRFSTR_CONFIG_DATA_STDALLOC:
        return _mrfstr_config.stdalloc;
    default:
        return 0;
    }
}

void mrfstr_config(
    mrfstr_config_type_t type,
    mrfstr_config_simd_t normal,
    mrfstr_config_simd_t threaded)
{
    switch (type)
    {
    case MRFSTR_CONFIG_TYPE_MEMORY:
        switch (normal)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512F__
            _mrfstr_config.bcopy_sub = __mrfstr_avx512_copy;
            _mrfstr_config.ncopy_sub = __mrfstr_avx512_ntcopy;
            _mrfstr_config.brcopy_sub = __mrfstr_avx512_brcopy;
            _mrfstr_config.nrcopy_sub = __mrfstr_avx512_rcopy;
            _mrfstr_config.bfill_sub = __mrfstr_avx512_bfill;
            _mrfstr_config.nfill_sub = __mrfstr_avx512_fill;
            _mrfstr_config.nmem_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX__
            _mrfstr_config.bcopy_sub = __mrfstr_avx_copy;
            _mrfstr_config.ncopy_sub = __mrfstr_avx_ntcopy;
            _mrfstr_config.brcopy_sub = __mrfstr_avx_brcopy;
            _mrfstr_config.nrcopy_sub = __mrfstr_avx_rcopy;
            _mrfstr_config.bfill_sub = __mrfstr_avx_bfill;
            _mrfstr_config.nfill_sub = __mrfstr_avx_fill;
            _mrfstr_config.nmem_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE2__
            _mrfstr_config.bcopy_sub = __mrfstr_sse_copy;
            _mrfstr_config.ncopy_sub = __mrfstr_sse_ntcopy;
            _mrfstr_config.brcopy_sub = __mrfstr_sse_brcopy;
            _mrfstr_config.nrcopy_sub = __mrfstr_sse_rcopy;
            _mrfstr_config.bfill_sub = __mrfstr_sse_bfill;
            _mrfstr_config.nfill_sub = __mrfstr_sse_fill;
            _mrfstr_config.nmem_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.bcopy_sub = __mrfstr_base_copy;
            _mrfstr_config.ncopy_sub = __mrfstr_base_copy;
            _mrfstr_config.brcopy_sub = __mrfstr_base_rcopy;
            _mrfstr_config.nrcopy_sub = __mrfstr_base_rcopy;
            _mrfstr_config.bfill_sub = __mrfstr_base_fill;
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
    case MRFSTR_CONFIG_TYPE_REVERSE:
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
    case MRFSTR_CONFIG_TYPE_COMPARE:
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
            _mrfstr_config.ncountchr_sub = __mrfstr_avx512_countchr;
            _mrfstr_config.nsearch_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.ncontchr_sub = __mrfstr_avx_contchr;
            _mrfstr_config.nfindchr_sub = __mrfstr_avx_findchr;
            _mrfstr_config.ncountchr_sub = __mrfstr_avx_countchr;
            _mrfstr_config.nsearch_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE2__
            _mrfstr_config.ncontchr_sub = __mrfstr_sse_contchr;
            _mrfstr_config.nfindchr_sub = __mrfstr_sse_findchr;
            _mrfstr_config.ncountchr_sub = __mrfstr_sse_countchr;
            _mrfstr_config.nsearch_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.ncontchr_sub = __mrfstr_base_contchr;
            _mrfstr_config.nfindchr_sub = __mrfstr_base_findchr;
            _mrfstr_config.ncountchr_sub = __mrfstr_base_countchr;
            _mrfstr_config.nsearch_size = 8;
            break;
        }
        switch (threaded)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
#ifdef __AVX512F__
            _mrfstr_config.tcontchr_sub = __mrfstr_avx512_tcontchr;
            _mrfstr_config.tfindchr_sub = __mrfstr_avx512_tfindchr;
            _mrfstr_config.tcountchr_sub = __mrfstr_avx512_countchr;
            _mrfstr_config.tsearch_size = 64;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_AVX:
#ifdef __AVX2__
            _mrfstr_config.tcontchr_sub = __mrfstr_avx_tcontchr;
            _mrfstr_config.tfindchr_sub = __mrfstr_avx_tfindchr;
            _mrfstr_config.tcountchr_sub = __mrfstr_avx_countchr;
            _mrfstr_config.tsearch_size = 32;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_SSE:
#ifdef __SSE2__
            _mrfstr_config.tcontchr_sub = __mrfstr_sse_tcontchr;
            _mrfstr_config.tfindchr_sub = __mrfstr_sse_tfindchr;
            _mrfstr_config.tcountchr_sub = __mrfstr_sse_countchr;
            _mrfstr_config.tsearch_size = 16;
            break;
#endif
        case MRFSTR_CONFIG_SIMD_NONE:
            _mrfstr_config.tcontchr_sub = __mrfstr_base_tcontchr;
            _mrfstr_config.tfindchr_sub = __mrfstr_base_tfindchr;
            _mrfstr_config.tcountchr_sub = __mrfstr_base_countchr;
            _mrfstr_config.tsearch_size = 8;
            break;
        }
        break;
    }
}

void mrfstr_config_str(
    mrfstr_config_type_t type,
    mrfstr_config_simd_t simd)
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
