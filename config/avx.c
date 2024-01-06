/*
    MRFStr Library version 0.1.0
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
void __mrfstr_avx_replace_chr(
    mrfstr_ptr_t str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    __m256i *sblock = (__m256i*)str;
    __m256i oblock = _mm256_set1_epi8(old);
    __m256i nblock = _mm256_set1_epi8(new);

    __m256i block, mask;
    for (; size; size--)
    {
        block = _mm256_load_si256(sblock);

        mask = _mm256_cmpeq_epi8(block, oblock);
        block = _mm256_blendv_epi8(block, nblock, mask);
        _mm256_store_si256(sblock++, block);
    }
}

void __mrfstr_avx_replace_chr2(
    mrfstr_ptr_t res, mrfstr_ptr_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    __m256i *rblock = (__m256i*)res;
    __m256i *sblock = (__m256i*)str;
    __m256i oblock = _mm256_set1_epi8(old);
    __m256i nblock = _mm256_set1_epi8(new);

    __m256i block, mask;
    for (; size; size--)
    {
        block = _mm256_loadu_si256(sblock++);

        mask = _mm256_cmpeq_epi8(block, oblock);
        block = _mm256_blendv_epi8(block, nblock, mask);
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

mrfstr_bool_t __mrfstr_avx_contain_chr(
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
        if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(block1, block2)))
#endif
            return MRFSTR_TRUE;
    }

    return MRFSTR_FALSE;
}

void __mrfstr_avx_tcontain_chr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *sblock = (__m256i*)str;
    __m256i cblock = _mm256_set1_epi8(chr);

    __m256i block;
    mrfstr_size_t nsize;
    while (size >= MRFSTR_AVX_TCONTAIN_CHR_LOAD)
    {
        if (*res)
            return;

        nsize = size - MRFSTR_AVX_TCONTAIN_CHR_LOAD;
        for (; size != nsize; size--)
        {
            block = _mm256_loadu_si256(sblock++);

#ifdef __AVX512VL__
            if (_mm256_cmpeq_epi8_mask(block, cblock))
#else
            if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(block1, block2)))
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
        if (_mm256_movemask_epi8(_mm256_cmpeq_epi64(block1, block2)))
#endif
        {
            *res = MRFSTR_TRUE;
            return;
        }
    }
}

mrfstr_idx_t __mrfstr_avx_find_chr(
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

mrfstr_idx_t __mrfstr_avx_tfind_chr(
    volatile mrfstr_idx_t *res, mrfstr_idx_t start,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *sblock = (__m256i*)str;
    __m256i cblock = _mm256_set1_epi8(chr);

    __m256i block;
    mrfstr_long_t mask;
    mrfstr_size_t i = 0, ni, lsize = size - MRFSTR_AVX_TFIND_CHR_LOAD;
    while (i <= lsize)
    {
        if (*res < start)
            return MRFSTR_INVIDX;

        ni = i + MRFSTR_AVX_TFIND_CHR_LOAD;
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
