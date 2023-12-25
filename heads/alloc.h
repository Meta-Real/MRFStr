/*
    MRStr Library version 1.0.0
    MetaReal String Library
*/

#ifndef __MRSTR_ALLOC__
#define __MRSTR_ALLOC__

#include <stdint.h>

typedef uint64_t mrstr_size_t;
typedef void *mrstr_ptr_t;

inline mrstr_ptr_t mrstr_alloc(mrstr_size_t size)
{
    return malloc(size);
}

inline mrstr_ptr_t mrstr_calloc(mrstr_size_t count, mrstr_size_t unit)
{
    return calloc(count, unit);
}

inline mrstr_ptr_t mrstr_realloc(mrstr_ptr_t block, mrstr_size_t size)
{
    return realloc(block, size);
}

inline void mrstr_free(mrstr_ptr_t block)
{
    free(block);
}

#endif /* __MRSTR_ALLOC__ */
