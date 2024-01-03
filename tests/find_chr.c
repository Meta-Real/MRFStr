/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#define TEST_LOW 0x400
#define TEST_MID 0x100000
#define TEST_HIGH 0x40000000

#define TEST_COUNT 100
#define GENERATE32_RAND (((mrfstr_long_t)rand() << 16) + rand())

int main(void)
{
    mrfstr_t str = mrfstr_init();
    if (!str)
    {
        fputs("\"find_chr\" error: Initializing \"str\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(str, TEST_HIGH))
    {
        mrfstr_free(str);

        fputs("\"find_chr\" error: Allocating \"str\" section\n", stderr);
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));
    mrfstr_byte_t i;
    mrfstr_idx_t idx;

    memset(MRFSTR_DATA(str), '0', TEST_LOW);
    MRFSTR_SIZE(str) = TEST_LOW;

    if (mrfstr_find_chr(str, '1') != MRFSTR_INVIDX)
    {
        mrfstr_free(str);

        fputs("\"find_chr\" error: TEST_LOW all zero section\n", stderr);
        return EXIT_FAILURE;
    }

    for (i = 0; i < TEST_COUNT;)
    {
        idx = rand() % TEST_LOW;
        if (MRFSTR_DATA(str)[idx] == '2')
            continue;

        MRFSTR_DATA(str)[idx] = '1';
        if (mrfstr_find_chr(str, '1') != idx)
        {
            mrfstr_free(str);

            fprintf(stderr, "\"find_chr\" error: TEST_LOW random one section\n"
                "\tFailed index: %" PRIu64 "\n", idx);
            return EXIT_FAILURE;
        }

        MRFSTR_DATA(str)[idx] = '2';
        i++;
    }

    memset(MRFSTR_DATA(str), '0', TEST_MID);
    MRFSTR_SIZE(str) = TEST_MID;

    if (mrfstr_find_chr(str, '1') != MRFSTR_INVIDX)
    {
        mrfstr_free(str);

        fputs("\"find_chr\" error: TEST_MID all zero section\n", stderr);
        return EXIT_FAILURE;
    }

    for (i = 0; i < TEST_COUNT;)
    {
        idx = GENERATE32_RAND % TEST_MID;
        if (MRFSTR_DATA(str)[idx] == '2')
            continue;

        MRFSTR_DATA(str)[idx] = '1';
        if (mrfstr_find_chr(str, '1') != idx)
        {
            mrfstr_free(str);

            fprintf(stderr, "\"find_chr\" error: TEST_MID random one section\n"
                "\tFailed index: %" PRIu64 "\n", idx);
            return EXIT_FAILURE;
        }

        MRFSTR_DATA(str)[idx] = '2';
        i++;
    }

    memset(MRFSTR_DATA(str), '0', TEST_HIGH);
    MRFSTR_SIZE(str) = TEST_HIGH;

    if (mrfstr_find_chr(str, '1') != MRFSTR_INVIDX)
    {
        mrfstr_free(str);

        fputs("\"find_chr\" error: TEST_HIGH all zero section\n", stderr);
        return EXIT_FAILURE;
    }

    for (i = 0; i < TEST_COUNT;)
    {
        idx = (((mrfstr_size_t)GENERATE32_RAND << 32) + GENERATE32_RAND) % TEST_HIGH;
        if (MRFSTR_DATA(str)[idx] == '2')
            continue;

        MRFSTR_DATA(str)[idx] = '1';
        if (mrfstr_find_chr(str, '1') != idx)
        {
            mrfstr_free(str);

            fprintf(stderr, "\"find_chr\" error: TEST_HIGH random one section\n"
                "\tFailed index: %" PRIu64 "\n", idx);
            return EXIT_FAILURE;
        }

        MRFSTR_DATA(str)[idx] = '2';
        i++;
    }

    mrfstr_free(str);
    return EXIT_SUCCESS;
}
