/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <binary.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <intrin.h>

mrfstr_byte_t __mrfstr_ctz64(
    mrfstr_longlong_t bits)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(bits);
#elif defined(_MSC_VER)
    mrfstr_byte_t count;
    BitScanForward64((DWORD*)&count, bits);
    return count;
#else
    mrfstr_byte_t count = 0;

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
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctz(bits);
#elif defined(_MSC_VER)
    mrfstr_byte_t count;
    BitScanForward((DWORD*)&count, bits);
    return count;
#else
    mrfstr_byte_t count = 0;

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
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctz(bits);
#elif defined(_MSC_VER)
    mrfstr_byte_t count;
    BitScanForward((DWORD*)&count, bits);
    return count;
#else
    mrfstr_byte_t count = 0;

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
