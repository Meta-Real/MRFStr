/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <sse.h>

#ifdef __SSE2__

void mrfstr_sse_copy_sub(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    __m128i *dblock = (__m128i*)dst;
    __m128i *sblock = (__m128i*)src;

    __m128i
        block1, block2, block3, block4,
        block5, block6, block7, block8,
        blockA, blockB, blockC, blockD,
        blockE, blockF, blockG, blockH;
    for (; size >= 16; size -= 16)
    {
        block1 = _mm_loadu_si128(sblock++);
        block2 = _mm_loadu_si128(sblock++);
        block3 = _mm_loadu_si128(sblock++);
        block4 = _mm_loadu_si128(sblock++);
        block5 = _mm_loadu_si128(sblock++);
        block6 = _mm_loadu_si128(sblock++);
        block7 = _mm_loadu_si128(sblock++);
        block8 = _mm_loadu_si128(sblock++);
        blockA = _mm_loadu_si128(sblock++);
        blockB = _mm_loadu_si128(sblock++);
        blockC = _mm_loadu_si128(sblock++);
        blockD = _mm_loadu_si128(sblock++);
        blockE = _mm_loadu_si128(sblock++);
        blockF = _mm_loadu_si128(sblock++);
        blockG = _mm_loadu_si128(sblock++);
        blockH = _mm_loadu_si128(sblock++);
        _mm_stream_si128(dblock++, block1);
        _mm_stream_si128(dblock++, block2);
        _mm_stream_si128(dblock++, block3);
        _mm_stream_si128(dblock++, block4);
        _mm_stream_si128(dblock++, block5);
        _mm_stream_si128(dblock++, block6);
        _mm_stream_si128(dblock++, block7);
        _mm_stream_si128(dblock++, block8);
        _mm_stream_si128(dblock++, blockA);
        _mm_stream_si128(dblock++, blockB);
        _mm_stream_si128(dblock++, blockC);
        _mm_stream_si128(dblock++, blockD);
        _mm_stream_si128(dblock++, blockE);
        _mm_stream_si128(dblock++, blockF);
        _mm_stream_si128(dblock++, blockG);
        _mm_stream_si128(dblock++, blockH);
    }

    while (size--)
    {
        block1 = _mm_loadu_si128(sblock++);
        _mm_store_si128(dblock++, block1);
    }
}

void mrfstr_sse_fill_sub(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m128i *rblock = (__m128i*)res;
    __m128i block = _mm_set1_epi8(chr);

    for (; size; size--)
        _mm_stream_si128(rblock++, block);
}

mrfstr_bool_t mrfstr_sse_cmp_sub(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    __m128i *s1block = (__m128i*)str1;
    __m128i *s2block = (__m128i*)str2;

    __m128i block1, block2;
    for (; size; size--)
    {
        block1 = _mm_loadu_si128(s1block++);
        block2 = _mm_loadu_si128(s2block++);

#if defined(__AVX512F__) && defined(__AVX512VL__)
        if (_mm_cmpneq_epi64_mask(block1, block2))
#else
        if (~(mrfstr_short_t)_mm_movemask_epi8(_mm_cmpeq_epi32(block1, block2)))
#endif
            return MRFSTR_FALSE;
    }

    return MRFSTR_TRUE;
}

void mrfstr_sse_tcmp_sub(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    __m128i *s1block = (__m128i*)str1;
    __m128i *s2block = (__m128i*)str2;

    __m128i block1, block2;
    mrfstr_size_t nsize;
    while (size >= MRFSTR_SSE_TCMP_LOAD)
    {
        if (!*res)
            return;

        nsize = size - MRFSTR_SSE_TCMP_LOAD;
        for (; size != nsize; size--)
        {
            block1 = _mm_loadu_si128(s1block++);
            block2 = _mm_loadu_si128(s2block++);

#if defined(__AVX512F__) && defined(__AVX512VL__)
            if (_mm_cmpneq_epi64_mask(block1, block2))
#else
            if (~(mrfstr_short_t)_mm_movemask_epi8(_mm_cmpeq_epi32(block1, block2)))
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
        block1 = _mm_loadu_si128(s1block++);
        block2 = _mm_loadu_si128(s2block++);

#if defined(__AVX512F__) && defined(__AVX512VL__)
        if (_mm_cmpneq_epi64_mask(block1, block2))
#else
        if (~(mrfstr_short_t)_mm_movemask_epi8(_mm_cmpeq_epi32(block1, block2)))
#endif
        {
            *res = MRFSTR_FALSE;
            return;
        }
    }
}

#endif
