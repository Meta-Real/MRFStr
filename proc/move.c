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

#include <mrfstr-intern.h>
#include <string.h>

#define mrfstr_move_rem \
    while (rem--)       \
        *dst++ = *src++

#define mrfstr_rmove_rem \
    while (rem--)        \
        *--dst = *--src

void __mrfstr_move(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    mrfstr_byte_t rem;

    if (size < MRFSTR_SLIMIT)
    {
        memmove(dst, src, size);
        return;
    }

    rem = (uintptr_t)dst & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        rem = MRFSTR_ALIGN_SIZE - rem;
        size -= rem;
        mrfstr_move_rem;
    }

    rem = size & MRFSTR_ALIGN_MASK;
    size -= rem;
    _mrfstr_config.copy_func(dst += size, src += size,
        (mrfstr_size_t)-(mrfstr_ssize_t)size);

    mrfstr_move_rem;
}

void __mrfstr_rmove(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    mrfstr_byte_t rem;

    if (size < MRFSTR_SLIMIT)
    {
        memmove(dst, src, size);
        return;
    }

    dst += size;
    src += size;

    rem = (uintptr_t)dst & MRFSTR_ALIGN_MASK;
    if (rem)
    {
        size -= rem;
        mrfstr_rmove_rem;
    }

    rem = size & MRFSTR_ALIGN_MASK;
    size -= rem;
    _mrfstr_config.rcopy_func(dst -= size, src -= size, size);

    mrfstr_rmove_rem;
}
