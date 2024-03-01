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

#include <sse.h>
#include <binary.h>

#ifdef __SSE2__

#ifdef __SSSE3__
void __mrfstr_sse_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
{
    __m128i *lblock, *rblock, revidx, block1, block2;

    lblock = (__m128i*)left;
    rblock = (__m128i*)right;

    revidx = _mm_set_epi32(
        0x00010203, 0x04050607,
        0x08090a0b, 0x0c0d0e0f);
    while (size--)
    {
        block1 = _mm_load_si128(lblock);
        block2 = _mm_loadu_si128(--rblock);

#ifdef __AVX512VBMI__
        block1 = _mm_permutexvar_epi8(revidx, block1);
        block2 = _mm_permutexvar_epi8(revidx, block2);
#else
        block1 = _mm_shuffle_epi8(block1, revidx);
        block2 = _mm_shuffle_epi8(block2, revidx);
#endif

        _mm_store_si128(lblock++, block2);
        _mm_storeu_si128(rblock, block1);
    }
}

void __mrfstr_sse_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right,
    mrfstr_size_t size)
{
    __m128i *lblock, *rblock, revidx, block;

    lblock = (__m128i*)left;
    rblock = (__m128i*)right;

    revidx = _mm_set_epi32(
        0x00010203, 0x04050607,
        0x08090a0b, 0x0c0d0e0f);
    while (size--)
    {
        block = _mm_loadu_si128(--rblock);

#ifdef __AVX512VBMI__
        block = _mm_permutexvar_epi8(revidx, block);
#else
        block = _mm_shuffle_epi8(block, revidx);
#endif

        _mm_stream_si128(lblock++, block);
    }
}
#endif

#ifdef __SSE4_1__
void __mrfstr_sse_replchr(
    mrfstr_ptr_t str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size)
{
    __m128i *sblock, oblock, nblock, block;
#ifdef __AVX512BW__
    mrfstr_short_t mask;
#else
    __m128i mask;
#endif

    sblock = (__m128i*)str;
    oblock = _mm_set1_epi8(ochr);
    nblock = _mm_set1_epi8(nchr);
    for (; size; size--, sblock++)
    {
        block = _mm_load_si128(sblock);

#ifdef __AVX512BW__
        mask = _mm_cmpeq_epi8_mask(block, oblock);
        if (mask)
            _mm_mask_storeu_epi8(sblock, mask, nblock);
#else
        mask = _mm_cmpeq_epi8(block, oblock);
        block = _mm_blendv_epi8(block, nblock, mask);
        _mm_store_si128(sblock, block);
#endif
    }
}

void __mrfstr_sse_replchr2(
    mrfstr_ptr_t res, mrfstr_ptr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size)
{
    __m128i *rblock, *sblock, oblock, nblock, block;
#ifdef __AVX512BW__
    mrfstr_short_t mask;
#else
    __m128i mask;
#endif

    rblock = (__m128i*)res;
    sblock = (__m128i*)str;
    oblock = _mm_set1_epi8(ochr);
    nblock = _mm_set1_epi8(nchr);
    while (size--)
    {
        block = _mm_loadu_si128(sblock++);

#ifdef __AVX512BW__
        mask = _mm_cmpeq_epi8_mask(block, oblock);
        if (mask)
            block = _mm_mask_blend_epi8(mask, block, nblock);
#else
        mask = _mm_cmpeq_epi8(block, oblock);
        block = _mm_blendv_epi8(block, nblock, mask);
#endif

        _mm_store_si128(rblock++, block);
    }
}
#endif

mrfstr_bool_t __mrfstr_sse_equal(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    __m128i *s1block, *s2block, block1, block2;

    s1block = (__m128i*)str1;
    s2block = (__m128i*)str2;
    while (size--)
    {
        block1 = _mm_loadu_si128(s1block++);
        block2 = _mm_loadu_si128(s2block++);

#ifdef __AVX512VL__
        if (_mm_cmpneq_epi64_mask(block1, block2))
#else
        if (~(mrfstr_sshort_t)_mm_movemask_epi8(_mm_cmpeq_epi32(block1, block2)))
#endif
            return MRFSTR_FALSE;
    }

    return MRFSTR_TRUE;
}

void __mrfstr_sse_tequal(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    __m128i *s1block, *s2block, block1, block2;
    mrfstr_size_t nsize;

    s1block = (__m128i*)str1;
    s2block = (__m128i*)str2;
    while (size >= MRFSTR_SSE_TEQUAL_LOAD)
    {
        if (!*res)
            return;

        nsize = size - MRFSTR_SSE_TEQUAL_LOAD;
        for (; size != nsize; size--)
        {
            block1 = _mm_loadu_si128(s1block++);
            block2 = _mm_loadu_si128(s2block++);

#ifdef __AVX512VL__
            if (_mm_cmpneq_epi64_mask(block1, block2))
#else
            if (~(mrfstr_sshort_t)_mm_movemask_epi8(_mm_cmpeq_epi32(block1, block2)))
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
        block1 = _mm_loadu_si128(s1block++);
        block2 = _mm_loadu_si128(s2block++);

#ifdef __AVX512VL__
        if (_mm_cmpneq_epi64_mask(block1, block2))
#else
        if (~(mrfstr_sshort_t)_mm_movemask_epi8(_mm_cmpeq_epi32(block1, block2)))
#endif
        {
            *res = MRFSTR_FALSE;
            return;
        }
    }
}

mrfstr_bool_t __mrfstr_sse_contchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m128i *sblock, cblock,
        block1, block2, block3, block4;

    sblock = (__m128i*)str;
    cblock = _mm_set1_epi8(chr);
    for (; size >= 4; size -= 4)
    {
        block1 = _mm_load_si128(sblock++);
        block2 = _mm_load_si128(sblock++);
        block3 = _mm_load_si128(sblock++);
        block4 = _mm_load_si128(sblock++);

#ifdef __AVX512BW__
        if (_mm_cmpeq_epi8_mask(block1, cblock) ||
            _mm_cmpeq_epi8_mask(block2, cblock) ||
            _mm_cmpeq_epi8_mask(block3, cblock) ||
            _mm_cmpeq_epi8_mask(block4, cblock))
#else
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(block1, cblock)) ||
            _mm_movemask_epi8(_mm_cmpeq_epi8(block2, cblock)) ||
            _mm_movemask_epi8(_mm_cmpeq_epi8(block3, cblock)) ||
            _mm_movemask_epi8(_mm_cmpeq_epi8(block4, cblock)))
