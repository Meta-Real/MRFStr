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

#ifndef __MRFSTR_CPUID__
#define __MRFSTR_CPUID__

#include <mrfstr.h>

enum __MRFSTR_CPUID_VENDOR_ENUM
{
    MRFSTR_CPUID_VENDOR_UNKNOWN,
    MRFSTR_CPUID_VENDOR_INTEL,
    MRFSTR_CPUID_VENDOR_AMD
};

mrfstr_ubyte_t mrfstr_cpuid_cputype(
    mrfstr_ubyte_t *family, mrfstr_ubyte_t *model);
mrfstr_ubyte_t mrfstr_cpuid_funccnt(
    mrfstr_ubyte_t *ext);

mrfstr_ubyte_t mrfstr_cpuid_simdset(void);

mrfstr_ubyte_t mrfstr_cpuid_proccnt(
    mrfstr_ubyte_t *logical);

mrfstr_size_t mrfstr_cpuid_cachesize(
    mrfstr_size_t *l2, mrfstr_size_t *l3);

#endif
