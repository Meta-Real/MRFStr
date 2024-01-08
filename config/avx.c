/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <avx.h>
#include <binary.h>

#ifdef __AVX__

void __mrfstr_avx_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    __m256i *dblock = (__m256i*)dst;
    __m256i *sblock = (__m256i*)src;

    __m256i block1, block2, block3, block4;
    for (; size >= 4; size -= 4)
    {
        block1 = _mm256_loadu_si256(sblock++);
        block2 = _mm256_loadu_si256(sblock++);
        block3 = _mm256_loadu_si256(sblock++);
        block4 = _mm256_loadu_si256(sblock++);
        _mm256_stream_si256(dblock++, block1);
        _mm256_stream_si256(dblock++, block2);
        _mm256_stream_si256(dblock++, block3);
        _mm256_stream_si256(dblock++, block4);
    }

    while (size--)
    {
        block1 = _mm256_loadu_si256(sblock++);
        _mm256_store_si256(dblock++, block1);
    }
}

void __mrfstr_avx_fill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *rblock = (__m256i*)res;
    __m256i block = _mm256_set1_epi8(chr);

    for (; size; size--)
        _mm256_stream_si256(rblock++, block);
}

#ifdef __AVX2__
void __mrfstr_avx_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
{
    __m256i *lblock = (__m256i*)left;
    __m256i *rblock = (__m256i*)right;

#ifdef __AVX512VBMI__
    const __m256i revidx = _mm256_set_epi64x(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x1011121314151617, 0x18191a1b1c1d1e1f);
#else
    const __m256i revidx = _mm256_set_epi64x(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f);
#endif

    __m256i block1, block2;
    for (; size; size--)
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
    __m256i *lblock = (__m256i*)left;
    __m256i *rblock = (__m256i*)right;

#ifdef __AVX512VBMI__
    const __m256i revidx = _mm256_set_epi64x(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x1011121314151617, 0x18191a1b1c1d1e1f);
#else
    const __m256i revidx = _mm256_set_epi64x(
        0x0001020304050607, 0x08090a0b0c0d0e0f,
        0x0001020304050607, 0x08090a0b0c0d0e0f);
#endif

    __m256i block;
    for (; size; size--)
    {
        block = _mm256_loadu_si256(--rblock);

#ifdef __AVX512VBMI__
        block = _mm256_permutexvar_epi8(revidx, block);
#else
        block = _mm256_shuffle_epi8(block, revidx);
        block = _mm256_permute2x128_si256(block, block, 1);
#endif

        _mm256_store_si256(lblock++, block);
    }
}

void __mrfstr_avx_replchr(
    mrfstr_ptr_t str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    __m256i *sblock = (__m256i*)str;
    __m256i oblock = _mm256_set1_epi8(old);
    __m256i nblock = _mm256_set1_epi8(new);

    __m256i block;
#ifdef __AVX512VL__
    mrfstr_long_t mask;
#else
    __m256i mask;
#endif
    for (; size; size--, sblock++)
    {
        block = _mm256_load_si256(sblock);

#ifdef __AVX512VL__
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
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    __m256i *rblock = (__m256i*)res;
    __m256i *sblock = (__m256i*)str;
    __m256i oblock = _mm256_set1_epi8(old);
    __m256i nblock = _mm256_set1_epi8(new);

    __m256i block;
#ifdef __AVX512VL__
    mrfstr_long_t mask;
#else
    __m256i mask;
#endif
    for (; size; size--)
    {
        block = _mm256_loadu_si256(sblock++);

#ifdef __AVX512VL__
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
    __m256i *s1block = (__m256i*)str1;
    __m256i *s2block = (__m256i*)str2;

    __m256i block1, block2;
    for (; size; size--)
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
    __m256i *s1block = (__m256i*)str1;
    __m256i *s2block = (__m256i*)str2;

    __m256i block1, block2;
    mrfstr_size_t nsize;
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

    for (; size; size--)
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
    __m256i *sblock = (__m256i*)str;
    __m256i cblock = _mm256_set1_epi8(chr);

    __m256i block;
    for (; size; size--)
    {
        block = _mm256_loadu_si256(sblock++);

#ifdef __AVX512VL__
        if (_mm256_cmpeq_epi8_mask(block, cblock))
#else
        if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(block, cblock)))
#endif
            return MRFSTR_TRUE;
    }

    return MRFSTR_FALSE;
}

void __mrfstr_avx_tcontchr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *sblock = (__m256i*)str;
    __m256i cblock = _mm256_set1_epi8(chr);

    __m256i block;
    mrfstr_size_t nsize;
    while (size >= MRFSTR_AVX_TCONTCHR_LOAD)
    {
        if (*res)
            return;

        nsize = size - MRFSTR_AVX_TCONTCHR_LOAD;
        for (; size != nsize; size--)
        {
            block = _mm256_loadu_si256(sblock++);

#ifdef __AVX512VL__
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

    for (; size; size--)
    {
        block = _mm256_loadu_si256(sblock++);

#ifdef __AVX512VL__
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
    __m256i *sblock = (__m256i*)str;
    __m256i cblock = _mm256_set1_epi8(chr);

    __m256i block;
    mrfstr_long_t mask;
    mrfstr_size_t i;
    for (i = 0; i != size; i++)
    {
        block = _mm256_loadu_si256(sblock++);

#ifdef __AVX512VL__
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
    volatile mrfstr_idx_t *res, mrfstr_idx_t start,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *sblock = (__m256i*)str;
    __m256i cblock = _mm256_set1_epi8(chr);

    __m256i block;
    mrfstr_long_t mask;
    mrfstr_size_t i = 0, ni, lsize = size - MRFSTR_AVX_TFINDCHR_LOAD;
    while (i <= lsize)
    {
        if (*res < start)
            return MRFSTR_INVIDX;

        ni = i + MRFSTR_AVX_TFINDCHR_LOAD;
        for (; i != ni; i++)
        {
            block = _mm256_loadu_si256(sblock++);

#ifdef __AVX512VL__
            mask = _mm256_cmpeq_epi8_mask(block, cblock);
#else
            mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, cblock));
#endif
            if (mask)
                return start + (i << 5) + __mrfstr_ctz32(mask);
        }
    }

    if (*res < start)
        return MRFSTR_INVIDX;

    for (; i != size; i++)
    {
        block = _mm256_loadu_si256(sblock++);

#ifdef __AVX512VL__
        mask = _mm256_cmpeq_epi8_mask(block, cblock);
#else
        mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, cblock));
#endif
        if (mask)
            return start + (i << 5) + __mrfstr_ctz32(mask);
    }

    return MRFSTR_INVIDX;
}

mrfstr_size_t __mrfstr_avx_strlen(
    mrfstr_ptr_ct str)
{
    mrfstr_data_ct base = (mrfstr_data_ct)str;
    __m256i *sblock = (__m256i*)str;

    __m256i block, zero = _mm256_setzero_si256();
    mrfstr_long_t mask;
    for (;; sblock++)
    {
        block = _mm256_load_si256(sblock);

#ifdef __AVX512VL__
        mask = _mm256_cmpeq_epi8_mask(block, zero);
#else
        mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(block, zero));
#endif
        if (mask)
            return (mrfstr_size_t)((mrfstr_data_ct)sblock - base) + __mrfstr_ctz32(mask);
    }
}
#endif

#endif
