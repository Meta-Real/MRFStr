/*
MIT License

Copyright (c) 2023 MetaReal

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/

#ifndef __MRFSTR__
#define __MRFSTR__

#ifdef __cplusplus
extern "C"
{
#endif

#include <inttypes.h>
#include <stddef.h>

#ifndef MRFSTR_DONT_INCLUDE_STDIO
#include <stdio.h>
#endif

#define MRFSTR_VERSION "1.1.0"
#define MRFSTR_VERSION_MAJOR 1
#define MRFSTR_VERSION_MINOR 1
#define MRFSTR_VERSION_PATCH 0

/* data types */

typedef uint8_t mrfstr_bool_t;
typedef int8_t mrfstr_sbyte_t;
typedef uint8_t mrfstr_byte_t;
typedef int16_t mrfstr_sshort_t;
typedef uint16_t mrfstr_short_t;
typedef int32_t mrfstr_slong_t;
typedef uint32_t mrfstr_long_t;
typedef int64_t mrfstr_slonglong_t;
typedef uint64_t mrfstr_longlong_t;

typedef size_t mrfstr_idx_t;
typedef size_t mrfstr_size_t;

typedef char mrfstr_chr_t;
typedef mrfstr_chr_t *mrfstr_data_t;
typedef const mrfstr_chr_t *mrfstr_data_ct;

typedef void *mrfstr_ptr_t;
typedef const void *mrfstr_ptr_ct;

#define MRFSTR_TRUE ((mrfstr_bool_t)1)
#define MRFSTR_FALSE ((mrfstr_bool_t)0)

#define MRFSTR_INVIDX ((mrfstr_idx_t)-1)

struct __MRFSTR_T
{
    mrfstr_data_t _data;
    mrfstr_size_t _size;
    mrfstr_size_t _capa;
};
typedef struct __MRFSTR_T *mrfstr_t;
typedef const struct __MRFSTR_T *mrfstr_ct;

#define MRFSTR_DATA(x) ((x)->_data)
#define MRFSTR_SIZE(x) ((x)->_size)
#define MRFSTR_CAPA(x) ((x)->_capa)

enum __MRFSTR_RES_ENUM
{
    MRFSTR_RES_NOERROR,
    MRFSTR_RES_MEM_ERROR,
    MRFSTR_RES_OVERFLOW_ERROR,
    MRFSTR_RES_IDXOUT_ERROR
};
typedef mrfstr_byte_t mrfstr_res_enum_t;

/* init functions */

mrfstr_t mrfstr_init(void);
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

mrfstr_res_enum_t mrfstr_set(
    mrfstr_t dst, mrfstr_ct src);

mrfstr_res_enum_t mrfstr_set_str(
    mrfstr_t dst, mrfstr_data_ct src);
mrfstr_res_enum_t mrfstr_set_nstr(
    mrfstr_t dst, mrfstr_data_ct src,
    mrfstr_size_t size);

mrfstr_res_enum_t mrfstr_set_chr(
    mrfstr_t dst, mrfstr_chr_t src);

/* get functions */

mrfstr_data_t mrfstr_get_str(
    mrfstr_data_t dst, mrfstr_ct src);
mrfstr_data_t mrfstr_n_get_str(
    mrfstr_data_t dst, mrfstr_ct src, mrfstr_size_t size);

/* concat functions */

mrfstr_res_enum_t mrfstr_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2);
mrfstr_res_enum_t mrfstr_n_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2,
    mrfstr_size_t size);

/* remove functions */

mrfstr_res_enum_t mrfstr_remove(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_idx_t idx);
mrfstr_res_enum_t mrfstr_n_remove(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_idx_t idx, mrfstr_size_t count);

/* repeat functions */

mrfstr_res_enum_t mrfstr_repeat(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_size_t count);

mrfstr_res_enum_t mrfstr_repeat_chr(
    mrfstr_t res, mrfstr_chr_t chr,
    mrfstr_size_t count);

/* reverse functions */

mrfstr_res_enum_t mrfstr_reverse(
    mrfstr_t res, mrfstr_ct str);

/* replace functions */

