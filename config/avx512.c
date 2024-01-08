/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <avx512.h>
#include <binary.h>

#ifdef __AVX512F__

#ifdef __AVX512VBMI__
const __m512i mrfstr_avx512_revidx =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
};
#elif defined(__AVX512BW__)
const __m512i mrfstr_avx512_revidx1 =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

const __m512i mrfstr_avx512_revidx2 =
{
    0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 5,
    0, 0, 0, 0, 0, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 0, 7,
    0, 0, 0, 0, 0, 0, 0, 6
};
#endif

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

#ifdef __AVX512BW__
void __mrfstr_avx512_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
{
    __m512i *lblock = (__m512i*)left;
    __m512i *rblock = (__m512i*)right;

    __m512i block1, block2;
    for (; size; size--)
    {
        block1 = _mm512_load_si512(lblock);
        block2 = _mm512_loadu_si512(--rblock);

#ifdef __AVX512VBMI__
        block1 = _mm512_permutexvar_epi8(mrfstr_avx512_revidx, block1);
        block2 = _mm512_permutexvar_epi8(mrfstr_avx512_revidx, block2);
#else
        block1 = _mm512_shuffle_epi8(block1, mrfstr_avx512_revidx1);
        block1 = _mm512_permutexvar_epi64(mrfstr_avx512_revidx2, block1);

        block2 = _mm512_shuffle_epi8(block2, mrfstr_avx512_revidx1);
        block2 = _mm512_permutexvar_epi64(mrfstr_avx512_revidx2, block2);
#endif

        _mm512_store_si512(lblock++, block2);
        _mm512_storeu_si512(rblock, block1);
    }
}

void __mrfstr_avx512_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size)
{
    __m512i *lblock = (__m512i*)left;
    __m512i *rblock = (__m512i*)right;

    __m512i block;
    for (; size; size--)
    {
        block = _mm512_loadu_si512(--rblock);

#ifdef __AVX512VBMI__
        block = _mm512_permutexvar_epi8(mrfstr_avx512_revidx, block);
#else
        block = _mm512_shuffle_epi8(block, mrfstr_avx512_revidx1);
        block = _mm512_permutexvar_epi64(mrfstr_avx512_revidx2, block);
#endif

        _mm512_store_si512(lblock++, block);
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
    for (; size; size--)
    {
        block = _mm512_load_si512(sblock);

        mask = _mm512_cmpeq_epi8_mask(block, oblock);
        if (mask)
            _mm512_mask_storeu_epi8(sblock++, mask, nblock);
    }
}

void __mrfstr_avx512_replchr2(
    mrfstr_ptr_t res, mrfstr_ptr_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    __m512i *rblock = (__m512i*)res;
    __m512i *sblock = (__m512i*)str;
    __m512i oblock = _mm512_set1_epi8(old);
    __m512i nblock = _mm512_set1_epi8(new);

    __m512i block;
    mrfstr_longlong_t mask;
    for (; size; size--)
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
    for (; size; size--)
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

    for (; size; size--)
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
    for (; size; size--)
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

    for (; size; size--)
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
