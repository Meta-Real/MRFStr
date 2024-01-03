/*
    MRFStr Library version 0.1.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <string.h>

void mrfstr_set(
    mrfstr_t dst, mrfstr_ct src)
{
    if (!MRFSTR_SIZE(src))
    {
        free(MRFSTR_DATA(dst));

        MRFSTR_DATA(dst) = NULL;
        MRFSTR_SIZE(dst) = 0;
        return;
    }

    mrfstr_copy(MRFSTR_DATA(dst), MRFSTR_DATA(src), MRFSTR_SIZE(src));
    MRFSTR_SIZE(dst) = MRFSTR_SIZE(src);
}

void mrfstr_set_str(
    mrfstr_t dst, mrfstr_data_ct src)
{
    mrfstr_size_t size = strlen(src);
    if (!size)
    {
        free(MRFSTR_DATA(dst));

        MRFSTR_DATA(dst) = NULL;
        MRFSTR_SIZE(dst) = 0;
        return;
    }

    mrfstr_copy(MRFSTR_DATA(dst), src, size);
    MRFSTR_SIZE(dst) = size;
}

void mrfstr_set_nstr(
    mrfstr_t dst, mrfstr_data_ct src,
    mrfstr_size_t size)
{
    if (!size)
    {
        free(MRFSTR_DATA(dst));

        MRFSTR_DATA(dst) = NULL;
        MRFSTR_SIZE(dst) = 0;
        return;
    }

    mrfstr_copy(MRFSTR_DATA(dst), src, size);
    MRFSTR_SIZE(dst) = size;
}

void mrfstr_set_chr(
    mrfstr_t dst, mrfstr_chr_t src)
{
    *MRFSTR_DATA(dst) = src;
    MRFSTR_SIZE(dst) = 1;
}
