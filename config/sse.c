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

void __mrfstr_sse_bcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src,
    mrfstr_size_t size)
{
    __m128i *dblock = (__m128i*)dst;
    __m128i *sblock = (__m128i*)src;

    __m128i block;
    while (size--)
    {
        block = _mm_loadu_si128(sblock++);
        _mm_store_si128(dblock++, block);
    }
}

void __mrfstr_sse_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src,
    mrfstr_size_t size)
{
    __m128i *dblock = (__m128i*)dst;
    __m128i *sblock = (__m128i*)src;

    __m128i block1, block2, block3, block4;
    for (; size >= 4; size -= 4)
    {
        block1 = _mm_loadu_si128(sblock++);
        block2 = _mm_loadu_si128(sblock++);
        block3 = _mm_loadu_si128(sblock++);
        block4 = _mm_loadu_si128(sblock++);
        _mm_stream_si128(dblock++, block1);
        _mm_stream_si128(dblock++, block2);
        _mm_stream_si128(dblock++, block3);
        _mm_stream_si128(dblock++, block4);
    }

    while (size--)
    {
        block1 = _mm_loadu_si128(sblock++);
        _mm_stream_si128(dblock++, block1);
    }
}

void __mrfstr_sse_brcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    __m128i *dblock = (__m128i*)dst;
    __m128i *sblock = (__m128i*)src;

    __m128i block;
    while (size--)
    {
        block = _mm_loadu_si128(--sblock);
        _mm_store_si128(--dblock, block);
    }
}

void __mrfstr_sse_rcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    __m128i *dblock = (__m128i*)dst;
    __m128i *sblock = (__m128i*)src;

    __m128i block1, block2, block3, block4,
        block5, block6, block7, block8;
    for (; size >= 8; size -= 8)
    {
        block1 = _mm_loadu_si128(--sblock);
        block2 = _mm_loadu_si128(--sblock);
        block3 = _mm_loadu_si128(--sblock);
        block4 = _mm_loadu_si128(--sblock);
        block5 = _mm_loadu_si128(--sblock);
        block6 = _mm_loadu_si128(--sblock);
        block7 = _mm_loadu_si128(--sblock);
        block8 = _mm_loadu_si128(--sblock);
        _mm_stream_si128(--dblock, block1);
        _mm_stream_si128(--dblock, block2);
        _mm_stream_si128(--dblock, block3);
        _mm_stream_si128(--dblock, block4);
        _mm_stream_si128(--dblock, block5);
        _mm_stream_si128(--dblock, block6);
        _mm_stream_si128(--dblock, block7);
        _mm_stream_si128(--dblock, block8);
    }

    while (size--)
    {
        block1 = _mm_loadu_si128(--sblock);
        _mm_stream_si128(--dblock, block1);
    }
}

void __mrfstr_sse_bfill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m128i *rblock = (__m128i*)res;
    __m128i block = _mm_set1_epi8(chr);

    while (size--)
        _mm_store_si128(rblock++, block);
}

void __mrfstr_sse_fill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m128i *rblock = (__m128i*)res;
    __m128i block = _mm_set1_epi8(chr);

    while (size--)
        _mm_stream_si128(rblock++, block);
}

#ifdef __SSSE3__
void __mrfstr_sse_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
{
    __m128i *lblock = (__m128i*)left;
    __m128i *rblock = (__m128i*)right;

    const __m128i revidx = _mm_set_epi32(
        0x00010203, 0x04050607,
        0x08090a0b, 0x0c0d0e0f);

    __m128i block1, block2;
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
    __m128i *lblock = (__m128i*)left;
    __m128i *rblock = (__m128i*)right;

    const __m128i revidx = _mm_set_epi32(
        0x00010203, 0x04050607,
        0x08090a0b, 0x0c0d0e0f);

    __m128i block;
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
    __m128i *sblock = (__m128i*)str;
    __m128i oblock = _mm_set1_epi8(ochr);
    __m128i nblock = _mm_set1_epi8(nchr);

    __m128i block;
#ifdef __AVX512BW__
    mrfstr_short_t mask;
#else
    __m128i mask;
#endif
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
    __m128i *rblock = (__m128i*)res;
    __m128i *sblock = (__m128i*)str;
    __m128i oblock = _mm_set1_epi8(ochr);
    __m128i nblock = _mm_set1_epi8(nchr);

    __m128i block;
#ifdef __AVX512BW__
    mrfstr_short_t mask;
#else
    __m128i mask;
#endif
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
    __m128i *s1block = (__m128i*)str1;
    __m128i *s2block = (__m128i*)str2;

    __m128i block1, block2;
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
    __m128i *s1block = (__m128i*)str1;
    __m128i *s2block = (__m128i*)str2;

    __m128i block1, block2;
    mrfstr_size_t nsize;
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
    __m128i *sblock = (__m128i*)str;
    __m128i cblock = _mm_set1_epi8(chr);

    __m128i block;
    while (size--)
    {
        block = _mm_loadu_si128(sblock++);

#ifdef __AVX512BW__
        if (_mm_cmpeq_epi8_mask(block, cblock))
#else
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(block, cblock)))
#endif
            return MRFSTR_TRUE;
    }

    return MRFSTR_FALSE;
}

