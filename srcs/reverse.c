/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
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

    if (!MRFSTR_SIZE(str))
    {
        free(MRFSTR_DATA(res));

        MRFSTR_DATA(res) = NULL;
        MRFSTR_SIZE(res) = 0;
        return;
    }

    __mrfstr_rev2(MRFSTR_DATA(res), MRFSTR_DATA(str) + MRFSTR_SIZE(str), MRFSTR_SIZE(str));
    MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
}
