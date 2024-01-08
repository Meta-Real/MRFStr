/*
MIT License

Copyright (c) 2023 MetaReal

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
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
        fputs("\"equal\" error: Initializing \"str1\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(str1, TEST_HIGH))
    {
        mrfstr_free(str1);

        fputs("\"equal\" error: Allocating \"str1\" section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_t str2 = mrfstr_init();
    if (!str2)
    {
        mrfstr_free(str1);

        fputs("\"equal\" error: Initializing \"str2\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(str2, TEST_HIGH))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);

        fputs("\"equal\" error: Allocating \"str2\" section\n", stderr);
        return EXIT_FAILURE;
    }

    memset(MRFSTR_DATA(str1), '0', TEST_LOW);
    MRFSTR_SIZE(str1) = TEST_LOW;
    memset(MRFSTR_DATA(str2), '0', TEST_LOW);
    MRFSTR_SIZE(str2) = TEST_LOW;

    if (!mrfstr_equal(str1, str2))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);

        fputs("\"equal\" error: TEST_LOW section\n", stderr);
        return EXIT_FAILURE;
    }

    memset(MRFSTR_DATA(str1), '1', TEST_MID);
    MRFSTR_SIZE(str1) = TEST_MID;
    memset(MRFSTR_DATA(str2), '1', TEST_MID);
    MRFSTR_SIZE(str2) = TEST_MID;

    if (!mrfstr_equal(str1, str2))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);

        fputs("\"equal\" error: TEST_MID section\n", stderr);
        return EXIT_FAILURE;
    }

    memset(MRFSTR_DATA(str1), '2', TEST_HIGH);
    MRFSTR_SIZE(str1) = TEST_MID;
    memset(MRFSTR_DATA(str2), '2', TEST_HIGH);
    MRFSTR_SIZE(str2) = TEST_MID;

    if (!mrfstr_equal(str1, str2))
    {
        mrfstr_free(str1);
        mrfstr_free(str2);

        fputs("\"equal\" error: TEST_HIGH section\n", stderr);
        return EXIT_FAILURE;
    }

    mrfstr_free(str1);
    mrfstr_free(str2);
    return EXIT_SUCCESS;
}
