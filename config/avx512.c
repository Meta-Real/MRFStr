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

void __mrfstr_avx512_bcopy(
    restrict mrfstr_ptr_t dst, restrict mrfstr_ptr_ct src,
    mrfstr_size_t size)
{
    __m512i *dblock = (__m512i*)dst;
    __m512i *sblock = (__m512i*)src;

    __m512i block;
    while (size--)
    {
        block = _mm512_loadu_si512(sblock++);
        _mm512_store_si512(dblock++, block);
    }
}

void __mrfstr_avx512_copy(
    restrict mrfstr_ptr_t dst, restrict mrfstr_ptr_ct src,
    mrfstr_size_t size)
{
    __m512i *dblock = (__m512i*)dst;
    __m512i *sblock = (__m512i*)src;

    __m512i block;
    while (size--)
    {
        block = _mm512_loadu_si512(sblock++);
        _mm512_stream_si512(dblock++, block);
    }
}

void __mrfstr_avx512_brcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    __m512i *dblock = (__m512i*)dst;
    __m512i *sblock = (__m512i*)src;

    __m512i block;
    while (size--)
    {
        block = _mm512_loadu_si512(--sblock);
        _mm512_store_si512(--dblock, block);
    }
}

void __mrfstr_avx512_rcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    __m512i *dblock = (__m512i*)dst;
    __m512i *sblock = (__m512i*)src;

    __m512i block1, block2;
    for (; size >= 2; size -= 2)
    {
        block1 = _mm512_loadu_si512(--sblock);
        block2 = _mm512_loadu_si512(--sblock);
        _mm512_stream_si512(--dblock, block1);
        _mm512_stream_si512(--dblock, block2);
    }

    if (size)
    {
        block1 = _mm512_loadu_si512(--sblock);
        _mm512_stream_si512(--dblock, block1);
    }
}

void __mrfstr_avx512_bfill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *rblock = (__m512i*)res;
    __m512i block = _mm512_set1_epi8(chr);

    while (size--)
        _mm512_store_si512(rblock++, block);
}

void __mrfstr_avx512_fill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *rblock = (__m512i*)res;
    __m512i block = _mm512_set1_epi8(chr);

    while (size--)
        _mm512_stream_si512(rblock++, block);
}

#ifdef __AVX512BW__
void __mrfstr_avx512_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
{
    __m512i *lblock = (__m512i*)left;
    __m512i *rblock = (__m512i*)right;

#ifdef __AVX512VBMI__
    const __m512i revidx = _mm512_set_epi64(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x1011121314151617, 0x18191a1b1c1d1e1f,
        0x2021222324252627, 0x28292a2b2c2d2e2f,
        0x3031323334353637, 0x38393a3b3c3d3e3f);
#else
    const __m512i revidx1 = _mm512_set_epi64(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f);

    const __m512i revidx2 = _mm512_set_epi64(1, 0, 3, 2, 5, 4, 7, 6);
#endif

    __m512i block1, block2;
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
    restrict mrfstr_ptr_t left, restrict mrfstr_ptr_ct right,
    mrfstr_size_t size)
{
    __m512i *lblock = (__m512i*)left;
    __m512i *rblock = (__m512i*)right;

#ifdef __AVX512VBMI__
    const __m512i revidx = _mm512_set_epi64(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x1011121314151617, 0x18191a1b1c1d1e1f,
        0x2021222324252627, 0x28292a2b2c2d2e2f,
        0x3031323334353637, 0x38393a3b3c3d3e3f);
#else
    const __m512i revidx1 = _mm512_set_epi64(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f);

    const __m512i revidx2 = _mm512_set_epi64(1, 0, 3, 2, 5, 4, 7, 6);
#endif

    __m512i block;
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
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    __m512i *sblock = (__m512i*)str;
    __m512i oblock = _mm512_set1_epi8(old);
    __m512i nblock = _mm512_set1_epi8(new);

    __m512i block;
    mrfstr_longlong_t mask;
    while (size--)
    {
        block = _mm512_load_si512(sblock);

        mask = _mm512_cmpeq_epi8_mask(block, oblock);
        if (mask)
            _mm512_mask_storeu_epi8(sblock++, mask, nblock);
    }
}

