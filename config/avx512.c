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

#include <avx512.h>
#include <binary.h>

#ifdef __AVX512F__

#ifdef __AVX512BW__
void __mrfstr_avx512_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
{
    __m512i *lblock, *rblock, block1, block2;
#ifdef __AVX512VBMI__
    __m512i revidx;
#else
    __m512i revidx1, revidx2;
#endif

    lblock = (__m512i*)left;
    rblock = (__m512i*)right;

#ifdef __AVX512VBMI__
    revidx = _mm512_set_epi64(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x1011121314151617, 0x18191a1b1c1d1e1f,
        0x2021222324252627, 0x28292a2b2c2d2e2f,
        0x3031323334353637, 0x38393a3b3c3d3e3f);
#else
    revidx1 = _mm512_set_epi64(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f);
    revidx2 = _mm512_set_epi64(1, 0, 3, 2, 5, 4, 7, 6);
#endif

    while (size--)
    {
        block1 = _mm512_load_si512(lblock);
        block2 = _mm512_loadu_si512(--rblock);

#ifdef __AVX512VBMI__
        block1 = _mm512_permutexvar_epi8(revidx, block1);
        block2 = _mm512_permutexvar_epi8(revidx, block2);
#else
        block1 = _mm512_shuffle_epi8(block1, revidx1);
        block1 = _mm512_permutexvar_epi64(revidx2, block1);

        block2 = _mm512_shuffle_epi8(block2, revidx1);
        block2 = _mm512_permutexvar_epi64(revidx2, block2);
#endif

        _mm512_store_si512(lblock++, block2);
        _mm512_storeu_si512(rblock, block1);
    }
}

void __mrfstr_avx512_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size)
{
    __m512i *lblock, *rblock, block;
#ifdef __AVX512VBMI__
    __m512i revidx;
#else
    __m512i revidx1, revidx2;
#endif

    lblock = (__m512i*)left;
    rblock = (__m512i*)right;

#ifdef __AVX512VBMI__
    revidx = _mm512_set_epi64(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x1011121314151617, 0x18191a1b1c1d1e1f,
        0x2021222324252627, 0x28292a2b2c2d2e2f,
        0x3031323334353637, 0x38393a3b3c3d3e3f);
#else
    revidx1 = _mm512_set_epi64(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f);
    revidx2 = _mm512_set_epi64(1, 0, 3, 2, 5, 4, 7, 6);
#endif

    while (size--)
    {
        block = _mm512_loadu_si512(--rblock);

#ifdef __AVX512VBMI__
        block = _mm512_permutexvar_epi8(revidx, block);
#else
        block = _mm512_shuffle_epi8(block, revidx1);
        block = _mm512_permutexvar_epi64(revidx2, block);
#endif

        _mm512_stream_si512(lblock++, block);
    }
}

void __mrfstr_avx512_replchr(
    mrfstr_ptr_t str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size)
{
    __m512i *sblock, oblock, nblock, block;
    mrfstr_longlong_t mask;

    sblock = (__m512i*)str;
    oblock = _mm512_set1_epi8(ochr);
    nblock = _mm512_set1_epi8(nchr);
    while (size--)
    {
        block = _mm512_load_si512(sblock);

        mask = _mm512_cmpeq_epi8_mask(block, oblock);
        if (mask)
            _mm512_mask_storeu_epi8(sblock++, mask, nblock);
    }
}

void __mrfstr_avx512_replchr2(
    mrfstr_ptr_t res, mrfstr_ptr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size)
{
    __m512i *rblock, *sblock, oblock, nblock, block;
    mrfstr_longlong_t mask;

    rblock = (__m512i*)res;
    sblock = (__m512i*)str;
    oblock = _mm512_set1_epi8(ochr);
    nblock = _mm512_set1_epi8(nchr);
    while (size--)
    {
        block = _mm512_loadu_si512(sblock++);

        mask = _mm512_cmpeq_epi8_mask(block, oblock);
        if (mask)
            block = _mm512_mask_blend_epi8(mask, block, nblock);

        _mm512_store_si512(rblock++, block);
    }
}
#endif

mrfstr_bool_t __mrfstr_avx512_equal(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    __m512i *s1block, *s2block, block1, block2;

    s1block = (__m512i*)str1;
    s2block = (__m512i*)str2;
    while (size--)
    {
        block1 = _mm512_loadu_si512(s1block++);
        block2 = _mm512_loadu_si512(s2block++);
        if (_mm512_cmpneq_epi64_mask(block1, block2))
            return MRFSTR_FALSE;
    }

    return MRFSTR_TRUE;
}

