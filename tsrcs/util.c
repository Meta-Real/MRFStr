/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <stdio.h>

mrtstr_data_t mrtstr_get_data(mrtstr_ct str)
{
    if (!str->size)
        return NULL;
    
    for (; mrtstr_locked(str););

    return str->data;
}

mrtstr_size_t mrtstr_get_size(mrtstr_ct str)
{
    return str->size;
}

mrtstr_bool_t mrtstr_locked(mrtstr_ct str)
{
    mrtstr_bit_t i;
    for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        if (str->lock[i])
            return MRTSTR_TRUE;
    return MRTSTR_FALSE;
}

mrtstr_chr_t mrtstr_get_chr(mrtstr_ct str, mrtstr_size_t idx)
{
    if (!str->forced)
        return str->data[idx];

    mrtstr_size_t part = (idx / MRTSTR_SIMD_SIZE) /
        (str->size / (MRTSTR_SIMD_SIZE * MRTSTR_THREAD_COUNT));
    if (part == MRTSTR_THREAD_COUNT)
        return str->data[idx];

    for (; str->lock[part];);
    return str->data[idx];
}

void mrtstr_modify_chr(mrtstr_ct str, mrtstr_chr_t chr, mrtstr_size_t idx)
{
    mrtstr_size_t part = (idx / MRTSTR_SIMD_SIZE) /
        (str->size / (MRTSTR_SIMD_SIZE * MRTSTR_THREAD_COUNT));
    if (part == MRTSTR_THREAD_COUNT)
    {
        str->data[idx] = chr;
        return;
    }

    for (; str->lock[part];);
    str->data[idx] = chr;
}

void mrtstr_print(mrtstr_ct str)
{
    if (!str->size)
        return;

    for (; mrtstr_locked(str););
    fputs(str->data, stdout);
}
