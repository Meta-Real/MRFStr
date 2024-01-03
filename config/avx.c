/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <avx.h>

#ifdef __AVX__

void mrfstr_avx_copy_sub(mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
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

void mrfstr_avx_fill_sub(mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    __m256i *rblock = (__m256i*)res;
    __m256i block = _mm256_set1_epi8(chr);

    for (; size; size--)
        _mm256_stream_si256(rblock++, block);
}

#endif
