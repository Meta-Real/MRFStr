/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#ifndef __MRTSTR_INTERN__
#define __MRTSTR_INTERN__

#include <mrtstr.h>

#define MRTSTR_THREAD_LIMIT (4194368 * MRTSTR_THREAD_COUNT - 1)
#define MRTSTR_THREAD_CHUNK (64 * MRTSTR_THREAD_COUNT)

#define mrtstr_lock_inc(l, m)    \
    do                           \
    {                            \
        pthread_mutex_lock(m);   \
        (l)++;                   \
        pthread_mutex_unlock(m); \
    } while (0)

#define mrtstr_lock_dec(l, m)    \
    do                           \
    {                            \
        pthread_mutex_lock(m);   \
        (l)--;                   \
        pthread_mutex_unlock(m); \
    } while (0)

mrtstr_bool_t mrtstr_locked(mrtstr_ct str);

#endif /* __MRTSTR_INTERN__ */
