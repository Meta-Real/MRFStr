/*
MIT License

Copyright (c) 2023 MetaReal

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
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

#define mrfstr_base_rev(block)                                                                 \
    do                                                                                         \
    {                                                                                          \
        block = (block & 0x00000000ffffffffULL) << 32 | (block & 0xffffffff00000000ULL) >> 32; \
        block = (block & 0x0000ffff0000ffffULL) << 16 | (block & 0xffff0000ffff0000ULL) >> 16; \
        block = (block & 0x00ff00ff00ff00ffULL) << 8  | (block & 0xff00ff00ff00ff00ULL) >> 8;  \
    } while (0)

#define mrfstr_base_cmp(mask) \
    ((mask - 0x0101010101010101ULL) & ~mask & 0x8080808080808080ULL)

#define mrfstr_base_cmp2(mask) \
    (~((((mask & 0x7f7f7f7f7f7f7f7f) + 0x7f7f7f7f7f7f7f7f) | mask) | 0x7f7f7f7f7f7f7f7f))

#define mrfstr_base_idx(mask) (mrfstr_base_cmp2(mask) >> 7)

void __mrfstr_base_copy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src,
    mrfstr_size_t size)
{
    mrfstr_longlong_t *dblock, *sblock;

    dblock = (mrfstr_longlong_t*)dst;
    sblock = (mrfstr_longlong_t*)src;
    while (size--)
        *dblock++ = *sblock++;
}

void __mrfstr_base_rcopy(
    mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
{
    mrfstr_longlong_t *dblock, *sblock;

    dblock = (mrfstr_longlong_t*)dst;
    sblock = (mrfstr_longlong_t*)src;
    while (size--)
        *--dblock = *--sblock;
}

void __mrfstr_base_fill(
    mrfstr_ptr_t res, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *rblock, cblock;

    rblock = (mrfstr_longlong_t*)res;
    mrfstr_base_set1(cblock, chr);
    while (size--)
        *rblock++ = cblock;
}

void __mrfstr_base_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
{
    mrfstr_longlong_t *lblock, *rblock, block1, block2;

    lblock = (mrfstr_longlong_t*)left;
    rblock = (mrfstr_longlong_t*)right;
    while (size--)
    {
        block1 = *lblock;
        mrfstr_base_rev(block1);

        block2 = *--rblock;
        mrfstr_base_rev(block2);

        *lblock++ = block2;
        *rblock = block1;
    }
}

void __mrfstr_base_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right,
    mrfstr_size_t size)
{
    mrfstr_longlong_t *lblock, *rblock, block;

    lblock = (mrfstr_longlong_t*)left;
    rblock = (mrfstr_longlong_t*)right;
    while (size--)
    {
        block = *--rblock;
        mrfstr_base_rev(block);

        *lblock++ = block;
    }
}

void __mrfstr_base_replchr(
    mrfstr_ptr_t str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock, oblock, mask, block;

    sblock = (mrfstr_longlong_t*)str;
    mrfstr_base_set1(oblock, ochr);
    for (; size; size--, sblock++)
    {
        block = *sblock;

        mask = oblock ^ block;
        mask = mrfstr_base_idx(mask);
        if (mask)
        {
            block &= ~(0xff * mask);
            block |= nchr * mask;
            *sblock = block;
        }
    }
}

void __mrfstr_base_replchr2(
    mrfstr_ptr_t res, mrfstr_ptr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
    mrfstr_size_t size)
{
    mrfstr_longlong_t *rblock, *sblock, oblock, mask, block;

    rblock = (mrfstr_longlong_t*)res;
    sblock = (mrfstr_longlong_t*)str;
    mrfstr_base_set1(oblock, ochr);
    while (size--)
    {
        block = *sblock++;

        mask = oblock ^ block;
        mask = mrfstr_base_idx(mask);
        if (mask)
        {
            block &= ~(0xff * mask);
            block |= nchr * mask;
        }

        *rblock++ = block;
    }
}

mrfstr_bool_t __mrfstr_base_equal(
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    mrfstr_longlong_t *s1block, *s2block;

    s1block = (mrfstr_longlong_t*)str1;
    s2block = (mrfstr_longlong_t*)str2;
    while (size--)
        if (*s1block++ != *s2block++)
            return MRFSTR_FALSE;

    return MRFSTR_TRUE;
}

void __mrfstr_base_tequal(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
{
    mrfstr_longlong_t *s1block, *s2block;
    mrfstr_size_t nsize;

    s1block = (mrfstr_longlong_t*)str1;
    s2block = (mrfstr_longlong_t*)str2;
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

    while (size--)
        if (*s1block++ != *s2block++)
        {
            *res = MRFSTR_FALSE;
            return;
        }
}

mrfstr_bool_t __mrfstr_base_contchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock, cblock, mask,
        block1, block2, block3, block4;

    sblock = (mrfstr_longlong_t*)str;
    mrfstr_base_set1(cblock, chr);
    for (; size >= 4; size -= 4)
    {
        block1 = *sblock++;
        block2 = *sblock++;
        block3 = *sblock++;
        block4 = *sblock++;

        mask = cblock ^ block1;
        if (mrfstr_base_cmp(mask))
            return MRFSTR_TRUE;

        mask = cblock ^ block2;
        if (mrfstr_base_cmp(mask))
            return MRFSTR_TRUE;

        mask = cblock ^ block3;
        if (mrfstr_base_cmp(mask))
            return MRFSTR_TRUE;

        mask = cblock ^ block4;
        if (mrfstr_base_cmp(mask))
            return MRFSTR_TRUE;
    }

    while (size--)
    {
        mask = cblock ^ *sblock++;
        if (mrfstr_base_cmp(mask))
            return MRFSTR_TRUE;
    }

    return MRFSTR_FALSE;
}

void __mrfstr_base_tcontchr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock, cblock, mask;
    mrfstr_size_t nsize;

    sblock = (mrfstr_longlong_t*)str;
    mrfstr_base_set1(cblock, chr);
    while (size >= MRFSTR_BASE_TCONTCHR_LOAD)
    {
        if (*res)
            return;

        nsize = size - MRFSTR_BASE_TCONTCHR_LOAD;
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

    while (size--)
    {
        mask = cblock ^ *sblock++;
        if (mrfstr_base_cmp(mask))
        {
            *res = MRFSTR_TRUE;
            return;
        }
    }
}

mrfstr_idx_t __mrfstr_base_findchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock, cblock, mask;
    mrfstr_size_t i;

    sblock = (mrfstr_longlong_t*)str;
    mrfstr_base_set1(cblock, chr);
    for (i = 0; i != size; i++)
    {
        mask = cblock ^ *sblock++;
        if (mrfstr_base_cmp(mask))
            return (i << 3) + (__mrfstr_ctz64(mrfstr_base_cmp2(mask)) >> 3);
    }

    return MRFSTR_INVIDX;
}

mrfstr_idx_t __mrfstr_base_tfindchr(
    volatile mrfstr_idx_t *res, mrfstr_size_t start,
    mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_short_t step)
{
    mrfstr_longlong_t cblock, mask;

    mrfstr_base_set1(cblock, chr);
    for (start <<= 3; start < *res; start += step)
    {
        mask = cblock ^ *(mrfstr_longlong_t*)(str + start);
        if (mrfstr_base_cmp(mask))
            return start + (__mrfstr_ctz64(mrfstr_base_cmp2(mask)) >> 3);
    }

    return MRFSTR_INVIDX;
}

mrfstr_size_t __mrfstr_base_countchr(
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock, cblock, mask;
    mrfstr_size_t count;

    sblock = (mrfstr_longlong_t*)str;
    mrfstr_base_set1(cblock, chr);
    count = 0;
    while (size--)
    {
        mask = cblock ^ *sblock++;
        mask = mrfstr_base_cmp2(mask);
        count += __mrfstr_popcnt64(mask);
    }

    return count;
}

mrfstr_size_t __mrfstr_base_strlen(
    mrfstr_data_ct str)
{
    mrfstr_data_ct base;
    mrfstr_longlong_t *sblock, mask;

    base = str;
    sblock = (mrfstr_longlong_t*)str;
    for (;; sblock++)
    {
        mask = mrfstr_base_cmp(*sblock);
        if (mask)
        {
            mask = mrfstr_base_cmp2(*sblock);
            return (mrfstr_size_t)(uintptr_t)((mrfstr_data_ct)sblock - base) +
                (__mrfstr_ctz64(mask) >> 3);
        }
    }
}
