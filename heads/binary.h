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

#ifndef __MRFSTR_BINARY__
#define __MRFSTR_BINARY__

#include <mrfstr.h>

mrfstr_byte_t __mrfstr_ctz64(
    mrfstr_longlong_t bits);
mrfstr_byte_t __mrfstr_ctz32(
    mrfstr_long_t bits);
mrfstr_byte_t __mrfstr_ctz16(
    mrfstr_short_t bits);

#endif
