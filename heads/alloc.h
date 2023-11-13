/*
    MRStr Library version 1.0.0
    MetaReal String Library
*/

#ifndef __MRSTR_ALLOC__
#define __MRSTR_ALLOC__

#include <stdint.h>

void *__mrstr_alloc(uint64_t size);
void *__mrstr_alloc_una(uint64_t size);

void *__mrstr_realloc(void *block, uint64_t size);
void *__mrstr_realloc_una(void *block, uint64_t size);

void __mrstr_free(void *block);
void __mrstr_free_una(void *block);

#endif /* __MRSTR_ALLOC__ */
