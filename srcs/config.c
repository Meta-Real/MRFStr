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
#include <simd.h>
#include <cpuid.h>

mrfstr_size_t _mrfstr_mem_ntlimit = 1;
mrfstr_size_t _mrfstr_mem_ntrlimit = (mrfstr_size_t)-1;
mrfstr_size_t _mrfstr_cmp_load = 1;
mrfstr_size_t _mrfstr_search_load = 1;

mrfstr_config_t _mrfstr_config =
{
    __mrfstr_i64_copy, __mrfstr_i64_rcopy, __mrfstr_i64_fill,
    __mrfstr_i64_copy, __mrfstr_i64_rcopy, __mrfstr_i64_fill,
    (mrfstr_size_t)-1,
    __mrfstr_i64_replchr, __mrfstr_i64_replchr2,
    __mrfstr_i64_replchr, __mrfstr_i64_replchr2,
    (mrfstr_size_t)-1,
    __mrfstr_i64_rev, __mrfstr_i64_rev2,
    __mrfstr_i64_rev, __mrfstr_i64_rev2,
    (mrfstr_size_t)-1,
    __mrfstr_i64_equal,
    __mrfstr_i64_tequal,
    (mrfstr_size_t)-1,
    __mrfstr_i64_contchr,
    __mrfstr_i64_tcontchr,
    (mrfstr_size_t)-1,
    1,
#ifdef MRFSTR_BUILD_UNIX
    0,
#elif defined(_WIN32)
    THREAD_PRIORITY_NORMAL,
#endif
    64
};

void mrfstr_config_extreme(void);
void mrfstr_config_off(void);

mrfstr_res_t mrfstr_config(
    mrfstr_config_level_t type)
{
    switch (type)
    {
    case MRFSTR_CONFIG_LEVEL_EXTREME:
        mrfstr_config_extreme();
        break;
    case MRFSTR_CONFIG_LEVEL_OFF:
        mrfstr_config_off();
        break;
    default:
        return MRFSTR_RES_TYPE_ERROR;
    }

    return MRFSTR_RES_NOERROR;
}

void mrfstr_config_thread_count(
    mrfstr_byte_t tcount)
{
    if (tcount)
        _mrfstr_config.tcount = tcount;
}

void mrfstr_config_thread_count_max(
    mrfstr_bool_t use_logical)
{
    mrfstr_cpuid_cputype(NULL, NULL);

    if (use_logical)
        mrfstr_cpuid_proccnt(&_mrfstr_config.tcount);
    else
        _mrfstr_config.tcount = mrfstr_cpuid_proccnt(NULL);
}

void mrfstr_config_stdalloc(
    mrfstr_short_t stdalloc)
{
    if (stdalloc)
        _mrfstr_config.stdalloc = stdalloc;
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
        return _mrfstr_config.tcount;
    case MRFSTR_CONFIG_DATA_STDALLOC:
        return _mrfstr_config.stdalloc;
    default:
        return 0;
    }
}


