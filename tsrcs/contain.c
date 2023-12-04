/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

void mrtstr_contain_chr(mrtstr_bres_t *res, mrtstr_ct str, mrtstr_chr_t chr)
{
    res->lock = 0;
    pthread_mutex_init(&res->mutex, NULL);

    if (!str->size)
    {
        res->res = MRTSTR_FALSE;
        return;
    }

    mrtstr_memchr(res, str, chr, str->size);
}
