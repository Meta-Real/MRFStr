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

#include <mrfstr-intern.h>

void mrfstr_reverse(
    mrfstr_t res, mrfstr_ct str)
{
    if (res == str)
    {
        if (MRFSTR_SIZE(res) <= 1)
            return;

        __mrfstr_rev(MRFSTR_DATA(res), MRFSTR_SIZE(res));
        return;
    }

    MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
    if (MRFSTR_SIZE(res))
        __mrfstr_rev2(MRFSTR_DATA(res), MRFSTR_DATA(str) + MRFSTR_SIZE(str), MRFSTR_SIZE(res));
}
