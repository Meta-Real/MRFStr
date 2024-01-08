/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#ifndef __MR_BINARY__
#define __MR_BINARY__

#include <mrfstr.h>

mrfstr_byte_t __mrfstr_ctz64(
    mrfstr_longlong_t bits);
mrfstr_byte_t __mrfstr_ctz32(
    mrfstr_long_t bits);
mrfstr_byte_t __mrfstr_ctz16(
    mrfstr_short_t bits);

#endif
