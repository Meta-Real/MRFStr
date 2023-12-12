/*
    MRStr Library version 1.0.0
    MetaReal String Library
*/

#include <alloc.h>
#include <stdlib.h>

ptr_t mrstr_alloc(mrstr_size_t size)
{
    return malloc(size);
}

ptr_t mrstr_calloc(mrstr_size_t count, mrstr_size_t unit)
{
    return calloc(count, unit);
}

ptr_t mrstr_realloc(ptr_t block, mrstr_size_t size)
{
    return realloc(block, size);
}

void mrstr_free(ptr_t block)
{
    free(block);
}

ptr_t mrstr_aligned_alloc(mrstr_size_t size, mrstr_size_t align)
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    return _aligned_malloc(size, align);
#else
    return NULL;
#endif
}

ptr_t mrstr_aligned_realloc(ptr_t block, mrstr_size_t size, mrstr_size_t align)
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    return _aligned_realloc(block, size, align);
#else
    return NULL;
#endif
}

void mrstr_aligned_free(ptr_t block)
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    _aligned_free(block);
#endif
}