#endif
            return MRFSTR_TRUE;
    }

    while (size--)
    {
        block1 = _mm_load_si128(sblock++);

#ifdef __AVX512BW__
        if (_mm_cmpeq_epi8_mask(block1, cblock))
#else
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(block1, cblock)))
#endif
            return MRFSTR_TRUE;
    }

    return MRFSTR_FALSE;
}

void __mrfstr_sse_tcontchr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m128i *sblock, cblock, block;
    mrfstr_size_t nsize;

    sblock = (__m128i*)str;
    cblock = _mm_set1_epi8(chr);
    while (size >= MRFSTR_SSE_TCONTCHR_LOAD)
    {
        if (*res)
            return;

        nsize = size - MRFSTR_SSE_TCONTCHR_LOAD;
        for (; size != nsize; size--)
        {
            block = _mm_load_si128(sblock++);

#ifdef __AVX512BW__
            if (_mm_cmpeq_epi8_mask(block, cblock))
#else
            if (_mm_movemask_epi8(_mm_cmpeq_epi8(block, cblock)))
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
        block = _mm_load_si128(sblock++);

#ifdef __AVX512BW__
        if (_mm_cmpeq_epi8_mask(block, cblock))
#else
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(block, cblock)))
#endif
        {
            *res = MRFSTR_TRUE;
            return;
        }
    }
}

mrfstr_idx_t __mrfstr_sse_findchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m128i *sblock, cblock, block;
    mrfstr_size_t i;
    mrfstr_short_t mask;

    sblock = (__m128i*)str;
    cblock = _mm_set1_epi8(chr);
    for (i = 0; i != size; i++)
    {
        block = _mm_load_si128(sblock++);

#ifdef __AVX512BW__
        mask = _mm_cmpeq_epi8_mask(block, cblock);
#else
        mask = (mrfstr_short_t)_mm_movemask_epi8(_mm_cmpeq_epi8(block, cblock));
#endif
        if (mask)
            return (i << 4) + __mrfstr_ctz16(mask);
    }

    return MRFSTR_INVIDX;
}

mrfstr_idx_t __mrfstr_sse_tfindchr(
    volatile mrfstr_idx_t *res, mrfstr_size_t start,
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_short_t step)
{
    __m128i cblock, block;
    mrfstr_short_t mask;

    cblock = _mm_set1_epi8(chr);
    for (start <<= 4; start < *res; start += step)
    {
        block = _mm_load_si128((__m128i*)(str + start));

#ifdef __AVX512BW__
        mask = _mm_cmpeq_epi8_mask(block, cblock);
#else
        mask = (mrfstr_short_t)_mm_movemask_epi8(_mm_cmpeq_epi8(block, cblock));
#endif
        if (mask)
            return start + __mrfstr_ctz16(mask);
    }

    return MRFSTR_INVIDX;
}

mrfstr_size_t __mrfstr_sse_countchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m128i *sblock, cblock, block;
    mrfstr_size_t count;
    mrfstr_short_t mask;

    sblock = (__m128i*)str;
    cblock = _mm_set1_epi8(chr);
    count = 0;
    while (size--)
    {
        block = _mm_load_si128(sblock++);
#ifdef __AVX512BW__
        mask = _mm_cmpeq_epi8_mask(block, cblock);
#else
        mask = (mrfstr_short_t)_mm_movemask_epi8(_mm_cmpeq_epi8(block, cblock));
#endif
        count += __mrfstr_popcnt32(mask);
    }

    return count;
}

mrfstr_size_t __mrfstr_sse_strlen(
    mrfstr_data_ct str)
{
    mrfstr_data_ct base;
    __m128i *sblock, zero, block;
    mrfstr_short_t mask;

    base = str;
    sblock = (__m128i*)str;
    zero = _mm_setzero_si128();
    for (;; sblock++)
    {
        block = _mm_load_si128(sblock);

#ifdef __AVX512BW__
        mask = _mm_cmpeq_epi8_mask(block, zero);
#else
        mask = (mrfstr_short_t)_mm_movemask_epi8(_mm_cmpeq_epi8(block, zero));
#endif
        if (mask)
            return (mrfstr_size_t)(uintptr_t)((mrfstr_data_ct)sblock - base) +
                __mrfstr_ctz16(mask);
    }
}

#endif
