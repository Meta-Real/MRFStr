/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <avx.h>

#ifdef __AVX__

mrfstr_ptr_t mrfstr_avx_memcpy_sub(mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    mrfstr_avx_t *dblock = (mrfstr_avx_t*)dst;
    mrfstr_avx_t *sblock = (mrfstr_avx_t*)src;

    mrfstr_avx_t block1, block2, block3, block4;
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

    return NULL;
}

#endif
