/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE (1024 * 1024 * 1024)

int main(void)
{
    mrfstr_t a = mrfstr_init();
    mrfstr_alloc(a, SIZE);

    mrfstr_t b = mrfstr_init();
    mrfstr_alloc(b, SIZE);
    MRFSTR_SIZE(b) = SIZE;

    mrfstr_set(a, b);

    if (!memcmp(MRFSTR_DATA(a), MRFSTR_DATA(b), SIZE))
    {
        mrfstr_free(a);
        mrfstr_free(b);
        return EXIT_SUCCESS;
    }

    mrfstr_free(a);
    mrfstr_free(b);
    return EXIT_FAILURE;
}
