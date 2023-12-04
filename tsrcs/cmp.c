/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>
#include <string.h>

void mrtstr_equal(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_ct str2)
{
    res->lock = 0;
    pthread_mutex_init(&res->mutex, NULL);

    if (str1->size != str2->size)
    {
        res->res = MRTSTR_FALSE;
        return;
    }

    if (!str1->size || str1->data == str2->data)
    {
        res->res = MRTSTR_TRUE;
        return;
    }

    mrtstr_memcmp(res, str1, str2, str1->size);
}

void mrtstr_equal_str(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2)
{
    res->lock = 0;
    pthread_mutex_init(&res->mutex, NULL);

    mrtstr_size_t size = strlen(str2);
    if (str1->size != size)
    {
        res->res = MRTSTR_FALSE;
        return;
    }

    if (!str1->size || str1->data == str2)
    {
        res->res = MRTSTR_TRUE;
        return;
    }

    mrtstr_memcmp2(res, str1, str2, size);
}

void mrtstr_equal_nstr(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2, mrtstr_size_t size)
{
    res->lock = 0;
    pthread_mutex_init(&res->mutex, NULL);

    if (str1->size != size)
    {
        res->res = MRTSTR_FALSE;
        return;
    }

    if (!str1->size || str1->data == str2)
    {
        res->res = MRTSTR_TRUE;
        return;
    }

    mrtstr_memcmp2(res, str1, str2, size);
}
