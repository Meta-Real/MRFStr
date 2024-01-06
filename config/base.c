/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <base.h>
#include <binary.h>

#define mrfstr_base_set1(block, chr)            \
    do                                          \
    {                                           \
        block = (mrfstr_short_t)chr << 8 | chr; \
        block |= block << 16;                   \
        block |= block << 32;                   \
    } while (0)

#define mrfstr_base_cmp(mask) \
    ((mask - 0x1010101010101010ULL) & ~mask & 0x8080808080808080ULL)

void __mrfstr_base_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    mrfstr_longlong_t *dblock = (mrfstr_longlong_t*)dst;
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)src;

    for (; size; size--)
        *dblock++ = *sblock++;
}

void __mrfstr_base_fill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *rblock = (mrfstr_longlong_t*)res;
    mrfstr_longlong_t cblock;
    mrfstr_base_set1(cblock, chr);

    for (; size; size--)
        *rblock++ = cblock;
}

void __mrfstr_base_replace_chr(
    mrfstr_ptr_t str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)str;
    mrfstr_longlong_t oblock;
    mrfstr_base_set1(oblock, old);

    mrfstr_longlong_t mask, block;
    for (; size; size--, sblock++)
    {
        block = *sblock;

        mask = oblock ^ block;
        mask = mrfstr_base_cmp(mask) >> 7;
        if (mask)
        {
            block &= ~(0xff * mask);
            block |= new * mask;
            *sblock = block;
        }
    }
}

void __mrfstr_base_replace_chr2(
    mrfstr_ptr_t res, mrfstr_ptr_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size)
{
    mrfstr_longlong_t *rblock = (mrfstr_longlong_t*)res;
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)str;
    mrfstr_longlong_t oblock;
    mrfstr_base_set1(oblock, old);

    mrfstr_longlong_t mask, block;
    for (; size; size--)
    {
        block = *sblock++;

        mask = oblock ^ block;
        mask = mrfstr_base_cmp(mask) >> 7;
        if (mask)
        {
            block &= ~(0xff * mask);
            block |= new * mask;
        }

        *rblock++ = block;
    }
}

mrfstr_bool_t __mrfstr_base_equal(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    mrfstr_longlong_t *s1block = (mrfstr_longlong_t*)str1;
    mrfstr_longlong_t *s2block = (mrfstr_longlong_t*)str2;

    for (; size; size--)
        if (*s1block++ != *s2block++)
            return MRFSTR_FALSE;
    return MRFSTR_TRUE;
}

void __mrfstr_base_tequal(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    mrfstr_longlong_t *s1block = (mrfstr_longlong_t*)str1;
    mrfstr_longlong_t *s2block = (mrfstr_longlong_t*)str2;

    mrfstr_size_t nsize;
    while (size >= MRFSTR_BASE_TEQUAL_LOAD)
    {
        if (!*res)
            return;

        nsize = size - MRFSTR_BASE_TEQUAL_LOAD;
        for (; size != nsize; size--)
            if (*s1block++ != *s2block++)
            {
                *res = MRFSTR_FALSE;
                return;
            }
    }

    if (!*res)
        return;

    for (; size; size--)
        if (*s1block++ != *s2block++)
        {
            *res = MRFSTR_FALSE;
            return;
        }
}

mrfstr_bool_t __mrfstr_base_contain_chr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)str;
    mrfstr_longlong_t cblock;
    mrfstr_base_set1(cblock, chr);

    mrfstr_longlong_t mask;
    for (; size; size--)
    {
        mask = cblock ^ *sblock++;
        if (mrfstr_base_cmp(mask))
            return MRFSTR_TRUE;
    }

    return MRFSTR_FALSE;
}

void __mrfstr_base_tcontain_chr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)str;
    mrfstr_longlong_t cblock;
    mrfstr_base_set1(cblock, chr);

    mrfstr_size_t nsize;
    mrfstr_longlong_t mask;
    while (size >= MRFSTR_BASE_TCONTAIN_CHR_LOAD)
    {
        if (*res)
            return;

        nsize = size - MRFSTR_BASE_TCONTAIN_CHR_LOAD;
        for (; size != nsize; size--)
        {
            mask = cblock ^ *sblock++;
            if (mrfstr_base_cmp(mask))
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
        mask = cblock ^ *sblock++;
        if (mrfstr_base_cmp(mask))
        {
            *res = MRFSTR_TRUE;
            return;
        }
    }
}

mrfstr_idx_t __mrfstr_base_find_chr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)str;
    mrfstr_longlong_t cblock;
    mrfstr_base_set1(cblock, chr);

    mrfstr_longlong_t mask;
    mrfstr_size_t i;
    for (i = 0; i != size; i++)
    {
        mask = cblock ^ *sblock++;
        mask = mrfstr_base_cmp(mask) >> 7;
        if (mask)
            return i * 8 + __mrfstr_ctz64(mask) / 8;
    }

    return MRFSTR_FALSE;
}

mrfstr_idx_t __mrfstr_base_tfind_chr(
    volatile mrfstr_idx_t *res, mrfstr_idx_t start,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)str;
    mrfstr_longlong_t cblock;
    mrfstr_base_set1(cblock, chr);

    mrfstr_longlong_t mask;
    mrfstr_size_t i = 0, ni, lsize = size - MRFSTR_BASE_TFIND_CHR_LOAD;
    while (i <= lsize)
    {
        if (*res < start)
            return MRFSTR_INVIDX;

        ni = i + MRFSTR_BASE_TFIND_CHR_LOAD;
        for (; i != ni; i++)
        {
            mask = cblock ^ *sblock++;
            mask = mrfstr_base_cmp(mask) >> 7;
            if (mask)
                return i * 8 + __mrfstr_ctz64(mask) / 8;
        }
    }

    if (*res < start)
        return MRFSTR_INVIDX;

    for (; i != size; i++)
    {
        mask = cblock ^ *sblock++;
        mask = mrfstr_base_cmp(mask) >> 7;
        if (mask)
            return i * 8 + __mrfstr_ctz64(mask) / 8;
    }

    return MRFSTR_INVIDX;
}