void __mrfstr_sse_tcontchr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m128i *sblock = (__m128i*)str;
    __m128i cblock = _mm_set1_epi8(chr);

    __m128i block;
    mrfstr_size_t nsize;
    while (size >= MRFSTR_SSE_TCONTCHR_LOAD)
    {
        if (*res)
            return;

        nsize = size - MRFSTR_SSE_TCONTCHR_LOAD;
        for (; size != nsize; size--)
        {
            block = _mm_loadu_si128(sblock++);

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
        block = _mm_loadu_si128(sblock++);

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
    __m128i *sblock = (__m128i*)str;
    __m128i cblock = _mm_set1_epi8(chr);

    __m128i block;
    mrfstr_short_t mask;
    mrfstr_size_t i;
    for (i = 0; i != size; i++)
    {
        block = _mm_loadu_si128(sblock++);

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
    volatile mrfstr_idx_t *res, mrfstr_idx_t start,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m128i *sblock = (__m128i*)str;
    __m128i cblock = _mm_set1_epi8(chr);

    __m128i block;
    mrfstr_short_t mask;
    mrfstr_size_t i = 0, ni, lsize = size - MRFSTR_SSE_TFINDCHR_LOAD;
    while (i <= lsize)
    {
        if (*res < start)
            return MRFSTR_INVIDX;

        ni = i + MRFSTR_SSE_TFINDCHR_LOAD;
        for (; i != ni; i++)
        {
            block = _mm_loadu_si128(sblock++);

#ifdef __AVX512BW__
            mask = _mm_cmpeq_epi8_mask(block, cblock);
#else
            mask = (mrfstr_short_t)_mm_movemask_epi8(_mm_cmpeq_epi8(block, cblock));
#endif
            if (mask)
                return start + (i << 4) + __mrfstr_ctz16(mask);
        }
    }

    if (*res < start)
        return MRFSTR_INVIDX;

    for (; i != size; i++)
    {
        block = _mm_loadu_si128(sblock++);

#ifdef __AVX512BW__
        mask = _mm_cmpeq_epi8_mask(block, cblock);
#else
        mask = (mrfstr_short_t)_mm_movemask_epi8(_mm_cmpeq_epi8(block, cblock));
#endif
        if (mask)
            return start + (i << 4) + __mrfstr_ctz16(mask);
    }

    return MRFSTR_INVIDX;
}

mrfstr_size_t __mrfstr_sse_countchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m128i *sblock = (__m128i*)str;
    __m128i cblock = _mm_set1_epi8(chr);

    __m128i block;
    mrfstr_short_t mask;
    mrfstr_size_t count = 0;
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
    mrfstr_ptr_ct str)
{
    mrfstr_data_ct base = (mrfstr_data_ct)str;
    __m128i *sblock = (__m128i*)str;

    __m128i block, zero = _mm_setzero_si128();
    mrfstr_short_t mask;
    for (;; sblock++)
    {
        block = _mm_load_si128(sblock);

#ifdef __AVX512BW__
        mask = _mm_cmpeq_epi8_mask(block, zero);
#else
        mask = (mrfstr_short_t)_mm_movemask_epi8(_mm_cmpeq_epi8(block, zero));
#endif
        if (mask)
            return (mrfstr_size_t)((mrfstr_data_ct)sblock - base) + __mrfstr_ctz16(mask);
    }
}

#endif