mrfstr_res_enum_t mrfstr_replace_chr(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr);
mrfstr_res_enum_t mrfstr_n_replace_chr(
    mrfstr_t res, mrfstr_ct str,
    mrfstr_chr_t ochr, mrfstr_chr_t nchr,
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

mrfstr_bool_t mrfstr_startswith(
    mrfstr_ct str, mrfstr_ct substr);
mrfstr_bool_t mrfstr_startswith_str(
    mrfstr_ct str, mrfstr_data_ct substr);
mrfstr_bool_t mrfstr_startswith_nstr(
    mrfstr_ct str, mrfstr_data_ct substr, mrfstr_size_t size);

mrfstr_bool_t mrfstr_endswith(
    mrfstr_ct str, mrfstr_ct substr);
mrfstr_bool_t mrfstr_endswith_str(
    mrfstr_ct str, mrfstr_data_ct substr);
mrfstr_bool_t mrfstr_endswith_nstr(
    mrfstr_ct str, mrfstr_data_ct substr, mrfstr_size_t size);

/* find functions */

mrfstr_idx_t mrfstr_find_chr(
    mrfstr_ct str, mrfstr_chr_t chr);
mrfstr_idx_t mrfstr_n_find_chr(
    mrfstr_ct str, mrfstr_chr_t chr,
    mrfstr_size_t size);

/* count functions */

mrfstr_size_t mrfstr_count_chr(
    mrfstr_ct str, mrfstr_chr_t chr);
mrfstr_size_t mrfstr_n_count_chr(
    mrfstr_ct str, mrfstr_chr_t chr,
    mrfstr_size_t size);

/* data functions */

mrfstr_data_t mrfstr_get_data(
    mrfstr_ct str);
mrfstr_size_t mrfstr_get_size(
    mrfstr_ct str);
mrfstr_res_enum_t mrfstr_get_chr(
    mrfstr_chr_t *chr,
    mrfstr_ct str, mrfstr_size_t idx);

mrfstr_res_enum_t mrfstr_modify_chr(
    mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t idx);

/* io functions */

void mrfstr_export(
    mrfstr_ct str);
void mrfstr_n_export(
    mrfstr_ct str, mrfstr_size_t size);

mrfstr_res_enum_t mrfstr_import(
    mrfstr_t str);
mrfstr_res_enum_t mrfstr_n_import(
    mrfstr_t str, mrfstr_size_t size);

#ifndef MRFSTR_DONT_INCLUDE_STDIO
void mrfstr_fexport(
    FILE *stream,
    mrfstr_ct str);
void mrfstr_n_fexport(
    FILE *stream,
    mrfstr_ct str, mrfstr_size_t size);

mrfstr_res_enum_t mrfstr_fimport(
    mrfstr_t str,
    FILE *stream);
mrfstr_res_enum_t mrfstr_n_fimport(
    mrfstr_t str,
    FILE *stream, mrfstr_size_t size);
#endif

/* str functions */

mrfstr_size_t mrfstr_strlen(
    mrfstr_data_ct str);

/* config functions */

enum __MRFSTR_CONFIG_TYPE_ENUM
{
    MRFSTR_CONFIG_TYPE_MEMORY,
    MRFSTR_CONFIG_TYPE_REPLACE,
    MRFSTR_CONFIG_TYPE_REVERSE,

    MRFSTR_CONFIG_TYPE_COMPARE,
    MRFSTR_CONFIG_TYPE_SEARCH,

    MRFSTR_CONFIG_TYPE_STRLEN
};
typedef mrfstr_byte_t mrfstr_config_type_enum_t;

enum __MRFSTR_CONFIG_SIMD_ENUM
{
    MRFSTR_CONFIG_SIMD_AVX512,
    MRFSTR_CONFIG_SIMD_AVX,
    MRFSTR_CONFIG_SIMD_SSE,
    MRFSTR_CONFIG_SIMD_NONE
};
typedef mrfstr_byte_t mrfstr_config_simd_enum_t;

void mrfstr_config_tcount(
    mrfstr_byte_t tcount);
void mrfstr_config_nlimit(
    mrfstr_size_t nlimit);
void mrfstr_config_tlimit(
    mrfstr_size_t tlimit);
void mrfstr_config_stdalloc(
    mrfstr_short_t stdalloc);

void mrfstr_config(
    mrfstr_config_type_enum_t type,
    mrfstr_config_simd_enum_t normal,
    mrfstr_config_simd_enum_t threaded);
void mrfstr_config_str(
    mrfstr_config_type_enum_t type,
    mrfstr_config_simd_enum_t simd);

#ifdef __cplusplus
}
#endif

#endif /* __MRFSTR__ */
