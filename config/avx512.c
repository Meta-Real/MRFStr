/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <avx512.h>

#ifdef __AVX512F__

mrfstr_ptr_t mrfstr_avx512_memcpy_sub(mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    mrfstr_avx512_t *dblock = (mrfstr_avx512_t*)dst;
    mrfstr_avx512_t *sblock = (mrfstr_avx512_t*)src;

    mrfstr_avx512_t block1, block2;
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

    return NULL;
}

#endif
