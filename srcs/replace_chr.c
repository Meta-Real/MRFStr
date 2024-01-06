/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>

void mrfstr_replace_chr(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new)
{
    if (!MRFSTR_SIZE(str))
    {
        free(MRFSTR_DATA(res));

        MRFSTR_DATA(res) = NULL;
        MRFSTR_SIZE(res) = 0;
        return;
    }

    if (old == new)
    {
        if (res == str)
            return;

        __mrfstr_copy(MRFSTR_DATA(res), MRFSTR_DATA(str), MRFSTR_SIZE(str));
        MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
        return;
    }

    if (res == str)
    {
        __mrfstr_replace_chr(MRFSTR_DATA(res), old, new, MRFSTR_SIZE(res));
        return;
    }

    __mrfstr_replace_chr2(MRFSTR_DATA(res), MRFSTR_DATA(str), old, new, MRFSTR_SIZE(str));
    MRFSTR_SIZE(res) = MRFSTR_SIZE(str);
}
