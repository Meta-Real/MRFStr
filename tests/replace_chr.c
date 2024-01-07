/*
    MRFStr Library version 0.1.0
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

    mrfstr_t res1 = mrfstr_init();
    if (!res1)
    {
        fputs("\"replace\" error: Initializing \"res1\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(res1, TEST_HIGH))
    {
        mrfstr_free(res1);

        fputs("\"replace\" error: Allocating \"res1\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_t res2 = mrfstr_init();
    if (!res2)
    {
        mrfstr_free(res1);

        fputs("\"replace\" error: Initializing \"res2\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(res2, TEST_HIGH))
    {
        mrfstr_free(res1);
        mrfstr_free(res2);

        fputs("\"replace\" error: Allocating \"res2\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_data_t str = malloc(TEST_HIGH * sizeof(mrfstr_chr_t));
    if (!str)
    {
        mrfstr_free(res1);
        mrfstr_free(res2);

        fputs("\"replace\" error: Allocating \"str\" section\n", stderr);
        return EXIT_FAILURE;
    }

    memset(str, '1', TEST_HIGH);

    memset(MRFSTR_DATA(res1), '0', TEST_LOW);
    MRFSTR_SIZE(res1) = TEST_LOW;
    mrfstr_replace_chr(res1, res1, '0', '1');

    if (MRFSTR_SIZE(res1) != TEST_LOW || memcmp(MRFSTR_DATA(res1), str, TEST_LOW))
    {
        mrfstr_free(res1);
        mrfstr_free(res2);
        free(str);

        fputs("\"replace\" error: TEST_LOW on string section\n", stderr);
        return EXIT_FAILURE;
    }

    memset(MRFSTR_DATA(res1), '0', TEST_LOW);
    MRFSTR_SIZE(res1) = TEST_LOW;
    mrfstr_replace_chr(res2, res1, '0', '1');

    if (MRFSTR_SIZE(res2) != TEST_LOW || memcmp(MRFSTR_DATA(res2), str, TEST_LOW))
    {
        mrfstr_free(res1);
        mrfstr_free(res2);
        free(str);

        fputs("\"replace\" error: TEST_LOW two strings section\n", stderr);
        return EXIT_FAILURE;
    }

    memset(MRFSTR_DATA(res1), '0', TEST_MID);
    MRFSTR_SIZE(res1) = TEST_MID;
    mrfstr_replace_chr(res1, res1, '0', '1');

    if (MRFSTR_SIZE(res1) != TEST_MID || memcmp(MRFSTR_DATA(res1), str, TEST_MID))
    {
        mrfstr_free(res1);
        mrfstr_free(res2);
        free(str);

        fputs("\"replace\" error: TEST_MID one string section\n", stderr);
        return EXIT_FAILURE;
    }

    memset(MRFSTR_DATA(res1), '0', TEST_MID);
    MRFSTR_SIZE(res1) = TEST_MID;
    mrfstr_replace_chr(res2, res1, '0', '1');

    if (MRFSTR_SIZE(res2) != TEST_MID || memcmp(MRFSTR_DATA(res2), str, TEST_MID))
    {
        mrfstr_free(res1);
        mrfstr_free(res2);
        free(str);

        fputs("\"replace\" error: TEST_MID two strings section\n", stderr);
        return EXIT_FAILURE;
    }

    memset(MRFSTR_DATA(res1), '0', TEST_HIGH);
    MRFSTR_SIZE(res1) = TEST_HIGH;
    mrfstr_replace_chr(res1, res1, '0', '1');

    if (MRFSTR_SIZE(res1) != TEST_HIGH || memcmp(MRFSTR_DATA(res1), str, TEST_HIGH))
    {
        mrfstr_free(res1);
        mrfstr_free(res2);
        free(str);

        fputs("\"replace\" error: TEST_HIGH one string section\n", stderr);
        return EXIT_FAILURE;
    }

    memset(MRFSTR_DATA(res1), '0', TEST_HIGH);
    MRFSTR_SIZE(res1) = TEST_HIGH;
    mrfstr_replace_chr(res2, res1, '0', '1');

    if (MRFSTR_SIZE(res2) != TEST_HIGH || memcmp(MRFSTR_DATA(res2), str, TEST_HIGH))
    {
        mrfstr_free(res1);
        mrfstr_free(res2);
        free(str);

        fputs("\"replace\" error: TEST_HIGH two strings section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_free(res1);
    mrfstr_free(res2);
    free(str);
    return EXIT_SUCCESS;
}
