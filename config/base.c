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

#define mrfstr_base_cmp(mask) \
    ((mask - 0x1010101010101010ULL) & ~mask & 0x8080808080808080ULL)

void __mrfstr_base_copy(
    restrict mrfstr_ptr_t dst, restrict mrfstr_ptr_ct src, mrfstr_size_t size)
{
    mrfstr_longlong_t *dblock = (mrfstr_longlong_t*)dst;
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)src;

    while (size--)
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

void __mrfstr_base_rev(
    mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
{
    mrfstr_longlong_t *lblock = (mrfstr_longlong_t*)left;
    mrfstr_longlong_t *rblock = (mrfstr_longlong_t*)right;

    mrfstr_longlong_t block1, block2;
    for (; size; size--)
    {
        block1 = *lblock;
        block1 = (block1 & 0x00000000ffffffffULL) << 32 | (block1 & 0xffffffff00000000ULL) >> 32;
        block1 = (block1 & 0x0000ffff0000ffffULL) << 16 | (block1 & 0xffff0000ffff0000ULL) >> 16;
        block1 = (block1 & 0x00ff00ff00ff00ffULL) << 8 | (block1 & 0xff00ff00ff00ff00ULL) >> 8;

        block2 = *--rblock;
        block2 = (block2 & 0x00000000ffffffffULL) << 32 | (block2 & 0xffffffff00000000ULL) >> 32;
        block2 = (block2 & 0x0000ffff0000ffffULL) << 16 | (block2 & 0xffff0000ffff0000ULL) >> 16;
        block2 = (block2 & 0x00ff00ff00ff00ffULL) << 8 | (block2 & 0xff00ff00ff00ff00ULL) >> 8;

        *lblock++ = block2;
        *rblock = block1;
    }
}

void __mrfstr_base_rev2(
    mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size)
{
    mrfstr_longlong_t *lblock = (mrfstr_longlong_t*)left;
    mrfstr_longlong_t *rblock = (mrfstr_longlong_t*)right;

    mrfstr_longlong_t block;
    for (; size; size--)
    {
        block = *--rblock;
        block = (block & 0x00000000ffffffffULL) << 32 | (block & 0xffffffff00000000ULL) >> 32;
        block = (block & 0x0000ffff0000ffffULL) << 16 | (block & 0xffff0000ffff0000ULL) >> 16;
        block = (block & 0x00ff00ff00ff00ffULL) << 8 | (block & 0xff00ff00ff00ff00ULL) >> 8;

        *lblock++ = block;
    }
}

void __mrfstr_base_replchr(
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

void __mrfstr_base_replchr2(
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

mrfstr_bool_t __mrfstr_base_contchr(
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

void __mrfstr_base_tcontchr(
    volatile mrfstr_bool_t *res,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)str;
    mrfstr_longlong_t cblock;
    mrfstr_base_set1(cblock, chr);

    mrfstr_size_t nsize;
    mrfstr_longlong_t mask;
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

mrfstr_idx_t __mrfstr_base_findchr(
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
            return (i << 3) + (__mrfstr_ctz64(mask) >> 3);
    }

    return MRFSTR_FALSE;
}

mrfstr_idx_t __mrfstr_base_tfindchr(
    volatile mrfstr_idx_t *res, mrfstr_idx_t start,
    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
{
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)str;
    mrfstr_longlong_t cblock;
    mrfstr_base_set1(cblock, chr);

    mrfstr_longlong_t mask;
    mrfstr_size_t i = 0, ni, lsize = size - MRFSTR_BASE_TFINDCHR_LOAD;
    while (i <= lsize)
    {
        if (*res < start)
            return MRFSTR_INVIDX;

        ni = i + MRFSTR_BASE_TFINDCHR_LOAD;
        for (; i != ni; i++)
        {
            mask = cblock ^ *sblock++;
            mask = mrfstr_base_cmp(mask) >> 7;
            if (mask)
                return (i << 3) + (__mrfstr_ctz64(mask) >> 3);
        }
    }

    if (*res < start)
        return MRFSTR_INVIDX;

    for (; i != size; i++)
    {
        mask = cblock ^ *sblock++;
        mask = mrfstr_base_cmp(mask) >> 7;
        if (mask)
            return (i << 3) + (__mrfstr_ctz64(mask) >> 3);
    }

    return MRFSTR_INVIDX;
}

mrfstr_size_t __mrfstr_base_strlen(
    mrfstr_ptr_ct str)
{
    mrfstr_data_ct base = (mrfstr_data_ct)str;
    mrfstr_longlong_t *sblock = (mrfstr_longlong_t*)str;

    mrfstr_longlong_t mask;
    for (;; sblock++)
    {
        mask = mrfstr_base_cmp(*sblock) >> 7;
        if (mask)
            return (mrfstr_size_t)((mrfstr_data_ct)sblock - base) + (__mrfstr_ctz64(mask) >> 3);
    }
}
