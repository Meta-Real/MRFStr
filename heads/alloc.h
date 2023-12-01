/*
    MRStr Library version 1.0.0
    MetaReal String Library
*/

#ifndef __MRSTR_ALLOC__
#define __MRSTR_ALLOC__

typedef unsigned long long mrstr_size_t;
typedef void *ptr_t;

ptr_t mrstr_alloc(mrstr_size_t size);
ptr_t mrstr_calloc(mrstr_size_t count, mrstr_size_t unit);
ptr_t mrstr_realloc(ptr_t block, mrstr_size_t size);
void mrstr_free(ptr_t block);

ptr_t mrstr_aligned_alloc(mrstr_size_t size, mrstr_size_t align);
ptr_t mrstr_aligned_realloc(ptr_t block, mrstr_size_t size, mrstr_size_t align);
void mrstr_aligned_free(ptr_t block);

#endif /* __MRSTR_ALLOC__ */
