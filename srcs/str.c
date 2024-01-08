/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#include <mrfstr-intern.h>
#include <binary.h>

mrfstr_size_t mrfstr_strlen(
    mrfstr_data_ct str)
{
    mrfstr_byte_t align = (uintptr_t)str % _mrfstr_config.strlen_size;
    for (mrfstr_byte_t i = 0; i != align; i++)
        if (*str++ == '\0')
            return i;

    return _mrfstr_config.strlen_sub(str) + align;
}