void __mrfstr_avx512_replchr2(
    restrict mrfstr_ptr_t res, restrict mrfstr_ptr_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    __m512i *rblock = (__m512i*)res;
    __m512i *sblock = (__m512i*)str;
    __m512i oblock = _mm512_set1_epi8(old);
    __m512i nblock = _mm512_set1_epi8(new);

    __m512i block;
    mrfstr_longlong_t mask;
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
    __m512i *s1block = (__m512i*)str1;
    __m512i *s2block = (__m512i*)str2;

    __m512i block1, block2;
    while (size--)
    {
        block1 = _mm512_load_si512(s1block++);
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
    __m512i *s1block = (__m512i*)str1;
    __m512i *s2block = (__m512i*)str2;

    __m512i block1, block2;
    mrfstr_size_t nsize;
    while (size >= MRFSTR_AVX512_TEQUAL_LOAD)
    {
        if (!*res)
            return;

        nsize = size - MRFSTR_AVX512_TEQUAL_LOAD;
        for (; size != nsize; size--)
        {
            block1 = _mm512_load_si512(s1block++);
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
        block1 = _mm512_load_si512(s1block++);
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
    __m512i *sblock = (__m512i*)str;
    __m512i cblock = _mm512_set1_epi8(chr);

    __m512i block;
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
    __m512i *sblock = (__m512i*)str;
    __m512i cblock = _mm512_set1_epi8(chr);

    __m512i block;
    mrfstr_size_t nsize;
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
    __m512i *sblock = (__m512i*)str;
    __m512i cblock = _mm512_set1_epi8(chr);

    __m512i block;
    mrfstr_longlong_t mask;
    mrfstr_size_t i;
    for (i = 0; i != size; i++)
    {
        block = _mm512_loadu_si512(sblock++);

        mask = _mm512_cmpeq_epi8_mask(block, cblock);
        if (mask)
            return (i << 6) + __mrfstr_ctz64(mask);
    }

    return MRFSTR_INVIDX;
}

mrfstr_idx_t __mrfstr_avx512_tfindchr(
    volatile mrfstr_idx_t *res, mrfstr_idx_t start,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *sblock = (__m512i*)str;
    __m512i cblock = _mm512_set1_epi8(chr);

    __m512i block;
    mrfstr_longlong_t mask;
    mrfstr_size_t i = 0, ni, lsize = size - MRFSTR_AVX512_TFINDCHR_LOAD;
    while (i <= lsize)
    {
        if (*res < start)
            return MRFSTR_INVIDX;

        ni = i + MRFSTR_AVX512_TFINDCHR_LOAD;
        for (; i != ni; i++)
        {
            block = _mm512_load_si512(sblock++);

            mask = _mm512_cmpeq_epi8_mask(block, cblock);
            if (mask)
                return start + (i << 6) + __mrfstr_ctz64(mask);
        }
    }

    if (*res < start)
        return MRFSTR_INVIDX;

    for (; i != size; i++)
    {
        block = _mm512_load_si512(sblock++);

        mask = _mm512_cmpeq_epi8_mask(block, cblock);
        if (mask)
            return start + (i << 6) + __mrfstr_ctz64(mask);
    }

    return MRFSTR_INVIDX;
}

mrfstr_size_t __mrfstr_avx512_strlen(
    mrfstr_ptr_ct str)
{
    mrfstr_data_ct base = (mrfstr_data_ct)str;
    __m512i *sblock = (__m512i*)str;

    __m512i block, zero = _mm512_setzero_si512();
    mrfstr_longlong_t mask;
    for (;; sblock++)
    {
        block = _mm512_load_si512(sblock);

        mask = _mm512_cmpeq_epi8_mask(block, zero);
        if (mask)
            return (mrfstr_size_t)((mrfstr_data_ct)sblock - base) + __mrfstr_ctz64(mask);
    }
}

#endif
