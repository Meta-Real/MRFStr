/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr.h>

mrfstr_data_t mrfstr_get_data(
    mrfstr_ct str)
{
    return MRFSTR_DATA(str);
}

mrfstr_size_t mrfstr_get_size(
    mrfstr_ct str)
{
    return MRFSTR_SIZE(str);
}

mrfstr_chr_t mrfstr_get_chr(
    mrfstr_res_enum_t *error,
    mrfstr_ct str, mrfstr_size_t idx)
{
    if (idx >= MRFSTR_SIZE(str))
    {
        if (error)
            *error = MRFSTR_RES_IDXOUT_ERROR;
        return MRFSTR_DATA(str)[MRFSTR_SIZE(str) - 1];
    }
    else if (error)
        *error = MRFSTR_RES_NOERROR;

    return MRFSTR_DATA(str)[idx];
}

mrfstr_res_enum_t mrfstr_modify_chr(
    mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t idx)
{
    if (idx >= MRFSTR_SIZE(str))
        return MRFSTR_RES_IDXOUT_ERROR;

    MRFSTR_DATA(str)[idx] = chr;
    return MRFSTR_RES_NOERROR;
}
