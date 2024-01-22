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

#define mrfstr_move_rem \
    while (rem--)       \
        *dst++ = *src++

#define mrfstr_rmove_rem \
    while (rem--)        \
        *--dst = *--src

void __mrfstr_move(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        memmove(dst, src, size);
        return;
    }

    mrfstr_byte_t rem = (uintptr_t)dst % _mrfstr_config.nmem_size;
    if (rem)
    {
        rem = _mrfstr_config.nmem_size - rem;
        size -= rem;
        mrfstr_move_rem;
    }

    rem = size % _mrfstr_config.nmem_size;
    size -= rem;

    if (size < _mrfstr_config.nlimit)
        _mrfstr_config.bcopy_sub(dst, src, size / _mrfstr_config.nmem_size);
    else
        _mrfstr_config.ncopy_sub(dst, src, size / _mrfstr_config.nmem_size);
    dst += size;
    src += size;

    mrfstr_move_rem;
}

void __mrfstr_rmove(
    mrfstr_data_t dst, mrfstr_data_ct src, mrfstr_size_t size)
{
    if (size < MRFSTR_SLIMIT)
    {
        memmove(dst, src, size);
        return;
    }

    dst += size;
    src += size;

    mrfstr_byte_t rem = (uintptr_t)dst % _mrfstr_config.nmem_size;
    if (rem)
    {
        size -= rem;
        mrfstr_rmove_rem;
    }

    rem = size % _mrfstr_config.nmem_size;
    size -= rem;
    dst -= size;
    src -= size;

    if (size < _mrfstr_config.nlimit)
        _mrfstr_config.brcopy_sub(dst, src, size / _mrfstr_config.nmem_size);
    else
        _mrfstr_config.nrcopy_sub(dst, src, size / _mrfstr_config.nmem_size);

    mrfstr_rmove_rem;
}
