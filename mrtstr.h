/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#ifndef __MRTSTR__
#define __MRTSTR__

#include <pthread.h>
#include <stdint.h>

typedef uint64_t mrtstr_size_t;
typedef uint8_t mrtstr_bool_t;

#define MRTSTR_TRUE 1
#define MRTSTR_FALSE 0

#define MRTSTR_THREAD_COUNT 6

struct __MRTSTR_THREAD_T
{
    pthread_t id;

    void (*volatile func)(void*);
    void *args;

    volatile mrtstr_bool_t open;
};
typedef struct __MRTSTR_THREAD_T mrtstr_thread_t;

struct __MRTSTR_THREADS_T
{
    mrtstr_thread_t *threads;
    mrtstr_size_t size;

    volatile mrtstr_size_t free_threads;
    pthread_mutex_t mutex;
};
typedef struct __MRTSTR_THREADS_T mrtstr_threads_t;

extern mrtstr_threads_t mrtstr_threads;

void mrtstr_init_threads(mrtstr_size_t size);
void mrtstr_load_threads(void (*volatile func)(void*), void *args);
void mrtstr_free_threads();

typedef char mrtstr_chr_t;
typedef mrtstr_chr_t *mrtstr_data_t;
typedef const mrtstr_data_t mrtstr_data_ct;

typedef uint8_t mrtstr_bit_t;
typedef volatile mrtstr_bit_t mrtstr_lock_t;

struct __MRTSTR_T
{
    mrtstr_data_t data;
    mrtstr_size_t size;
    mrtstr_size_t alloc;

    mrtstr_lock_t *lock;
    mrtstr_bool_t forced;
    pthread_mutex_t mutex;
};
typedef struct __MRTSTR_T *mrtstr_t;
typedef const mrtstr_t mrtstr_ct;

typedef uint8_t mrtstr_rbit_t;
typedef volatile mrtstr_rbit_t mrtstr_rlock_t;

struct __MRTSTR_BRES_T
{
    volatile mrtstr_bool_t res;

    mrtstr_rlock_t lock;
    pthread_mutex_t mutex;
};
typedef struct __MRTSTR_BRES_T mrtstr_bres_t;

/* init functions */

mrtstr_t mrtstr_init();
void mrtstr_clear(mrtstr_t str);

void mrtstr_realloc(mrtstr_t str, mrtstr_size_t size);

void mrtstr_expand(mrtstr_t str, mrtstr_size_t size);
void mrtstr_expand_add(mrtstr_t str, mrtstr_size_t add);

void mrtstr_shrink(mrtstr_t str, mrtstr_size_t size);
void mrtstr_shrink_sub(mrtstr_t str, mrtstr_size_t sub);

/* set functions */

void mrtstr_set(mrtstr_t dst, mrtstr_ct src);

void mrtstr_set_str(mrtstr_t dst, mrtstr_data_ct src);
void mrtstr_set_nstr(mrtstr_t dst, mrtstr_data_ct src, mrtstr_size_t size);

/* repeat functions */

void mrtstr_repeat_chr(mrtstr_t res, mrtstr_chr_t chr, mrtstr_size_t count);

/* cmp functions */

void mrtstr_equal(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_ct str2);

void mrtstr_equal_str(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2);
void mrtstr_equal_nstr(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2, mrtstr_size_t size);

/* contain functions */

void mrtstr_contain_chr(mrtstr_bres_t *res, mrtstr_ct str, mrtstr_chr_t chr);

#endif /* __MRTSTR__ */
