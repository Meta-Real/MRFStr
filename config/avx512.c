/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <avx512.h>

#ifdef __AVX512F__

void mrfstr_avx512_copy_sub(mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
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

void mrfstr_avx512_fill_sub(mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m512i *rblock = (__m512i*)res;
    __m512i block = _mm512_set1_epi8(chr);

    for (; size; size--)
        _mm512_stream_si512(rblock++, block);
}

#endif
