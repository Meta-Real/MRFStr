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
#include <time.h>
#include <inttypes.h>

#define TEST_LOW 0x200
#define TEST_MID 0x100000
#define TEST_HIGH 0x40000000

#define TEST_COUNT 100
#define GENERATE32_RAND (((mrfstr_long_t)rand() << 16) + rand())

int main(void)
{
    mrfstr_config_tcount(5);

    mrfstr_t str = mrfstr_init();
    if (!str)
    {
        fputs("\"contain_chr\" error: Initializing \"str\" section\n", stderr);
        return EXIT_FAILURE;
    }

    if (mrfstr_alloc(str, TEST_HIGH))
    {
        mrfstr_free(str);

        fputs("\"contain_chr\" error: Allocating \"str\" section\n", stderr);
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));
    mrfstr_byte_t i;
    mrfstr_idx_t idx;

    memset(MRFSTR_DATA(str), '0', TEST_LOW);
    MRFSTR_SIZE(str) = TEST_LOW;

    if (mrfstr_contain_chr(str, '1'))
    {
        mrfstr_free(str);

        fputs("\"contain_chr\" error: TEST_LOW all zero section\n", stderr);
        return EXIT_FAILURE;
    }

    for (i = 0; i != TEST_COUNT;)
    {
        idx = rand() % TEST_LOW;
        if (MRFSTR_DATA(str)[idx] == '2')
            continue;

        MRFSTR_DATA(str)[idx] = '1';
        if (!mrfstr_contain_chr(str, '1'))
        {
            mrfstr_free(str);

            fprintf(stderr, "\"contain_chr\" error: TEST_LOW random one section\n"
                "\tFailed index: %zu\n", idx);
            return EXIT_FAILURE;
        }

        MRFSTR_DATA(str)[idx] = '2';
        i++;
    }

    memset(MRFSTR_DATA(str), '0', TEST_MID);
    MRFSTR_SIZE(str) = TEST_MID;

    if (mrfstr_contain_chr(str, '1'))
    {
        mrfstr_free(str);

        fputs("\"contain_chr\" error: TEST_MID all zero section\n", stderr);
        return EXIT_FAILURE;
    }

    for (i = 0; i != TEST_COUNT;)
    {
        idx = GENERATE32_RAND % TEST_MID;
        if (MRFSTR_DATA(str)[idx] == '2')
            continue;

        MRFSTR_DATA(str)[idx] = '1';
        if (!mrfstr_contain_chr(str, '1'))
        {
            mrfstr_free(str);

            fprintf(stderr, "\"contain_chr\" error: TEST_MID random one section\n"
                "\tFailed index: %zu\n", idx);
            return EXIT_FAILURE;
        }

        MRFSTR_DATA(str)[idx] = '2';
        i++;
    }

    memset(MRFSTR_DATA(str), '0', TEST_HIGH);
    MRFSTR_SIZE(str) = TEST_HIGH;

    if (mrfstr_contain_chr(str, '1'))
    {
        mrfstr_free(str);

        fputs("\"contain_chr\" error: TEST_HIGH all zero section\n", stderr);
        return EXIT_FAILURE;
    }

    for (i = 0; i != TEST_COUNT;)
    {
        idx = GENERATE32_RAND % TEST_HIGH;
        if (MRFSTR_DATA(str)[idx] == '2')
            continue;

        MRFSTR_DATA(str)[idx] = '1';
        if (!mrfstr_contain_chr(str, '1'))
        {
            mrfstr_free(str);

            fprintf(stderr, "\"contain_chr\" error: TEST_HIGH random one section\n"
                "\tFailed index: %zu\n", idx);
            return EXIT_FAILURE;
        }

        MRFSTR_DATA(str)[idx] = '2';
        i++;
    }

    mrfstr_free(str);
    return EXIT_SUCCESS;
}
