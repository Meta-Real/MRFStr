/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr.h>
#include <stddef.h>

mrfstr_data_t mrfstr_get_data(mrfstr_ct str)
{
    if (!str->size)
        return NULL;

    return str->data;
}

mrfstr_size_t mrfstr_get_size(mrfstr_ct str)
{
    return str->size;
}
