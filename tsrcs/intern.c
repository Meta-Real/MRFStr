/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <stdio.h>

mrtstr_bool_t mrtstr_locked(mrtstr_ct str)
{
    mrtstr_size_t i;
    for (i = 0; i < MRTSTR_THREAD_COUNT; i++)
        if (str->lock[i])
            return MRTSTR_TRUE;
    return MRTSTR_FALSE;
}
