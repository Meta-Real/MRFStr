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

#include <avx.h>
#include <binary.h>

#ifdef __AVX__

void __mrfstr_avx_brcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    __m256i *dblock, *sblock, block;

    dblock = (__m256i*)dst;
    sblock = (__m256i*)src;
    while (size--)
    {
        block = _mm256_loadu_si256(--sblock);
        _mm256_store_si256(--dblock, block);
    }
}

void __mrfstr_avx_rcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    __m256i *dblock, *sblock,
        block1, block2, block3, block4;

    dblock = (__m256i*)dst;
    sblock = (__m256i*)src;
    for (; size >= 4; size -= 4)
    {
        block1 = _mm256_loadu_si256(--sblock);
        block2 = _mm256_loadu_si256(--sblock);
        block3 = _mm256_loadu_si256(--sblock);
        block4 = _mm256_loadu_si256(--sblock);
        _mm256_stream_si256(--dblock, block1);
        _mm256_stream_si256(--dblock, block2);
        _mm256_stream_si256(--dblock, block3);
        _mm256_stream_si256(--dblock, block4);
    }

    while (size--)
    {
        block1 = _mm256_loadu_si256(--sblock);
        _mm256_stream_si256(--dblock, block1);
    }
}

void __mrfstr_avx_bfill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *rblock, block;

    rblock = (__m256i*)res;
    block = _mm256_set1_epi8(chr);
    while (size--)
        _mm256_store_si256(rblock++, block);
}

void __mrfstr_avx_fill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *rblock, block;

    rblock = (__m256i*)res;
    block = _mm256_set1_epi8(chr);
    while (size--)
        _mm256_stream_si256(rblock++, block);
}

#ifdef __AVX2__
void __mrfstr_avx_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
{
    __m256i *lblock, *rblock, revidx, block1, block2;

    lblock = (__m256i*)left;
    rblock = (__m256i*)right;

#ifdef __AVX512VBMI__
    revidx = _mm256_set_epi64x(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x1011121314151617, 0x18191a1b1c1d1e1f);
#else
    revidx = _mm256_set_epi64x(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f);
#endif

    while (size--)
    {
        block1 = _mm256_load_si256(lblock);
        block2 = _mm256_loadu_si256(--rblock);

#ifdef __AVX512VBMI__
        block1 = _mm256_permutexvar_epi8(revidx, block1);
        block2 = _mm256_permutexvar_epi8(revidx, block2);
#else
        block1 = _mm256_shuffle_epi8(block1, revidx);
        block1 = _mm256_permute2x128_si256(block1, block1, 1);

        block2 = _mm256_shuffle_epi8(block2, revidx);
        block2 = _mm256_permute2x128_si256(block2, block2, 1);
#endif

        _mm256_store_si256(lblock++, block2);
        _mm256_storeu_si256(rblock, block1);
    }
}

void __mrfstr_avx_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size)
{
    __m256i *lblock, *rblock, revidx, block;

    lblock = (__m256i*)left;
    rblock = (__m256i*)right;

#ifdef __AVX512VBMI__
    revidx = _mm256_set_epi64x(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x1011121314151617, 0x18191a1b1c1d1e1f);
#else
    revidx = _mm256_set_epi64x(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f);
#endif

    while (size--)
    {
        block = _mm256_loadu_si256(--rblock);

#ifdef __AVX512VBMI__
        block = _mm256_permutexvar_epi8(revidx, block);
#else
        block = _mm256_shuffle_epi8(block, revidx);
        block = _mm256_permute2x128_si256(block, block, 1);
#endif

        _mm256_stream_si256(lblock++, block);
    }
}

