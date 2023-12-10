/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr.h>

mrtstr_res_enum_t mrtstr_init_bres(mrtstr_bres_t *bres)
{
    bres->lock = 0;
    return pthread_mutex_init(&bres->mutex, NULL) ?
        MRTSTR_RES_THREAD_ERROR : MRTSTR_RES_NOERROR;
}

void mrtstr_free_bres(mrtstr_bres_t *bres)
{
    for (; bres->lock;);
    pthread_mutex_lock(&bres->mutex);
    pthread_mutex_unlock(&bres->mutex);
    pthread_mutex_destroy(&bres->mutex);
}

mrtstr_bool_t mrtstr_extract_bres(mrtstr_bres_t *bres)
{
    for (; bres->lock;);
    return bres->res;
}
