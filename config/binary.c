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

#include <binary.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <intrin.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
#define MRFSTR_GCC_CLANG
#endif

mrfstr_byte_t __mrfstr_ctz64(
    mrfstr_longlong_t bits)
{
#ifdef MRFSTR_GCC_CLANG
    return __builtin_ctzll(bits);
#elif defined(_MSC_VER)
    mrfstr_byte_t count;

    BitScanForward64((DWORD*)&count, bits);
    return count;
#else
    mrfstr_byte_t count;

    count = 0;
    if (!(bits & 0xffffffff))
    {
        count += 32;
        bits >>= 32;
    }
    if (!(bits & 0xffff))
    {
        count += 16;
        bits >>= 16;
    }
    if (!(bits & 0xff))
    {
        count += 8;
        bits >>= 8;
    }
    if (!(bits & 15))
    {
        count += 4;
        bits >>= 4;
    }
    if (!(bits & 3))
    {
        count += 2;
        bits >>= 2;
    }
    if (!(bits & 1))
        count++;

    return count;
#endif
}

mrfstr_byte_t __mrfstr_ctz32(
    mrfstr_long_t bits)
{
#ifdef MRFSTR_GCC_CLANG
    return __builtin_ctz(bits);
#elif defined(_MSC_VER)
    mrfstr_byte_t count;
    BitScanForward((DWORD*)&count, bits);
    return count;
#else
    mrfstr_byte_t count;

    count = 0;
    if (!(bits & 0xffff))
    {
        count += 16;
        bits >>= 16;
    }
    if (!(bits & 0xff))
    {
        count += 8;
        bits >>= 8;
    }
    if (!(bits & 15))
    {
        count += 4;
        bits >>= 4;
    }
    if (!(bits & 3))
    {
        count += 2;
        bits >>= 2;
    }
    if (!(bits & 1))
        count++;

    return count;
#endif
}

mrfstr_byte_t __mrfstr_ctz16(
    mrfstr_short_t bits)
{
#ifdef MRFSTR_GCC_CLANG
    return __builtin_ctz(bits);
#elif defined(_MSC_VER)
    mrfstr_byte_t count;
    BitScanForward((DWORD*)&count, bits);
    return count;
#else
    mrfstr_byte_t count;

    count = 0;
    if (!(bits & 0xff))
    {
        count += 8;
        bits >>= 8;
    }
    if (!(bits & 15))
    {
        count += 4;
        bits >>= 4;
    }
    if (!(bits & 3))
    {
        count += 2;
        bits >>= 2;
    }
    if (!(bits & 1))
        count++;

    return count;
#endif
}

mrfstr_byte_t __mrfstr_popcnt64(
    mrfstr_longlong_t bits)
{
#ifdef MRFSTR_GCC_CLANG
    return __builtin_popcountll(bits);
#elif defined(_MSC_VER)
    return (mrfstr_byte_t)PopulationCount64(bits);
#else
    mrfstr_byte_t count;

    count = 0;
    while (bits)
    {
        if (bits & 1)
            count++;
        bits >>= 1;
    }

    return count;
#endif
}

mrfstr_byte_t __mrfstr_popcnt32(
    mrfstr_long_t bits)
{
#ifdef MRFSTR_GCC_CLANG
    return __builtin_popcount(bits);
#elif defined(_MSC_VER)
    return (mrfstr_byte_t)PopulationCount64(bits);
#else
    mrfstr_byte_t count;

    count = 0;
    while (bits)
    {
        if (bits & 1)
            count++;
        bits >>= 1;
    }

    return count;
#endif
}