void __mrfstr_avx_replchr(
    mrfstr_ptr_t str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size)
{
    __m256i *sblock, oblock, nblock, block;
#ifdef __AVX512BW__
    mrfstr_long_t mask;
#else
    __m256i mask;
#endif

    sblock = (__m256i*)str;
    oblock = _mm256_set1_epi8(ochr);
    nblock = _mm256_set1_epi8(nchr);
    for (; size; size--, sblock++)
    {
        block = _mm256_load_si256(sblock);

#ifdef __AVX512BW__
        mask = _mm256_cmpeq_epi8_mask(block, oblock);
        if (mask)
            _mm256_mask_storeu_epi8(sblock, mask, nblock);
#else
        mask = _mm256_cmpeq_epi8(block, oblock);
        block = _mm256_blendv_epi8(block, nblock, mask);
        _mm256_store_si256(sblock, block);
#endif
    }
}

void __mrfstr_avx_replchr2(
    mrfstr_ptr_t res, mrfstr_ptr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size)
{
    __m256i *rblock, *sblock, oblock, nblock, block;
#ifdef __AVX512BW__
    mrfstr_long_t mask;
#else
    __m256i mask;
#endif

    rblock = (__m256i*)res;
    sblock = (__m256i*)str;
    oblock = _mm256_set1_epi8(ochr);
    nblock = _mm256_set1_epi8(nchr);
    while (size--)
    {
        block = _mm256_loadu_si256(sblock++);

#ifdef __AVX512BW__
        mask = _mm256_cmpeq_epi8_mask(block, oblock);
        if (mask)
            block = _mm256_mask_blend_epi8(mask, block, nblock);
#else
        mask = _mm256_cmpeq_epi8(block, oblock);
        block = _mm256_blendv_epi8(block, nblock, mask);
#endif

        _mm256_store_si256(rblock++, block);
    }
}

mrfstr_bool_t __mrfstr_avx_equal(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    __m256i *s1block, *s2block, block1, block2;

    s1block = (__m256i*)str1;
    s2block = (__m256i*)str2;
    while (size--)
    {
        block1 = _mm256_loadu_si256(s1block++);
        block2 = _mm256_loadu_si256(s2block++);

#ifdef __AVX512VL__
        if (_mm256_cmpneq_epi64_mask(block1, block2))
#else
        if (~_mm256_movemask_epi8(_mm256_cmpeq_epi64(block1, block2)))
#endif
            return MRFSTR_FALSE;
    }

    return MRFSTR_TRUE;
}

void __mrfstr_avx_tequal(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    __m256i *s1block, *s2block, block1, block2;
    mrfstr_size_t nsize;

    s1block = (__m256i*)str1;
    s2block = (__m256i*)str2;
    while (size >= MRFSTR_AVX_TEQUAL_LOAD)
    {
        if (!*res)
            return;

        nsize = size - MRFSTR_AVX_TEQUAL_LOAD;
        for (; size != nsize; size--)
        {
            block1 = _mm256_loadu_si256(s1block++);
            block2 = _mm256_loadu_si256(s2block++);

#ifdef __AVX512VL__
            if (_mm256_cmpneq_epi64_mask(block1, block2))
#else
            if (~_mm256_movemask_epi8(_mm256_cmpeq_epi64(block1, block2)))
#endif
            {
                *res = MRFSTR_FALSE;
                return;
            }
        }
    }

    if (!*res)
        return;

    while (size--)
    {
        block1 = _mm256_loadu_si256(s1block++);
        block2 = _mm256_loadu_si256(s2block++);

#ifdef __AVX512VL__
        if (_mm256_cmpneq_epi64_mask(block1, block2))
#else
        if (~_mm256_movemask_epi8(_mm256_cmpeq_epi64(block1, block2)))
#endif
        {
            *res = MRFSTR_FALSE;
            return;
        }
    }
}

mrfstr_bool_t __mrfstr_avx_contchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *sblock, cblock, block1, block2;

    sblock = (__m256i*)str;
    cblock = _mm256_set1_epi8(chr);
    for (; size >= 2; size -= 2)
    {
        block1 = _mm256_load_si256(sblock++);
        block2 = _mm256_load_si256(sblock++);

#ifdef __AVX512BW__
        if (_mm256_cmpeq_epi8_mask(block1, cblock) ||
            _mm256_cmpeq_epi8_mask(block2, cblock))
#else
        if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(block1, cblock)) ||
            _mm256_movemask_epi8(_mm256_cmpeq_epi8(block2, cblock)))
