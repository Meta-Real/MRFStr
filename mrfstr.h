/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#ifndef __MRFSTR__
#define __MRFSTR__

#include <stdint.h>

typedef char mrfstr_chr_t;
typedef mrfstr_chr_t *mrfstr_data_t;
typedef const mrfstr_chr_t *mrfstr_data_ct;

typedef uint8_t mrfstr_byte_t;
typedef uint16_t mrfstr_short_t;
typedef uint32_t mrfstr_long_t;
typedef uint64_t mrfstr_size_t;

typedef uint8_t mrfstr_bool_t;
typedef uint64_t mrfstr_idx_t;

#define MRFSTR_TRUE ((mrfstr_bool_t)1)
#define MRFSTR_FALSE ((mrfstr_bool_t)0)

#define MRFSTR_INVIDX ((mrfstr_idx_t)-1)

#define MRFSTR_THREADING 1

#if MRFSTR_THREADING
#define MRFSTR_THREAD_COUNT 6
#endif

struct __MRFSTR_T
{
    mrfstr_data_t data;
    mrfstr_size_t size;
};
typedef struct __MRFSTR_T *mrfstr_t;
typedef const struct __MRFSTR_T *mrfstr_ct;

#define MRFSTR_DATA(x) ((x)->data)
#define MRFSTR_SIZE(x) ((x)->size)

enum __MRFSTR_RES_ENUM
{
    MRFSTR_RES_NOERROR,
    MRFSTR_RES_MEM_ERROR,
    MRFSTR_RES_OVERFLOW_ERROR
};
typedef uint8_t mrfstr_res_enum_t;

/* init functions */

mrfstr_t mrfstr_init();
mrfstr_t mrfstr_init2(
    mrfstr_data_t data);
mrfstr_t mrfstr_init3(
    mrfstr_data_t data, mrfstr_size_t size);

mrfstr_res_enum_t mrfstr_alloc(
    mrfstr_t str,  mrfstr_size_t size);
void mrfstr_free(
    mrfstr_t str);
void mrfstr_clear(
    mrfstr_t str);

mrfstr_res_enum_t mrfstr_realloc(
    mrfstr_t str, mrfstr_size_t size);
mrfstr_res_enum_t mrfstr_clear_realloc(
    mrfstr_t str, mrfstr_size_t size);

mrfstr_res_enum_t mrfstr_expand(
    mrfstr_t str, mrfstr_size_t size);
mrfstr_res_enum_t mrfstr_clear_expand(
    mrfstr_t str, mrfstr_size_t size);

mrfstr_res_enum_t mrfstr_shrink(
    mrfstr_t str, mrfstr_size_t size);
mrfstr_res_enum_t mrfstr_clear_shrink(
    mrfstr_t str, mrfstr_size_t size);

void mrfstr_swap(
    mrfstr_t str1, mrfstr_t str2);

/* set functions */

void mrfstr_set(
    mrfstr_t dst, mrfstr_ct src);

void mrfstr_set_str(
    mrfstr_t dst, mrfstr_data_ct src);
void mrfstr_set_nstr(
    mrfstr_t dst, mrfstr_data_ct src,
    mrfstr_size_t size);

void mrfstr_set_chr(
    mrfstr_t dst, mrfstr_chr_t src);

/* concat functions */

mrfstr_res_enum_t mrfstr_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2);
mrfstr_res_enum_t mrfstr_n_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2,
    mrfstr_size_t size);

/* repeat functions */

mrfstr_res_enum_t mrfstr_repeat(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_size_t count);

void mrfstr_repeat_chr(
    mrfstr_t res, mrfstr_chr_t chr,
    mrfstr_size_t count);

/* reverse functions */

mrfstr_res_enum_t mrfstr_reverse(
    mrfstr_t res, mrfstr_ct str);

/* replace functions */

void mrfstr_replace(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new);
void mrfstr_n_replace(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t old, mrfstr_chr_t new,
    mrfstr_size_t size);

/* cmp functions */

mrfstr_bool_t mrfstr_equal(
    mrfstr_ct str1, mrfstr_ct str2);
mrfstr_bool_t mrfstr_n_equal(
    mrfstr_ct str1, mrfstr_ct str2,
    mrfstr_size_t size);

mrfstr_bool_t mrfstr_equal_str(
    mrfstr_ct str1, mrfstr_data_ct str2);
mrfstr_bool_t mrfstr_n_equal_str(
    mrfstr_ct str1, mrfstr_data_ct str2,
    mrfstr_size_t size);

mrfstr_bool_t mrfstr_equal_nstr(
    mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size);
mrfstr_bool_t mrfstr_n_equal_nstr(
    mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size2,
    mrfstr_size_t size);

mrfstr_bool_t mrfstr_equal_chr(
    mrfstr_ct str, mrfstr_chr_t chr);

/* contain functions */

mrfstr_bool_t mrfstr_contain_chr(
    mrfstr_ct str, mrfstr_chr_t chr);
mrfstr_bool_t mrfstr_n_contain_chr(
    mrfstr_ct str, mrfstr_chr_t chr,
    mrfstr_size_t size);

/* find functions */

mrfstr_idx_t mrfstr_find_chr(
    mrfstr_ct str, mrfstr_chr_t chr);
mrfstr_idx_t mrfstr_n_find_chr(
    mrfstr_ct str, mrfstr_chr_t chr,
    mrfstr_size_t size);

/* util functions */

inline mrfstr_data_t mrfstr_get_data(
    mrfstr_ct str)
{
    return MRFSTR_DATA(str);
}

inline mrfstr_size_t mrfstr_get_size(
    mrfstr_ct str)
{
    return MRFSTR_SIZE(str);
}

inline mrfstr_chr_t mrfstr_get_chr(
    mrfstr_ct str, mrfstr_size_t idx)
{
    return MRFSTR_DATA(str)[idx];
}

inline void mrfstr_modify_chr(
    mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t idx)
{
    MRFSTR_DATA(str)[idx] = chr;
}

void mrfstr_print(
    mrfstr_ct str);
void mrfstr_n_print(
    mrfstr_ct str, mrfstr_size_t size);

#endif /* __MRFSTR__ */
