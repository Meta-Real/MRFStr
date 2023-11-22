/*
    MRStr Library version 1.0.0
    MetaReal String Library
*/

#include <alloc.h>
#include <stdlib.h>
#include <stdio.h>

ptr_t mrstr_alloc(mrstr_size_t size)
{
    void *block = malloc(size);
    if (!block)
    {
        fprintf(stderr,
            "MRStr Library, mrstr_alloc function: Could not allocate %llu bytes from memory",
            size);
        abort();
    }

    return block;
}

ptr_t mrstr_calloc(mrstr_size_t count, mrstr_size_t unit)
{
    void *block = calloc(count, unit);
    if (!block)
    {
        fprintf(stderr,
            "MRStr Library, mrstr_calloc function: Could not allocate %llu bytes from memory",
            count * unit);
        abort();
    }

    return block;
}

ptr_t mrstr_realloc(ptr_t block, mrstr_size_t size)
{
    block = realloc(block, size);
    if (!block)
    {
        fprintf(stderr,
            "MRStr Library, mrstr_realloc function: Could not allocate %llu bytes from memory",
            size);
        abort();
    }

    return block;
}

void mrstr_free(ptr_t block)
{
    free(block);
}
