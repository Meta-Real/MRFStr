/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#ifndef __MRTSTR_INTERN__
#define __MRTSTR_INTERN__

#include <mrtstr.h>

#define lock_inc(l, m)           \
    do                           \
    {                            \
        pthread_mutex_lock(m);   \
        (l)++;                   \
        pthread_mutex_unlock(m); \
    } while (0)

#define lock_dec(l, m)           \
    do                           \
    {                            \
        pthread_mutex_lock(m);   \
        (l)--;                   \
        pthread_mutex_unlock(m); \
    } while (0)

inline mrtstr_bool_t mrtstr_locked(mrtstr_ct str)
{
    mrtstr_size_t i;
    for (i = 0; i < MRTSTR_LOCK_COUNT; i++)
        if (str->lock[i])
            return MRTSTR_TRUE;
    return MRTSTR_FALSE;
}

#endif /* __MRTSTR_INTERN__ */