mrfstr_res_t mrfstr_config_func(
    mrfstr_config_func_t type,
    mrfstr_config_simd_t single,
    mrfstr_config_simd_t multi)
{
    mrfstr_cpuid_funccnt(NULL);
    mrfstr_byte_t simdset = mrfstr_cpuid_simdset();

    switch (type)
    {
    case MRFSTR_CONFIG_FUNC_MEMORY:
        switch (single)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512F))
            {
                _mrfstr_config.copy_func = __mrfstr_avx512f_copy;
                _mrfstr_config.rcopy_func = __mrfstr_avx512f_rcopy;
                _mrfstr_config.fill_func = __mrfstr_avx512f_fill;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_AVX:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX))
            {
                _mrfstr_config.copy_func = __mrfstr_avx_copy;
                _mrfstr_config.rcopy_func = __mrfstr_avx_rcopy;
                _mrfstr_config.fill_func = __mrfstr_avx_fill;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_SSE:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE2))
            {
                _mrfstr_config.copy_func = __mrfstr_sse2_copy;
                _mrfstr_config.rcopy_func = __mrfstr_sse2_rcopy;
                _mrfstr_config.fill_func = __mrfstr_sse2_fill;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_INT64:
            _mrfstr_config.copy_func = __mrfstr_i64_copy;
            _mrfstr_config.rcopy_func = __mrfstr_i64_rcopy;
            _mrfstr_config.fill_func = __mrfstr_i64_fill;
            break;
        default:
            return MRFSTR_RES_TYPE_ERROR;
        }
        switch (multi)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512F))
            {
                _mrfstr_config.copy_tfunc = __mrfstr_avx512f_ntcopy;
                _mrfstr_config.rcopy_tfunc = __mrfstr_avx512f_ntrcopy;
                _mrfstr_config.fill_tfunc = __mrfstr_avx512f_ntfill;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_AVX:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX))
            {
                _mrfstr_config.copy_tfunc = __mrfstr_avx_ntcopy;
                _mrfstr_config.rcopy_tfunc = __mrfstr_avx_ntrcopy;
                _mrfstr_config.fill_tfunc = __mrfstr_avx_ntfill;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_SSE:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE2))
            {
                _mrfstr_config.copy_tfunc = __mrfstr_sse2_ntcopy;
                _mrfstr_config.rcopy_tfunc = __mrfstr_sse2_ntrcopy;
                _mrfstr_config.fill_tfunc = __mrfstr_sse2_ntfill;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_INT64:
            _mrfstr_config.copy_tfunc = __mrfstr_i64_copy;
            _mrfstr_config.rcopy_tfunc = __mrfstr_i64_rcopy;
            _mrfstr_config.fill_tfunc = __mrfstr_i64_fill;
            break;
        default:
            return MRFSTR_RES_TYPE_ERROR;
        }
        break;
    case MRFSTR_CONFIG_FUNC_REPLACE:
        switch (single)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
            {
                _mrfstr_config.replchr_func = __mrfstr_avx512bw_replchr;
                _mrfstr_config.replchr2_func = __mrfstr_avx512bw_replchr2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_AVX:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
            {
                _mrfstr_config.replchr_func = __mrfstr_avx2_avx512bw_replchr;
                _mrfstr_config.replchr2_func = __mrfstr_avx2_avx512bw_replchr2;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            {
                _mrfstr_config.replchr_func = __mrfstr_avx2_replchr;
                _mrfstr_config.replchr2_func = __mrfstr_avx2_replchr2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_SSE:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
            {
                _mrfstr_config.replchr_func = __mrfstr_sse41_avx512bw_replchr;
                _mrfstr_config.replchr2_func = __mrfstr_sse41_avx512bw_replchr2;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE41))
            {
                _mrfstr_config.replchr_func = __mrfstr_sse41_replchr;
                _mrfstr_config.replchr2_func = __mrfstr_sse41_replchr2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_INT64:
            _mrfstr_config.replchr_func = __mrfstr_i64_replchr;
            _mrfstr_config.replchr2_func = __mrfstr_i64_replchr2;
            break;
        default:
            return MRFSTR_RES_TYPE_ERROR;
        }
        switch (multi)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
            {
                _mrfstr_config.replchr_tfunc = __mrfstr_avx512bw_replchr;
                _mrfstr_config.replchr2_tfunc = __mrfstr_avx512bw_replchr2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_AVX:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
            {
                _mrfstr_config.replchr_tfunc = __mrfstr_avx2_avx512bw_replchr;
                _mrfstr_config.replchr2_tfunc = __mrfstr_avx2_avx512bw_replchr2;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            {
                _mrfstr_config.replchr_tfunc = __mrfstr_avx2_replchr;
                _mrfstr_config.replchr2_tfunc = __mrfstr_avx2_replchr2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_SSE:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
            {
                _mrfstr_config.replchr_tfunc = __mrfstr_sse41_avx512bw_replchr;
                _mrfstr_config.replchr2_tfunc = __mrfstr_sse41_avx512bw_replchr2;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE41))
            {
                _mrfstr_config.replchr_tfunc = __mrfstr_sse41_replchr;
                _mrfstr_config.replchr2_tfunc = __mrfstr_sse41_replchr2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_INT64:
            _mrfstr_config.replchr_tfunc = __mrfstr_i64_replchr;
            _mrfstr_config.replchr2_tfunc = __mrfstr_i64_replchr2;
            break;
        default:
            return MRFSTR_RES_TYPE_ERROR;
        }
        break;
    case MRFSTR_CONFIG_FUNC_REVERSE:
        switch (single)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512VBMI))
            {
                _mrfstr_config.rev_func = __mrfstr_avx512vbmi_rev;
                _mrfstr_config.rev2_func = __mrfstr_avx512vbmi_rev2;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
            {
                _mrfstr_config.rev_func = __mrfstr_avx512bw_rev;
                _mrfstr_config.rev2_func = __mrfstr_avx512bw_rev2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_AVX:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512VBMI))
            {
                _mrfstr_config.rev_func = __mrfstr_avx2_avx512vbmi_rev;
                _mrfstr_config.rev2_func = __mrfstr_avx2_avx512vbmi_rev2;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            {
                _mrfstr_config.rev_func = __mrfstr_avx2_rev;
                _mrfstr_config.rev2_func = __mrfstr_avx2_rev2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_SSE:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSSE3))
            {
                _mrfstr_config.rev_func = __mrfstr_ssse3_rev;
                _mrfstr_config.rev2_func = __mrfstr_ssse3_rev2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_INT64:
            _mrfstr_config.rev_func = __mrfstr_i64_rev;
            _mrfstr_config.rev2_func = __mrfstr_i64_rev2;
            break;
        default:
            return MRFSTR_RES_TYPE_ERROR;
        }
        switch (multi)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512VBMI))
            {
                _mrfstr_config.rev_tfunc = __mrfstr_avx512vbmi_rev;
                _mrfstr_config.rev2_tfunc = __mrfstr_avx512vbmi_rev2;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
            {
                _mrfstr_config.rev_tfunc = __mrfstr_avx512bw_rev;
                _mrfstr_config.rev2_tfunc = __mrfstr_avx512bw_rev2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_AVX:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512VBMI))
            {
                _mrfstr_config.rev_tfunc = __mrfstr_avx2_avx512vbmi_rev;
                _mrfstr_config.rev2_tfunc = __mrfstr_avx2_avx512vbmi_rev2;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            {
                _mrfstr_config.rev_tfunc = __mrfstr_avx2_rev;
                _mrfstr_config.rev2_tfunc = __mrfstr_avx2_rev2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_SSE:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSSE3))
            {
                _mrfstr_config.rev_tfunc = __mrfstr_ssse3_rev;
                _mrfstr_config.rev2_tfunc = __mrfstr_ssse3_rev2;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_INT64:
            _mrfstr_config.rev_tfunc = __mrfstr_i64_rev;
            _mrfstr_config.rev2_tfunc = __mrfstr_i64_rev2;
            break;
        default:
            return MRFSTR_RES_TYPE_ERROR;
        }
        break;
    case MRFSTR_CONFIG_FUNC_COMPARE:
        switch (single)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512DQ))
            {
                _mrfstr_config.equal_func = __mrfstr_avx512dq_equal;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512F))
            {
                _mrfstr_config.equal_func = __mrfstr_avx512f_equal;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_AVX:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512VL))
            {
                _mrfstr_config.equal_func = __mrfstr_avx2_avx512vl_equal;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            {
                _mrfstr_config.equal_func = __mrfstr_avx2_equal;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_SSE:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512VL))
            {
                _mrfstr_config.equal_func = __mrfstr_sse2_avx512vl_equal;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE2))
            {
                _mrfstr_config.equal_func = __mrfstr_sse2_equal;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_INT64:
            _mrfstr_config.equal_func = __mrfstr_i64_equal;
            break;
        default:
            return MRFSTR_RES_TYPE_ERROR;
        }
        switch (multi)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512DQ))
            {
                _mrfstr_config.equal_tfunc = __mrfstr_avx512dq_tequal;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512F))
            {
                _mrfstr_config.equal_tfunc = __mrfstr_avx512f_tequal;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_AVX:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512VL))
            {
                _mrfstr_config.equal_tfunc = __mrfstr_avx2_avx512vl_tequal;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            {
                _mrfstr_config.equal_tfunc = __mrfstr_avx2_tequal;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_SSE:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512VL))
            {
                _mrfstr_config.equal_tfunc = __mrfstr_sse2_avx512vl_tequal;
                break;
            }
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE2))
            {
                _mrfstr_config.equal_tfunc = __mrfstr_sse2_tequal;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_INT64:
            _mrfstr_config.equal_tfunc = __mrfstr_i64_tequal;
            break;
        default:
            return MRFSTR_RES_TYPE_ERROR;
        }
        break;
    case MRFSTR_CONFIG_FUNC_SEARCH:
        switch (single)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
            {
                _mrfstr_config.contchr_func = __mrfstr_avx512bw_contchr;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_AVX:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            {
                _mrfstr_config.contchr_func = __mrfstr_avx2_contchr;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_SSE:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE2))
            {
                _mrfstr_config.contchr_func = __mrfstr_sse2_contchr;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_INT64:
            _mrfstr_config.contchr_func = __mrfstr_i64_contchr;
            break;
        default:
            return MRFSTR_RES_TYPE_ERROR;
        }
        switch (multi)
        {
        case MRFSTR_CONFIG_SIMD_AVX512:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
            {
                _mrfstr_config.contchr_tfunc = __mrfstr_avx512bw_tcontchr;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_AVX:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            {
                _mrfstr_config.contchr_tfunc = __mrfstr_avx2_tcontchr;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_SSE:
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE2))
            {
                _mrfstr_config.contchr_tfunc = __mrfstr_sse2_tcontchr;
                break;
            }

            return MRFSTR_RES_SUPPORT_ERROR;
        case MRFSTR_CONFIG_SIMD_INT64:
            _mrfstr_config.contchr_tfunc = __mrfstr_i64_tcontchr;
            break;
        default:
            return MRFSTR_RES_TYPE_ERROR;
        }
        break;
    default:
        return MRFSTR_RES_TYPE_ERROR;
    }

    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_config_limits(
    mrfstr_config_func_t type,
    mrfstr_size_t tlimit, mrfstr_size_t limit)
{
    switch (type)
    {
    case MRFSTR_CONFIG_FUNC_MEMORY:
        if (tlimit)
            _mrfstr_config.mem_tlimit = tlimit;
        if (limit)
        {
            mrfstr_byte_t rem;

            rem = limit & MRFSTR_ALIGN_MASK;
            limit -= rem;

            _mrfstr_mem_ntlimit = (mrfstr_size_t)-(mrfstr_ssize_t)limit;
            _mrfstr_mem_ntrlimit = limit;
        }
        break;
    case MRFSTR_CONFIG_FUNC_REPLACE:
        if (tlimit)
            _mrfstr_config.repl_tlimit = tlimit;
        break;
    case MRFSTR_CONFIG_FUNC_REVERSE:
        if (tlimit)
            _mrfstr_config.rev_tlimit = tlimit;
        break;
    case MRFSTR_CONFIG_FUNC_COMPARE:
        if (tlimit)
            _mrfstr_config.cmp_tlimit = tlimit;
        if (limit)
        {
            mrfstr_byte_t rem;

            rem = limit & MRFSTR_ALIGN_MASK;
            _mrfstr_cmp_load = (mrfstr_size_t)-(mrfstr_ssize_t)(limit - rem);
        }
        break;
    case MRFSTR_CONFIG_FUNC_SEARCH:
        if (tlimit)
            _mrfstr_config.search_tlimit = tlimit;
        if (limit)
        {
            mrfstr_byte_t rem;

            rem = limit & MRFSTR_ALIGN_MASK;
            _mrfstr_search_load = (mrfstr_size_t)-(mrfstr_ssize_t)(limit - rem);
        }
        break;
    default:
        return MRFSTR_RES_TYPE_ERROR;
    }

    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_config_limits_get(
    mrfstr_config_func_t type,
    mrfstr_size_t *tlimit, mrfstr_size_t *limit)
{
    switch (type)
    {
    case MRFSTR_CONFIG_FUNC_MEMORY:
        if (tlimit)
            *tlimit = _mrfstr_config.mem_tlimit;
        if (limit)
            *limit = _mrfstr_mem_ntrlimit;
        break;
    case MRFSTR_CONFIG_FUNC_REPLACE:
        if (tlimit)
            *tlimit = _mrfstr_config.repl_tlimit;
        break;
    case MRFSTR_CONFIG_FUNC_REVERSE:
        if (tlimit)
            *tlimit = _mrfstr_config.rev_tlimit;
        break;
    case MRFSTR_CONFIG_FUNC_COMPARE:
        if (tlimit)
            *tlimit = _mrfstr_config.cmp_tlimit;
        if (limit)
            *limit = (mrfstr_size_t)-(mrfstr_ssize_t)_mrfstr_cmp_load;
        break;
    case MRFSTR_CONFIG_FUNC_SEARCH:
        if (tlimit)
            *tlimit = _mrfstr_config.search_tlimit;
        if (limit)
            *limit = (mrfstr_size_t)-(mrfstr_ssize_t)_mrfstr_search_load;
        break;
    default:
        return MRFSTR_RES_TYPE_ERROR;
    }

    return MRFSTR_RES_NOERROR;
}

void mrfstr_config_extreme(void)
{
    mrfstr_byte_t simdset;

    mrfstr_cpuid_cputype(NULL, NULL);
    mrfstr_cpuid_proccnt(&_mrfstr_config.tcount);
    mrfstr_config_thread_priority(MRFSTR_CONFIG_PRIORITY_HIGHEST);

    /* temporary */
    _mrfstr_mem_ntlimit = (mrfstr_size_t)-0x200000;
    _mrfstr_mem_ntrlimit = 0x200000;
    _mrfstr_cmp_load = (mrfstr_size_t)-0x100000;
    _mrfstr_search_load = (mrfstr_size_t)-0x100000;
    _mrfstr_config.mem_tlimit = 0x8000000;
    _mrfstr_config.rev_tlimit = 0x8000000;
    _mrfstr_config.repl_tlimit = 0x8000000;
    _mrfstr_config.cmp_tlimit = 0x8000000;
    _mrfstr_config.search_tlimit = 0x8000000;

    mrfstr_cpuid_funccnt(NULL);
    simdset = mrfstr_cpuid_simdset();

    if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512F))
    {
        _mrfstr_config.copy_func = __mrfstr_avx512f_copy;
        _mrfstr_config.rcopy_func = __mrfstr_avx512f_rcopy;
        _mrfstr_config.fill_func = __mrfstr_avx512f_fill;
        _mrfstr_config.copy_tfunc = __mrfstr_avx512f_ntcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_avx512f_ntrcopy;
        _mrfstr_config.fill_tfunc = __mrfstr_avx512f_ntfill;

        if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512BW))
        {
            _mrfstr_config.replchr_func = __mrfstr_avx512bw_replchr;
            _mrfstr_config.replchr2_func = __mrfstr_avx512bw_replchr2;
            _mrfstr_config.replchr_tfunc = __mrfstr_avx512bw_replchr;
            _mrfstr_config.replchr2_tfunc = __mrfstr_avx512bw_replchr2;

            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX512VBMI))
            {
                _mrfstr_config.rev_func = __mrfstr_avx512vbmi_rev;
                _mrfstr_config.rev2_func = __mrfstr_avx512vbmi_rev2;
                _mrfstr_config.rev_tfunc = __mrfstr_avx512vbmi_rev;
                _mrfstr_config.rev2_tfunc = __mrfstr_avx512vbmi_rev2;
            }
            else
            {
                _mrfstr_config.rev_func = __mrfstr_avx512bw_rev;
                _mrfstr_config.rev2_func = __mrfstr_avx512bw_rev2;
                _mrfstr_config.rev_tfunc = __mrfstr_avx512bw_rev;
                _mrfstr_config.rev2_tfunc = __mrfstr_avx512bw_rev2;
            }

            _mrfstr_config.equal_func = __mrfstr_avx512dq_equal;
            _mrfstr_config.equal_tfunc = __mrfstr_avx512dq_tequal;

            _mrfstr_config.contchr_func = __mrfstr_avx512bw_contchr;
            _mrfstr_config.contchr_tfunc = __mrfstr_avx512bw_tcontchr;
            return;
        }

        _mrfstr_config.replchr_func = __mrfstr_avx2_replchr;
        _mrfstr_config.replchr2_func = __mrfstr_avx2_replchr2;
        _mrfstr_config.replchr_tfunc = __mrfstr_avx2_replchr;
        _mrfstr_config.replchr2_tfunc = __mrfstr_avx2_replchr2;

        _mrfstr_config.rev_func = __mrfstr_avx2_rev;
        _mrfstr_config.rev2_func = __mrfstr_avx2_rev2;
        _mrfstr_config.rev_tfunc = __mrfstr_avx2_rev;
        _mrfstr_config.rev2_tfunc = __mrfstr_avx2_rev2;

        _mrfstr_config.equal_func = __mrfstr_avx512f_equal;
        _mrfstr_config.equal_tfunc = __mrfstr_avx512f_tequal;

        _mrfstr_config.contchr_func = __mrfstr_avx2_contchr;
        _mrfstr_config.contchr_tfunc = __mrfstr_avx2_tcontchr;
        return;
    }
    if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX))
    {
        _mrfstr_config.copy_func = __mrfstr_avx_copy;
        _mrfstr_config.rcopy_func = __mrfstr_avx_rcopy;
        _mrfstr_config.fill_func = __mrfstr_avx_fill;
        _mrfstr_config.copy_tfunc = __mrfstr_avx_ntcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_avx_ntrcopy;
        _mrfstr_config.fill_tfunc = __mrfstr_avx_ntfill;

        if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
        {
            _mrfstr_config.replchr_func = __mrfstr_avx2_replchr;
            _mrfstr_config.replchr2_func = __mrfstr_avx2_replchr2;
            _mrfstr_config.replchr_tfunc = __mrfstr_avx2_replchr;
            _mrfstr_config.replchr2_tfunc = __mrfstr_avx2_replchr2;

            _mrfstr_config.rev_func = __mrfstr_avx2_rev;
            _mrfstr_config.rev2_func = __mrfstr_avx2_rev2;
            _mrfstr_config.rev_tfunc = __mrfstr_avx2_rev;
            _mrfstr_config.rev2_tfunc = __mrfstr_avx2_rev2;

            _mrfstr_config.equal_func = __mrfstr_avx2_equal;
            _mrfstr_config.equal_tfunc = __mrfstr_avx2_tequal;

            _mrfstr_config.contchr_func = __mrfstr_avx2_contchr;
            _mrfstr_config.contchr_tfunc = __mrfstr_avx2_tcontchr;
            return;
        }

        _mrfstr_config.replchr_func = __mrfstr_sse41_replchr;
        _mrfstr_config.replchr2_func = __mrfstr_sse41_replchr2;
        _mrfstr_config.replchr_tfunc = __mrfstr_sse41_replchr;
        _mrfstr_config.replchr2_tfunc = __mrfstr_sse41_replchr2;

        _mrfstr_config.rev_func = __mrfstr_ssse3_rev;
        _mrfstr_config.rev2_func = __mrfstr_ssse3_rev2;
        _mrfstr_config.rev_tfunc = __mrfstr_ssse3_rev;
        _mrfstr_config.rev2_tfunc = __mrfstr_ssse3_rev2;

        _mrfstr_config.contchr_func = __mrfstr_sse2_contchr;
        _mrfstr_config.contchr_tfunc = __mrfstr_sse2_tcontchr;
        return;
    }
    if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE2))
    {
        _mrfstr_config.copy_func = __mrfstr_sse2_copy;
        _mrfstr_config.rcopy_func = __mrfstr_sse2_rcopy;
        _mrfstr_config.fill_func = __mrfstr_sse2_fill;
        _mrfstr_config.copy_tfunc = __mrfstr_sse2_ntcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_sse2_ntrcopy;
        _mrfstr_config.fill_tfunc = __mrfstr_sse2_ntfill;

        if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE41))
        {
            _mrfstr_config.replchr_func = __mrfstr_sse41_replchr;
            _mrfstr_config.replchr2_func = __mrfstr_sse41_replchr2;
            _mrfstr_config.replchr_tfunc = __mrfstr_sse41_replchr;
            _mrfstr_config.replchr2_tfunc = __mrfstr_sse41_replchr2;

            _mrfstr_config.rev_func = __mrfstr_ssse3_rev;
            _mrfstr_config.rev2_func = __mrfstr_ssse3_rev2;
            _mrfstr_config.rev_tfunc = __mrfstr_ssse3_rev;
            _mrfstr_config.rev2_tfunc = __mrfstr_ssse3_rev2;
        }
        else
        {
            _mrfstr_config.replchr_func = __mrfstr_i64_replchr;
            _mrfstr_config.replchr2_func = __mrfstr_i64_replchr2;
            _mrfstr_config.replchr_tfunc = __mrfstr_i64_replchr;
            _mrfstr_config.replchr2_tfunc = __mrfstr_i64_replchr2;

            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSSE3))
            {
                _mrfstr_config.rev_func = __mrfstr_ssse3_rev;
                _mrfstr_config.rev2_func = __mrfstr_ssse3_rev2;
                _mrfstr_config.rev_tfunc = __mrfstr_ssse3_rev;
                _mrfstr_config.rev2_tfunc = __mrfstr_ssse3_rev2;
            }
            else
            {
                _mrfstr_config.rev_func = __mrfstr_i64_rev;
                _mrfstr_config.rev2_func = __mrfstr_i64_rev2;
                _mrfstr_config.rev_tfunc = __mrfstr_i64_rev;
                _mrfstr_config.rev2_tfunc = __mrfstr_i64_rev2;
            }
        }

        _mrfstr_config.equal_func = __mrfstr_sse2_equal;
        _mrfstr_config.equal_tfunc = __mrfstr_sse2_tequal;

        _mrfstr_config.contchr_func = __mrfstr_sse2_contchr;
        _mrfstr_config.contchr_tfunc = __mrfstr_sse2_tcontchr;
        return;
    }

    _mrfstr_config.copy_func = __mrfstr_i64_copy;
    _mrfstr_config.rcopy_func = __mrfstr_i64_rcopy;
    _mrfstr_config.fill_func = __mrfstr_i64_fill;
    _mrfstr_config.copy_tfunc = __mrfstr_i64_copy;
    _mrfstr_config.rcopy_tfunc = __mrfstr_i64_rcopy;
    _mrfstr_config.fill_tfunc = __mrfstr_i64_fill;

    _mrfstr_config.replchr_func = __mrfstr_i64_replchr;
    _mrfstr_config.replchr2_func = __mrfstr_i64_replchr2;
    _mrfstr_config.replchr_tfunc = __mrfstr_i64_replchr;
    _mrfstr_config.replchr2_tfunc = __mrfstr_i64_replchr2;

    _mrfstr_config.rev_func = __mrfstr_i64_rev;
    _mrfstr_config.rev2_func = __mrfstr_i64_rev2;
    _mrfstr_config.rev_tfunc = __mrfstr_i64_rev;
    _mrfstr_config.rev2_tfunc = __mrfstr_i64_rev2;

    _mrfstr_config.equal_func = __mrfstr_i64_equal;
    _mrfstr_config.equal_tfunc = __mrfstr_i64_tequal;

    _mrfstr_config.contchr_func = __mrfstr_i64_contchr;
    _mrfstr_config.contchr_tfunc = __mrfstr_i64_tcontchr;
}