#endif
            return MRFSTR_TRUE;
    }

    if (size)
    {
        block1 = _mm256_load_si256(sblock);

#ifdef __AVX512BW__
        if (_mm256_cmpeq_epi8_mask(block1, cblock))
#else
        if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(block1, cblock)))
#endif
            return MRFSTR_TRUE;
    }

    return MRFSTR_FALSE;
}

void __mrfstr_avx_tcontchr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *sblock, cblock, block;
    mrfstr_size_t nsize;

    sblock = (__m256i*)str;
    cblock = _mm256_set1_epi8(chr);
    while (size >= MRFSTR_AVX_TCONTCHR_LOAD)
    {
        if (*res)
            return;

        nsize = size - MRFSTR_AVX_TCONTCHR_LOAD;
        for (; size != nsize; size--)
        {
            block = _mm256_load_si256(sblock++);

#ifdef __AVX512BW__
            if (_mm256_cmpeq_epi8_mask(block, cblock))
#else
            if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(block, cblock)))
#endif
            {
                *res = MRFSTR_TRUE;
                return;
            }
        }
    }

    if (*res)
        return;

    while (size--)
    {
        block = _mm256_load_si256(sblock++);

#ifdef __AVX512BW__
        if (_mm256_cmpeq_epi8_mask(block, cblock))
#else
        if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(block, cblock)))
#endif
        {
            *res = MRFSTR_TRUE;
            return;
        }
    }
}

mrfstr_idx_t __mrfstr_avx_findchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *sblock, cblock, block;
    mrfstr_size_t i;
    mrfstr_long_t mask;

    sblock = (__m256i*)str;
    cblock = _mm256_set1_epi8(chr);
    for (i = 0; i != size; i++)
    {
        block = _mm256_load_si256(sblock++);

#ifdef __AVX512BW__
        mask = _mm256_cmpeq_epi8_mask(block, cblock);
#else
        mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, cblock));
#endif
        if (mask)
            return (i << 5) + __mrfstr_ctz32(mask);
    }

    return MRFSTR_INVIDX;
}

mrfstr_idx_t __mrfstr_avx_tfindchr(
    volatile mrfstr_idx_t *res, mrfstr_size_t start,
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_short_t step)
{
    __m256i cblock, block;
    mrfstr_long_t mask;

    cblock = _mm256_set1_epi8(chr);
    for (start <<= 5; start < *res; start += step)
    {
        block = _mm256_load_si256((__m256i*)(str + start));

#ifdef __AVX512BW__
        mask = _mm256_cmpeq_epi8_mask(block, cblock);
#else
        mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, cblock));
#endif
        if (mask)
            return start + __mrfstr_ctz32(mask);
    }

    return MRFSTR_INVIDX;
}

mrfstr_size_t __mrfstr_avx_countchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *sblock, cblock, block;
    mrfstr_size_t count;
    mrfstr_long_t mask;

    sblock = (__m256i*)str;
    cblock = _mm256_set1_epi8(chr);
    count = 0;
    while (size--)
    {
        block = _mm256_load_si256(sblock++);

#ifdef __AVX512BW__
        mask = _mm256_cmpeq_epi8_mask(block, cblock);
#else
        mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, cblock));
#endif
        count += __mrfstr_popcnt32(mask);
    }

    return count;
}

mrfstr_size_t __mrfstr_avx_strlen(
    mrfstr_data_ct str)
{
    mrfstr_data_ct base;
    __m256i *sblock, zero, block;
    mrfstr_long_t mask;

    base = str;
    sblock = (__m256i*)str;
    zero = _mm256_setzero_si256();
    for (;; sblock++)
    {
        block = _mm256_load_si256(sblock);

#ifdef __AVX512BW__
        mask = _mm256_cmpeq_epi8_mask(block, zero);
#else
        mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, zero));
#endif
        if (mask)
            return (mrfstr_size_t)(uintptr_t)((mrfstr_data_ct)sblock - base) +
                __mrfstr_ctz32(mask);
    }
}
#endif

#endif
