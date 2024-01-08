/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_LOW 0x200
#define TEST_MID 0x100000
#define TEST_HIGH 0x40000000

int main(void)
{
    mrfstr_config_tcount(5);

    mrfstr_t dst = mrfstr_init();
    if (!dst)
    {
        fputs("\"set\" error: Initializing \"dst\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(dst, TEST_HIGH))
    {
        mrfstr_free(dst);

        fputs("\"set\" error: Allocating \"dst\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_t src = mrfstr_init();
    if (!src)
    {
        mrfstr_free(dst);

        fputs("\"set\" error: Initializing \"src\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(src, TEST_HIGH))
    {
        mrfstr_free(dst);
        mrfstr_free(src);

        fputs("\"set\" error: Allocating \"src\" section\n", stderr);
        return EXIT_FAILURE;
    }

    MRFSTR_SIZE(src) = TEST_LOW;
    memset(MRFSTR_DATA(src), '0', TEST_LOW);
    mrfstr_set(dst, src);

    if (MRFSTR_SIZE(dst) != TEST_LOW || memcmp(MRFSTR_DATA(dst), MRFSTR_DATA(src), TEST_LOW))
    {
        mrfstr_free(dst);
        mrfstr_free(src);

        fputs("\"set\" error: TEST_LOW section\n", stderr);
        return EXIT_FAILURE;
    }

    MRFSTR_SIZE(src) = TEST_MID;
    memset(MRFSTR_DATA(src), '1', TEST_MID);
    mrfstr_set(dst, src);

    if (MRFSTR_SIZE(dst) != TEST_MID || memcmp(MRFSTR_DATA(dst), MRFSTR_DATA(src), TEST_MID))
    {
        mrfstr_free(dst);
        mrfstr_free(src);

        fputs("\"set\" error: TEST_MID section\n", stderr);
        return EXIT_FAILURE;
    }

    MRFSTR_SIZE(src) = TEST_HIGH;
    memset(MRFSTR_DATA(src), '2', TEST_HIGH);
    mrfstr_set(dst, src);

    if (MRFSTR_SIZE(dst) != TEST_HIGH || memcmp(MRFSTR_DATA(dst), MRFSTR_DATA(src), TEST_HIGH))
    {
        mrfstr_free(dst);
        mrfstr_free(src);

        fputs("\"set\" error: TEST_HIGH section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_free(dst);
    mrfstr_free(src);
    return EXIT_SUCCESS;
}
