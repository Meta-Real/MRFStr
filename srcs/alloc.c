/*
    MRStr Library version 1.0.0
    MetaReal String Library
*/

#include <alloc.h>
#include <stdlib.h>
#include <stdio.h>

void *__mrstr_alloc(uint64_t size)
{
    void *block = _aligned_malloc(size, 64);
    if (!block)
    {
        fprintf(stderr,
            "MRStr Library, __mrstr_alloc function: Could not allocate %llu bytes from memory",
            size);
        abort();
    }

    return block;
}

void *__mrstr_alloc_una(uint64_t size)
{
    void *block = malloc(size);
    if (!block)
    {
        fprintf(stderr,
            "MRStr Library, __mrstr_alloc_una function: Could not allocate %llu bytes from memory",
            size);
        abort();
    }

    return block;
}

void *__mrstr_realloc(void *block, uint64_t size)
{
    block = _aligned_realloc(block, size, 64);
    if (!block)
    {
        fprintf(stderr,
            "MRStr Library, __mrstr_realloc function: Could not allocate %llu bytes from memory",
            size);
        abort();
    }

    return block;
}

void *__mrstr_realloc_una(void *block, uint64_t size)
{
    block = realloc(block, size);
    if (!block)
    {
        fprintf(stderr,
            "MRStr Library, __mrstr_realloc_una function: Could not allocate %llu bytes from memory",
            size);
        abort();
    }

    return block;
}

void __mrstr_free(void *block)
{
    _aligned_free(block);
}

void __mrstr_free_una(void *block)
{
    free(block);
}
