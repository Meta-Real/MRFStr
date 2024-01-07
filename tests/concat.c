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

    mrfstr_t res = mrfstr_init();
    if (!res)
    {
        fputs("\"concat\" error: Initializing \"res\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(res, TEST_HIGH))
    {
        mrfstr_free(res);

        fputs("\"concat\" error: Allocating \"res\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_t str1 = mrfstr_init();
    if (!str1)
    {
        mrfstr_free(res);

        fputs("\"concat\" error: Initializing \"str1\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(str1, TEST_HIGH / 2))
    {
        mrfstr_free(res);
        mrfstr_free(str1);

        fputs("\"concat\" error: Allocating \"str1\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_t str2 = mrfstr_init();
    if (!str2)
    {
        mrfstr_free(res);
        mrfstr_free(str1);

        fputs("\"concat\" error: Initializing \"str2\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(str2, TEST_HIGH))
    {
        mrfstr_free(res);
        mrfstr_free(str1);
        mrfstr_free(str2);

        fputs("\"concat\" error: Allocating \"str2\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_data_t str = malloc(TEST_HIGH * sizeof(mrfstr_chr_t));
    if (!str)
    {
        mrfstr_free(res);
        mrfstr_free(str1);
        mrfstr_free(str2);

        fputs("\"concat\" error: Allocating \"str\" section\n", stderr);
        return EXIT_FAILURE;
    }

    MRFSTR_SIZE(str1) = TEST_LOW / 2;
    memset(MRFSTR_DATA(str1), '0', MRFSTR_SIZE(str1));
    MRFSTR_SIZE(str2) = MRFSTR_SIZE(str1);
    memset(MRFSTR_DATA(str2), '0', MRFSTR_SIZE(str2));
    memset(str, '0', TEST_LOW);
    mrfstr_concat(res, str1, str2);

    if (MRFSTR_SIZE(res) != TEST_LOW || memcmp(MRFSTR_DATA(res), str, TEST_LOW))
    {
        mrfstr_free(res);
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);

        fputs("\"concat\" error: TEST_LOW three strings section\n", stderr);
        return EXIT_FAILURE;
    }

    MRFSTR_SIZE(res) = TEST_LOW / 2;
    memset(MRFSTR_DATA(res), '1', MRFSTR_SIZE(res));
    memset(MRFSTR_DATA(str1), '1', MRFSTR_SIZE(str1));
    memset(str, '1', TEST_LOW);
    mrfstr_concat(res, res, str1);

    if (MRFSTR_SIZE(res) != TEST_LOW || memcmp(MRFSTR_DATA(res), str, TEST_LOW))
    {
        mrfstr_free(res);
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);

        fputs("\"concat\" error: TEST_LOW two strings section\n", stderr);
        return EXIT_FAILURE;
    }

    MRFSTR_SIZE(str1) = TEST_MID / 2;
    memset(MRFSTR_DATA(str1), '0', MRFSTR_SIZE(str1));
    MRFSTR_SIZE(str2) = MRFSTR_SIZE(str1);
    memset(MRFSTR_DATA(str2), '0', MRFSTR_SIZE(str2));
    memset(str, '0', TEST_MID);
    mrfstr_concat(res, str1, str2);

    if (MRFSTR_SIZE(res) != TEST_MID || memcmp(MRFSTR_DATA(res), str, TEST_MID))
    {
        mrfstr_free(res);
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);

        fputs("\"concat\" error: TEST_MID three strings section\n", stderr);
        return EXIT_FAILURE;
    }

    MRFSTR_SIZE(res) = TEST_MID / 2;
    memset(MRFSTR_DATA(res), '1', MRFSTR_SIZE(res));
    memset(MRFSTR_DATA(str1), '1', MRFSTR_SIZE(str1));
    memset(str, '1', TEST_MID);
    mrfstr_concat(res, res, str1);

    if (MRFSTR_SIZE(res) != TEST_MID || memcmp(MRFSTR_DATA(res), str, TEST_MID))
    {
        mrfstr_free(res);
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);

        fputs("\"concat\" error: TEST_LOW two strings section\n", stderr);
        return EXIT_FAILURE;
    }

    MRFSTR_SIZE(str1) = TEST_HIGH / 2;
    memset(MRFSTR_DATA(str1), '0', MRFSTR_SIZE(str1));
    MRFSTR_SIZE(str2) = MRFSTR_SIZE(str1);
    memset(MRFSTR_DATA(str2), '0', MRFSTR_SIZE(str2));
    memset(str, '0', TEST_HIGH);
    mrfstr_concat(res, str1, str2);

    if (MRFSTR_SIZE(res) != TEST_HIGH || memcmp(MRFSTR_DATA(res), str, TEST_HIGH))
    {
        mrfstr_free(res);
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);

        fputs("\"concat\" error: TEST_HIGH three strings section\n", stderr);
        return EXIT_FAILURE;
    }

    MRFSTR_SIZE(res) = TEST_HIGH / 2;
    memset(MRFSTR_DATA(res), '1', MRFSTR_SIZE(res));
    memset(MRFSTR_DATA(str1), '1', MRFSTR_SIZE(str1));
    memset(str, '1', TEST_HIGH);
    mrfstr_concat(res, res, str1);

    if (MRFSTR_SIZE(res) != TEST_HIGH || memcmp(MRFSTR_DATA(res), str, TEST_HIGH))
    {
        mrfstr_free(res);
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);

        fputs("\"concat\" error: TEST_LOW two strings section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_free(res);
    mrfstr_free(str1);
    mrfstr_free(str2);
    free(str);
    return EXIT_SUCCESS;
}
