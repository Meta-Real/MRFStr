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

    mrfstr_t str1 = mrfstr_init();
    if (!str1)
    {
        fputs("\"reverse\" error: Initializing \"str1\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(str1, TEST_HIGH))
    {
        mrfstr_free(str1);

        fputs("\"reverse\" error: Allocating \"str1\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_t str2 = mrfstr_init();
    if (!str2)
    {
        mrfstr_free(str1);

        fputs("\"reverse\" error: Initializing \"str2\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(str2, TEST_HIGH))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);

        fputs("\"reverse\" error: Allocating \"str2\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_data_t str = malloc(TEST_HIGH * sizeof(mrfstr_chr_t));
    if (!str)
    {
        mrfstr_free(str1);
        mrfstr_free(str2);

        fputs("\"reverse\" error: Allocating \"str\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_data_t ptr = str;
    mrfstr_size_t i;
    mrfstr_sshort_t j;
    for (i = 0; i < TEST_HIGH; i += 0x100)
        for (j = 0; j != 0x100; j++)
            *ptr++ = (mrfstr_chr_t)j;

    mrfstr_data_t strr = malloc(TEST_HIGH * sizeof(mrfstr_chr_t));
    if (!strr)
    {
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);

        fputs("\"reverse\" error: Allocating \"strr\" section\n", stderr);
        return EXIT_FAILURE;
    }

    ptr = strr;
    for (i = 0; i < TEST_HIGH; i += 0x100)
        for (j = 0xff; j != -1; j--)
            *ptr++ = (mrfstr_chr_t)j;

    memcpy(MRFSTR_DATA(str1), str, TEST_LOW);
    MRFSTR_SIZE(str1) = TEST_LOW;

    mrfstr_reverse(str1, str1);

    if (MRFSTR_SIZE(str1) != TEST_LOW || memcmp(MRFSTR_DATA(str1), strr, TEST_LOW))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);
        free(strr);

        fputs("\"reverse\" error: TEST_LOW one string section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_reverse(str2, str1);

    if (MRFSTR_SIZE(str2) != TEST_LOW || memcmp(MRFSTR_DATA(str2), str, TEST_LOW))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);
        free(strr);

        fputs("\"reverse\" error: TEST_LOW two strings section\n", stderr);
        return EXIT_FAILURE;
    }

    memcpy(MRFSTR_DATA(str1), str, TEST_MID);
    MRFSTR_SIZE(str1) = TEST_MID;

    mrfstr_reverse(str1, str1);

    if (MRFSTR_SIZE(str1) != TEST_MID || memcmp(MRFSTR_DATA(str1), strr, TEST_MID))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);
        free(strr);

        fputs("\"reverse\" error: TEST_MID one string section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_reverse(str2, str1);

    if (MRFSTR_SIZE(str2) != TEST_MID || memcmp(MRFSTR_DATA(str2), str, TEST_MID))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);
        free(strr);

        fputs("\"reverse\" error: TEST_MID two strings section\n", stderr);
        return EXIT_FAILURE;
    }

    memcpy(MRFSTR_DATA(str1), str, TEST_HIGH);
    MRFSTR_SIZE(str1) = TEST_HIGH;

    mrfstr_reverse(str1, str1);

    if (MRFSTR_SIZE(str1) != TEST_HIGH || memcmp(MRFSTR_DATA(str1), strr, TEST_HIGH))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);
        free(strr);

        fputs("\"reverse\" error: TEST_HIGH one string section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_reverse(str2, str1);

    if (MRFSTR_SIZE(str2) != TEST_HIGH || memcmp(MRFSTR_DATA(str2), str, TEST_HIGH))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);
        free(str);
        free(strr);

        fputs("\"reverse\" error: TEST_HIGH two strings section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_free(str1);
    mrfstr_free(str2);
    free(str);
    free(strr);
    return EXIT_SUCCESS;
}
