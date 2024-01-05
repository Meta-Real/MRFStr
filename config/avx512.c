/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <avx512.h>

#ifdef __AVX512F__

void __mrfstr_avx512_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    __m512i *dblock = (__m512i*)dst;
    __m512i *sblock = (__m512i*)src;

    __m512i block1, block2;
    for (; size >= 2; size -= 2)
    {
        block1 = _mm512_loadu_si512(sblock++);
        block2 = _mm512_loadu_si512(sblock++);
        _mm512_stream_si512(dblock++, block1);
        _mm512_stream_si512(dblock++, block2);
    }

    if (size)
    {
        block1 = _mm512_loadu_si512(sblock);
        _mm512_store_si512(dblock, block1);
    }
}

void __mrfstr_avx512_fill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *rblock = (__m512i*)res;
    __m512i block = _mm512_set1_epi8(chr);

    for (; size; size--)
        _mm512_stream_si512(rblock++, block);
}

mrfstr_bool_t __mrfstr_avx512_equal(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    __m512i *s1block = (__m512i*)str1;
    __m512i *s2block = (__m512i*)str2;

    __m512i block1, block2;
    for (; size; size--)
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

    for (; size; size--)
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

mrfstr_bool_t __mrfstr_avx512_contain(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *sblock = (__m512i*)str;
    __m512i cblock = _mm512_set1_epi8(chr);

    __m512i block;
    for (; size; size--)
    {
        block = _mm512_loadu_si512(sblock++);
        if (_mm512_cmpeq_epi8_mask(block, cblock))
            return MRFSTR_TRUE;
    }

    return MRFSTR_FALSE;
}

void __mrfstr_avx512_tcontain(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *sblock = (__m512i*)str;
    __m512i cblock = _mm512_set1_epi8(chr);

    __m512i block;
    mrfstr_size_t nsize;
    while (size >= MRFSTR_AVX512_TCONTAIN_LOAD)
    {
        if (*res)
            return;

        nsize = size - MRFSTR_AVX512_TCONTAIN_LOAD;
        for (; size != nsize; size--)
        {
            block = _mm512_loadu_si512(sblock++);
            if (_mm512_cmpeq_epi8_mask(block, cblock))
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
        block = _mm512_loadu_si512(sblock++);
        if (_mm512_cmpeq_epi8_mask(block, cblock))
        {
            *res = MRFSTR_TRUE;
            return;
        }
    }
}

#endif
