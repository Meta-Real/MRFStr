/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#ifndef __MRTSTR__
#define __MRTSTR__

#include <pthread.h>

typedef unsigned long long mrtstr_size_t;
typedef unsigned char mrtstr_bool_t;

#define MRTSTR_TRUE 1
#define MRTSTR_FALSE 0

#define MRTSTR_THREAD_COUNT 6

struct __MRTSTR_THREAD_T
{
    pthread_t id;

    void (*func)(void*);
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

enum __MRTSTR_RES_ENUM
{
    MRTSTR_RES_NOERROR,
    MRTSTR_RES_MEM_ERROR,
    MRTSTR_RES_THREAD_ERROR,
};
typedef unsigned char mrtstr_res_enum_t;

extern mrtstr_threads_t mrtstr_threads;

mrtstr_res_enum_t mrtstr_init_threads(mrtstr_size_t size);
void mrtstr_free_threads();

typedef char mrtstr_chr_t;
typedef mrtstr_chr_t *mrtstr_data_t;
typedef const mrtstr_data_t mrtstr_data_ct;

typedef unsigned char mrtstr_bit_t;
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

typedef unsigned char mrtstr_rbit_t;
typedef volatile mrtstr_rbit_t mrtstr_rlock_t;

struct __MRTSTR_BRES_T
{
    volatile mrtstr_bool_t res;

    mrtstr_rlock_t lock;
    pthread_mutex_t mutex;
};
typedef struct __MRTSTR_BRES_T mrtstr_bres_t;

mrtstr_res_enum_t mrtstr_init_bres(mrtstr_bres_t *bres);
void mrtstr_free_bres(mrtstr_bres_t *bres);

mrtstr_bool_t mrtstr_extract_bres(mrtstr_bres_t *bres);

/* init functions */

mrtstr_t mrtstr_init();
mrtstr_t mrtstr_init2(mrtstr_data_t data);
mrtstr_t mrtstr_init3(mrtstr_data_t data, mrtstr_size_t size);

mrtstr_res_enum_t mrtstr_alloc(mrtstr_t str, mrtstr_size_t size);
void mrtstr_free(mrtstr_t str);
void mrtstr_clear(mrtstr_t str);

mrtstr_res_enum_t mrtstr_realloc(mrtstr_t str, mrtstr_size_t size);
mrtstr_res_enum_t mrtstr_clear_realloc(mrtstr_t str, mrtstr_size_t size);

mrtstr_res_enum_t mrtstr_expand(mrtstr_t str, mrtstr_size_t size);
mrtstr_res_enum_t mrtstr_clear_expand(mrtstr_t str, mrtstr_size_t size);
mrtstr_res_enum_t mrtstr_expand_add(mrtstr_t str, mrtstr_size_t add);
mrtstr_res_enum_t mrtstr_clear_expand_add(mrtstr_t str, mrtstr_size_t add);

mrtstr_res_enum_t mrtstr_shrink(mrtstr_t str, mrtstr_size_t size);
mrtstr_res_enum_t mrtstr_clear_shrink(mrtstr_t str, mrtstr_size_t size);
mrtstr_res_enum_t mrtstr_shrink_sub(mrtstr_t str, mrtstr_size_t sub);
mrtstr_res_enum_t mrtstr_clear_shrink_sub(mrtstr_t str, mrtstr_size_t sub);

void mrtstr_swap(mrtstr_ct str1, mrtstr_ct str2);

/* set functions */

mrtstr_res_enum_t mrtstr_set(mrtstr_t dst, mrtstr_ct src);

mrtstr_res_enum_t mrtstr_set_str(mrtstr_t dst, mrtstr_data_ct src);
mrtstr_res_enum_t mrtstr_set_nstr(mrtstr_t dst, mrtstr_data_ct src, mrtstr_size_t size);

/* repeat functions */

mrtstr_res_enum_t mrtstr_repeat_chr(mrtstr_t res, mrtstr_chr_t chr, mrtstr_size_t count);

/* cmp functions */

mrtstr_res_enum_t mrtstr_equal(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_ct str2);

mrtstr_res_enum_t mrtstr_equal_str(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2);
mrtstr_res_enum_t mrtstr_equal_nstr(mrtstr_bres_t *res, mrtstr_ct str1, mrtstr_data_ct str2, mrtstr_size_t size);

/* contain functions */

mrtstr_res_enum_t mrtstr_contain_chr(mrtstr_bres_t *res, mrtstr_ct str, mrtstr_chr_t chr);

/* util functions */

mrtstr_data_t mrtstr_get_data(mrtstr_ct str);
mrtstr_size_t mrtstr_get_size(mrtstr_ct str);

mrtstr_bool_t mrtstr_locked(mrtstr_ct str);

#endif /* __MRTSTR__ */