void __mrfstr_avx512_tequal(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    __m512i *s1block, *s2block, block1, block2;
    mrfstr_size_t nsize;

    s1block = (__m512i*)str1;
    s2block = (__m512i*)str2;
    while (size >= MRFSTR_AVX512_TEQUAL_LOAD)
    {
        if (!*res)
            return;

        nsize = size - MRFSTR_AVX512_TEQUAL_LOAD;
        for (; size != nsize; size--)
        {
            block1 = _mm512_loadu_si512(s1block++);
            block2 = _mm512_loadu_si512(s2block++);
            if (_mm512_cmpneq_epi64_mask(block1, block2))
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
        block1 = _mm512_loadu_si512(s1block++);
        block2 = _mm512_loadu_si512(s2block++);
        if (_mm512_cmpneq_epi64_mask(block1, block2))
        {
            *res = MRFSTR_FALSE;
            return;
        }
    }
}

mrfstr_bool_t __mrfstr_avx512_contchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *sblock, cblock, block;

    sblock = (__m512i*)str;
    cblock = _mm512_set1_epi8(chr);
    while (size--)
    {
        block = _mm512_load_si512(sblock++);
        if (_mm512_cmpeq_epi8_mask(block, cblock))
            return MRFSTR_TRUE;
    }

    return MRFSTR_FALSE;
}

void __mrfstr_avx512_tcontchr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *sblock, cblock, block;
    mrfstr_size_t nsize;

    sblock = (__m512i*)str;
    cblock = _mm512_set1_epi8(chr);
    while (size >= MRFSTR_AVX512_TCONTCHR_LOAD)
    {
        if (*res)
            return;

        nsize = size - MRFSTR_AVX512_TCONTCHR_LOAD;
        for (; size != nsize; size--)
        {
            block = _mm512_load_si512(sblock++);
            if (_mm512_cmpeq_epi8_mask(block, cblock))
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
        block = _mm512_load_si512(sblock++);
        if (_mm512_cmpeq_epi8_mask(block, cblock))
        {
            *res = MRFSTR_TRUE;
            return;
        }
    }
}

mrfstr_idx_t __mrfstr_avx512_findchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *sblock, cblock, block;
    mrfstr_longlong_t mask;
    mrfstr_size_t i;

    sblock = (__m512i*)str;
    cblock = _mm512_set1_epi8(chr);
    for (i = 0; i != size; i++)
    {
        block = _mm512_load_si512(sblock++);

        mask = _mm512_cmpeq_epi8_mask(block, cblock);
        if (mask)
            return (i << 6) + __mrfstr_ctz64(mask);
    }

    return MRFSTR_INVIDX;
}

mrfstr_idx_t __mrfstr_avx512_tfindchr(
    volatile mrfstr_idx_t *res, mrfstr_size_t start,
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_short_t step)
{
    __m512i cblock, block;
    mrfstr_longlong_t mask;

    cblock = _mm512_set1_epi8(chr);
    for (start <<= 6; start < *res; start += step)
    {
        block = _mm512_load_si512(str + start);

        mask = _mm512_cmpeq_epi8_mask(block, cblock);
        if (mask)
            return start + __mrfstr_ctz64(mask);
    }

    return MRFSTR_INVIDX;
}

mrfstr_size_t __mrfstr_avx512_countchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *sblock, cblock, block;
    mrfstr_longlong_t mask;
    mrfstr_size_t count;

    sblock = (__m512i*)str;
    cblock = _mm512_set1_epi8(chr);
    count = 0;
    while (size--)
    {
        block = _mm512_load_si512(sblock++);
        mask = _mm512_cmpeq_epi8_mask(block, cblock);
        count += __mrfstr_popcnt64(mask);
    }

    return count;
}

mrfstr_size_t __mrfstr_avx512_strlen(
    mrfstr_data_ct str)
{
    mrfstr_data_ct base;
    __m512i *sblock, zero, block;
    mrfstr_longlong_t mask;

    base = str;
    sblock = (__m512i*)str;
    zero = _mm512_setzero_si512();
    for (;; sblock++)
    {
        block = _mm512_load_si512(sblock);

        mask = _mm512_cmpeq_epi8_mask(block, zero);
        if (mask)
            return (mrfstr_size_t)(uintptr_t)((mrfstr_data_ct)sblock - base) +
                __mrfstr_ctz64(mask);
    }
}

#endif
