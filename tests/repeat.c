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

    mrfstr_t res = mrfstr_init();
    if (!res)
    {
        fputs("\"repeat\" error: Initializing \"res\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(res, TEST_HIGH))
    {
        mrfstr_free(res);

        fputs("\"repeat\" error: Allocating \"res\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_data_t str = malloc(TEST_HIGH * sizeof(mrfstr_chr_t));
    if (!str)
    {
        mrfstr_free(res);

        fputs("\"repeat\" error: Allocating \"str\" section\n", stderr);
        return EXIT_FAILURE;
    }

    memcpy(MRFSTR_DATA(res), "00000000", 8);
    MRFSTR_SIZE(res) = 8;
    mrfstr_repeat(res, res, TEST_LOW / 8);
    memset(str, '0', TEST_LOW);

    if (MRFSTR_SIZE(res) != TEST_LOW || memcmp(MRFSTR_DATA(res), str, TEST_LOW))
    {
        mrfstr_free(res);
        free(str);

        fputs("\"repeat\" error: TEST_LOW section\n", stderr);
        return EXIT_FAILURE;
    }

    memcpy(MRFSTR_DATA(res), "11111111", 8);
    MRFSTR_SIZE(res) = 8;
    mrfstr_repeat(res, res, TEST_MID / 8);
    memset(str, '1', TEST_MID);

    if (MRFSTR_SIZE(res) != TEST_MID || memcmp(MRFSTR_DATA(res), str, TEST_MID))
    {
        mrfstr_free(res);
        free(str);

        fputs("\"repeat\" error: TEST_MID section\n", stderr);
        return EXIT_FAILURE;
    }

    memcpy(MRFSTR_DATA(res), "22222222", 8);
    MRFSTR_SIZE(res) = 8;
    mrfstr_repeat(res, res, TEST_HIGH / 8);
    memset(str, '2', TEST_HIGH);

    if (MRFSTR_SIZE(res) != TEST_HIGH || memcmp(MRFSTR_DATA(res), str, TEST_HIGH))
    {
        mrfstr_free(res);
        free(str);

        fputs("\"repeat\" error: TEST_HIGH section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_free(res);
    free(str);
    return EXIT_SUCCESS;
}