void mrfstr_config_off(void)
{
    _mrfstr_mem_ntlimit = 1;
    _mrfstr_mem_ntrlimit = (mrfstr_size_t)-1;
    _mrfstr_cmp_load = 1;
    _mrfstr_search_load = 1;

    _mrfstr_config = (mrfstr_config_t){
        __mrfstr_i64_copy, __mrfstr_i64_rcopy, __mrfstr_i64_fill,
        __mrfstr_i64_copy, __mrfstr_i64_rcopy, __mrfstr_i64_fill,
        (mrfstr_size_t)-1,
        __mrfstr_i64_replchr, __mrfstr_i64_replchr2,
        __mrfstr_i64_replchr, __mrfstr_i64_replchr2,
        (mrfstr_size_t)-1,
        __mrfstr_i64_rev, __mrfstr_i64_rev2,
        __mrfstr_i64_rev, __mrfstr_i64_rev2,
        (mrfstr_size_t)-1,
        __mrfstr_i64_equal,
        __mrfstr_i64_tequal,
        (mrfstr_size_t)-1,
        __mrfstr_i64_contchr,
        __mrfstr_i64_tcontchr,
        (mrfstr_size_t)-1,
        1,
    #ifdef MRFSTR_BUILD_UNIX
        0,
    #elif defined(_WIN32)
        THREAD_PRIORITY_NORMAL,
    #endif
        64
    };
}
