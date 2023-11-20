/*
    MRStr Library version 1.0.0
    MetaReal String Library
*/

#ifndef __MRSTR_ALLOC__
#define __MRSTR_ALLOC__

#include <stdint.h>

typedef uint64_t mrstr_size_t;
typedef void *ptr_t;

ptr_t mrstr_alloc(mrstr_size_t size);
ptr_t mrstr_mm_alloc(mrstr_size_t size, mrstr_size_t off);

ptr_t mrstr_calloc(mrstr_size_t count, mrstr_size_t unit);

ptr_t mrstr_realloc(ptr_t block, mrstr_size_t size);
ptr_t mrstr_mm_realloc(ptr_t block, mrstr_size_t size, mrstr_size_t off);

void mrstr_free(ptr_t block);
void mrstr_mm_free(ptr_t block);

#endif /* __MRSTR_ALLOC__ */
