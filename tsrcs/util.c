/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr.h>

mrtstr_data_t mrtstr_get_data(mrtstr_ct str)
{
    for (; mrtstr_locked(str););

    if (!str->size)
        return NULL;

    return str->data;
}

mrtstr_size_t mrtstr_get_size(mrtstr_ct str)
{
    return str->size;
}

mrtstr_bool_t mrtstr_locked(mrtstr_ct str)
{
    mrtstr_size_t i;
    for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        if (str->lock[i])
            return MRTSTR_TRUE;
    return MRTSTR_FALSE;
}

mrtstr_bool_t mrtstr_bres_extract(mrtstr_bres_t *res)
{
    for (; res->lock;);
    return res->res;
}
