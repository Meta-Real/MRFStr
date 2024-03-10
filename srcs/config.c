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

mrfstr_size_t _mrfstr_mem_ntlimit = (mrfstr_size_t)-1;

mrfstr_config_t _mrfstr_config =
{
    __mrfstr_i64_copy, __mrfstr_i64_rcopy, __mrfstr_i64_fill,
    __mrfstr_i64_copy, __mrfstr_i64_rcopy, __mrfstr_i64_fill,
    (mrfstr_size_t)-1,
    __mrfstr_i64_rev, __mrfstr_i64_rev2,
    __mrfstr_i64_rev, __mrfstr_i64_rev2,
    (mrfstr_size_t)-1,
    1,
#ifdef MRFSTR_BUILD_UNIX
    0,
#elif defined(_WIN32)
    THREAD_PRIORITY_NORMAL,
#endif
    64
};

void mrfstr_config_func_all(
    mrfstr_config_level_t type);

mrfstr_res_t mrfstr_config(
    mrfstr_config_level_t type)
{
    mrfstr_cpuid_cputype(NULL, NULL);

    switch (type)
    {
    case MRFSTR_CONFIG_LEVEL_EXTREME:
        mrfstr_cpuid_proccnt(&_mrfstr_config.tcount);
        mrfstr_config_thread_priority(MRFSTR_CONFIG_PRIORITY_HIGHEST);
        mrfstr_config_func_all(type);
        /* temporary */
        _mrfstr_mem_ntlimit = 0x200000;
        _mrfstr_config.mem_tlimit = 0x8000000;
        _mrfstr_config.rev_tlimit = 0x8000000;
        break;
    case MRFSTR_CONFIG_LEVEL_OFF:
        _mrfstr_config.tcount = 1;
        mrfstr_config_thread_priority(MRFSTR_CONFIG_PRIORITY_NORMAL);
        mrfstr_config_func_all(type);
        _mrfstr_mem_ntlimit = (mrfstr_size_t)-1;
        _mrfstr_config.mem_tlimit = (mrfstr_size_t)-1;
        _mrfstr_config.rev_tlimit = (mrfstr_size_t)-1;
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
        return _mrfstr_config.tcount - 1;
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
            return MRFSTR_RES_SUPPORT_ERROR;
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
            return MRFSTR_RES_SUPPORT_ERROR;
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
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            {
                _mrfstr_config.rev_func = __mrfstr_avx2_rev;
                _mrfstr_config.rev2_func = __mrfstr_avx2_rev2;
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
            return MRFSTR_RES_SUPPORT_ERROR;
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
            if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            {
                _mrfstr_config.rev_tfunc = __mrfstr_avx2_rev;
                _mrfstr_config.rev2_tfunc = __mrfstr_avx2_rev2;
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
            return MRFSTR_RES_SUPPORT_ERROR;
        }
        break;
    default:
        return MRFSTR_RES_TYPE_ERROR;
    }

    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_config_limits(
    mrfstr_config_func_t type,
    mrfstr_size_t tlimit, mrfstr_size_t ntlimit)
{
    switch (type)
    {
    case MRFSTR_CONFIG_FUNC_MEMORY:
        if (tlimit)
            _mrfstr_config.mem_tlimit = tlimit;
        if (ntlimit)
            _mrfstr_mem_ntlimit = ntlimit;
        break;
    case MRFSTR_CONFIG_FUNC_REVERSE:
        if (tlimit)
            _mrfstr_config.rev_tlimit = tlimit;
        break;
    default:
        return MRFSTR_RES_TYPE_ERROR;
    }

    return MRFSTR_RES_NOERROR;
}

mrfstr_res_t mrfstr_config_limits_get(
    mrfstr_config_func_t type,
    mrfstr_size_t *tlimit, mrfstr_size_t *ntlimit)
{
    switch (type)
    {
    case MRFSTR_CONFIG_FUNC_MEMORY:
        if (tlimit)
            *tlimit = _mrfstr_config.mem_tlimit;
        if (ntlimit)
            *ntlimit = _mrfstr_mem_ntlimit;
        break;
    case MRFSTR_CONFIG_FUNC_REVERSE:
        if (tlimit)
            *tlimit = _mrfstr_config.rev_tlimit;
        break;
    default:
        return MRFSTR_RES_TYPE_ERROR;
    }

    return MRFSTR_RES_NOERROR;
}

void mrfstr_config_func_all(
    mrfstr_config_level_t type)
{
    mrfstr_cpuid_funccnt(NULL);
    mrfstr_byte_t simdset = mrfstr_cpuid_simdset();

    switch (type)
    {
    case MRFSTR_CONFIG_LEVEL_EXTREME:
    case MRFSTR_CONFIG_LEVEL_EFFICIENT:
        break;
    case MRFSTR_CONFIG_LEVEL_MODERATE:
        if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX2))
            simdset = MRFSTR_SIMD_AVX2;
        else if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_AVX))
            simdset = MRFSTR_SIMD_AVX;
        else if (MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE2))
            simdset = MRFSTR_SIMD_SSE2;
        else
            simdset = MRFSTR_SIMD_INT64;
        break;
    case MRFSTR_CONFIG_LEVEL_LIMITED:
        simdset = MRFSTR_SIMD_SSE2 * MRFSTR_SUPPORTS_SIMD(MRFSTR_SIMD_SSE2);
        break;
    case MRFSTR_CONFIG_LEVEL_OFF:
        simdset = MRFSTR_SIMD_INT64;
        break;
    }

    switch (simdset)
    {
    case MRFSTR_SIMD_AVX512VBMI:
        _mrfstr_config.copy_func = __mrfstr_avx512f_copy;
        _mrfstr_config.copy_tfunc = __mrfstr_avx512f_ntcopy;
        _mrfstr_config.rcopy_func = __mrfstr_avx512f_rcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_avx512f_ntrcopy;
        _mrfstr_config.fill_func = __mrfstr_avx512f_fill;
        _mrfstr_config.fill_tfunc = __mrfstr_avx512f_ntfill;
        _mrfstr_config.rev_func = __mrfstr_avx512vbmi_rev;
        _mrfstr_config.rev_tfunc = __mrfstr_avx512vbmi_rev;
        _mrfstr_config.rev2_func = __mrfstr_avx512vbmi_rev2;
        _mrfstr_config.rev2_tfunc = __mrfstr_avx512vbmi_rev2;
        break;
    case MRFSTR_SIMD_AVX512BW:
        _mrfstr_config.copy_func = __mrfstr_avx512f_copy;
        _mrfstr_config.copy_tfunc = __mrfstr_avx512f_ntcopy;
        _mrfstr_config.rcopy_func = __mrfstr_avx512f_rcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_avx512f_ntrcopy;
        _mrfstr_config.fill_func = __mrfstr_avx512f_fill;
        _mrfstr_config.fill_tfunc = __mrfstr_avx512f_ntfill;
        _mrfstr_config.rev_func = __mrfstr_avx512bw_rev;
        _mrfstr_config.rev_tfunc = __mrfstr_avx512bw_rev;
        _mrfstr_config.rev2_func = __mrfstr_avx512bw_rev2;
        _mrfstr_config.rev2_tfunc = __mrfstr_avx512bw_rev2;
        break;
    case MRFSTR_SIMD_AVX512F:
        _mrfstr_config.copy_func = __mrfstr_avx512f_copy;
        _mrfstr_config.copy_tfunc = __mrfstr_avx512f_ntcopy;
        _mrfstr_config.rcopy_func = __mrfstr_avx512f_rcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_avx512f_ntrcopy;
        _mrfstr_config.fill_func = __mrfstr_avx512f_fill;
        _mrfstr_config.fill_tfunc = __mrfstr_avx512f_ntfill;
        _mrfstr_config.rev_func = __mrfstr_avx2_rev;
        _mrfstr_config.rev_tfunc = __mrfstr_avx2_rev;
        _mrfstr_config.rev2_func = __mrfstr_avx2_rev2;
        _mrfstr_config.rev2_tfunc = __mrfstr_avx2_rev2;
        break;
    case MRFSTR_SIMD_AVX2:
        _mrfstr_config.copy_func = __mrfstr_avx_copy;
        _mrfstr_config.copy_tfunc = __mrfstr_avx_ntcopy;
        _mrfstr_config.rcopy_func = __mrfstr_avx_rcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_avx_ntrcopy;
        _mrfstr_config.fill_func = __mrfstr_avx_fill;
        _mrfstr_config.fill_tfunc = __mrfstr_avx_ntfill;
        _mrfstr_config.rev_func = __mrfstr_avx2_rev;
        _mrfstr_config.rev_tfunc = __mrfstr_avx2_rev;
        _mrfstr_config.rev2_func = __mrfstr_avx2_rev2;
        _mrfstr_config.rev2_tfunc = __mrfstr_avx2_rev2;
        break;
    case MRFSTR_SIMD_AVX:
        _mrfstr_config.copy_func = __mrfstr_avx_copy;
        _mrfstr_config.copy_tfunc = __mrfstr_avx_ntcopy;
        _mrfstr_config.rcopy_func = __mrfstr_avx_rcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_avx_ntrcopy;
        _mrfstr_config.fill_func = __mrfstr_avx_fill;
        _mrfstr_config.fill_tfunc = __mrfstr_avx_ntfill;
        _mrfstr_config.rev_func = __mrfstr_ssse3_rev;
        _mrfstr_config.rev_tfunc = __mrfstr_ssse3_rev;
        _mrfstr_config.rev2_func = __mrfstr_ssse3_rev2;
        _mrfstr_config.rev2_tfunc = __mrfstr_ssse3_rev2;
        break;
    case MRFSTR_SIMD_SSE4_1:
    case MRFSTR_SIMD_SSSE3:
        _mrfstr_config.copy_func = __mrfstr_sse2_copy;
        _mrfstr_config.copy_tfunc = __mrfstr_sse2_ntcopy;
        _mrfstr_config.rcopy_func = __mrfstr_sse2_rcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_sse2_ntrcopy;
        _mrfstr_config.fill_func = __mrfstr_sse2_fill;
        _mrfstr_config.fill_tfunc = __mrfstr_sse2_ntfill;
        _mrfstr_config.rev_func = __mrfstr_ssse3_rev;
        _mrfstr_config.rev_tfunc = __mrfstr_ssse3_rev;
        _mrfstr_config.rev2_func = __mrfstr_ssse3_rev2;
        _mrfstr_config.rev2_tfunc = __mrfstr_ssse3_rev2;
        break;
    case MRFSTR_SIMD_SSE2:
        _mrfstr_config.copy_func = __mrfstr_sse2_copy;
        _mrfstr_config.copy_tfunc = __mrfstr_sse2_ntcopy;
        _mrfstr_config.rcopy_func = __mrfstr_sse2_rcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_sse2_ntrcopy;
        _mrfstr_config.fill_func = __mrfstr_sse2_fill;
        _mrfstr_config.fill_tfunc = __mrfstr_sse2_ntfill;
        _mrfstr_config.rev_func = __mrfstr_i64_rev;
        _mrfstr_config.rev_tfunc = __mrfstr_i64_rev;
        _mrfstr_config.rev2_func = __mrfstr_i64_rev2;
        _mrfstr_config.rev2_tfunc = __mrfstr_i64_rev2;
        break;
    case MRFSTR_SIMD_INT64:
        _mrfstr_config.copy_func = __mrfstr_i64_copy;
        _mrfstr_config.copy_tfunc = __mrfstr_i64_copy;
        _mrfstr_config.rcopy_func = __mrfstr_i64_rcopy;
        _mrfstr_config.rcopy_tfunc = __mrfstr_i64_rcopy;
        _mrfstr_config.fill_func = __mrfstr_i64_fill;
        _mrfstr_config.fill_tfunc = __mrfstr_i64_fill;
        _mrfstr_config.rev_func = __mrfstr_i64_rev;
        _mrfstr_config.rev_tfunc = __mrfstr_i64_rev;
        _mrfstr_config.rev2_func = __mrfstr_i64_rev2;
        _mrfstr_config.rev2_tfunc = __mrfstr_i64_rev2;
        break;
    }
}
