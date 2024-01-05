/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <avx.h>

#ifdef __AVX__

void __mrfstr_avx_copy(mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
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

void __mrfstr_avx_fill(mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *rblock = (__m256i*)res;
    __m256i block = _mm256_set1_epi8(chr);

    for (; size; size--)
        _mm256_stream_si256(rblock++, block);

#ifdef __SSE__
    _mm_sfence();
#endif
}

#ifdef __AVX2__

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

#if defined(__AVX512F__) && defined(__AVX512VL__)
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

#if defined(__AVX512F__) && defined(__AVX512VL__)
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

#if defined(__AVX512F__) && defined(__AVX512VL__)
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

#endif

#endif
