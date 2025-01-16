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

#define MRFSTR_VERSION "1.2.0"
#define MRFSTR_VERSION_MAJOR 1
#define MRFSTR_VERSION_MINOR 2
#define MRFSTR_VERSION_PATCH 0

#ifdef __GNUC__
#define __MRFSTR_DECLSPEC_EXPORT __declspec(__dllexport__)
#define __MRFSTR_DECLSPEC_IMPORT
#elif defined(_MSC_VER)
#define __MRFSTR_DECLSPEC_EXPORT __declspec(dllexport)
#define __MRFSTR_DECLSPEC_IMPORT __declspec(dllimport)
#else
#define __MRFSTR_DECLSPEC_EXPORT
#define __MRFSTR_DECLSPEC_IMPORT
#endif

#ifdef __MRFSTR_SHARED__
#ifdef __MRFSTR_COMPILE_TIME__
#define __MRFSTR_DECLSPEC __MRFSTR_DECLSPEC_EXPORT
#else
#define __MRFSTR_DECLSPEC __MRFSTR_DECLSPEC_IMPORT
#endif
#else
#define __MRFSTR_DECLSPEC
#endif

#ifndef MRFSTR_DONT_INCLUDE_STDIO
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define __USE_LARGEFILE64
#endif

#include <stdio.h>
#endif

#if defined(FILE) || defined(H_STDIO) || defined(_STDIO_H) || defined(_FILE_DEFINED)
#define MRFSTR_HAVE_FILE
#endif

#ifdef __GNUC__
#define extern __inline__
#elif defined(_MSC_VER)
#define __MRFSTR_INLINE __inline
#elif defined(__cplusplus)
#define __MRFSTR_INLINE inline
#endif

/* data types */

typedef uint8_t mrfstr_bool_t;
typedef int8_t mrfstr_byte_t;
typedef uint8_t mrfstr_ubyte_t;
typedef int16_t mrfstr_short_t;
typedef uint16_t mrfstr_ushort_t;
typedef int32_t mrfstr_int_t;
typedef uint32_t mrfstr_uint_t;
typedef int64_t mrfstr_long_t;
typedef uint64_t mrfstr_ulong_t;

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
typedef struct __MRFSTR_T mrfstr_t[1];
typedef struct __MRFSTR_T *mrfstr_p;

typedef const struct __MRFSTR_T mrfstr_ct[1];

#define MRFSTR_DATA(x) ((x)->_data)
#define MRFSTR_SIZE(x) ((x)->_size)
#define MRFSTR_CAPA(x) ((x)->_capa)

enum __MRFSTR_RES_ENUM
{
    MRFSTR_RES_NOERROR,
    MRFSTR_RES_MEM_ERROR,
    MRFSTR_RES_OVERFLOW_ERROR,
    MRFSTR_RES_IDXOUT_ERROR,
    MRFSTR_RES_READ_ERROR,
    MRFSTR_RES_TYPE_ERROR,
    MRFSTR_RES_SUPPORT_ERROR
};
typedef mrfstr_ubyte_t mrfstr_res_t;

/* init functions */

__MRFSTR_DECLSPEC void mrfstr_init(
    mrfstr_t str);
__MRFSTR_DECLSPEC void mrfstr_inits(
    mrfstr_p str, ...);

__MRFSTR_DECLSPEC void mrfstr_init_str(
    mrfstr_t str, mrfstr_data_t data);
__MRFSTR_DECLSPEC void mrfstr_init_nstr(
    mrfstr_t str, mrfstr_data_t data, mrfstr_size_t size);
__MRFSTR_DECLSPEC void mrfstr_init_nstr2(
    mrfstr_t str, mrfstr_data_t data, mrfstr_size_t size, mrfstr_size_t capa);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_init_alloc(
    mrfstr_t str, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_alloc(
    mrfstr_t str,  mrfstr_size_t size);

__MRFSTR_DECLSPEC void mrfstr_free(
    mrfstr_t str);
__MRFSTR_DECLSPEC void mrfstr_frees(
    mrfstr_p str, ...);

__MRFSTR_DECLSPEC void mrfstr_clear(
    mrfstr_t str);
__MRFSTR_DECLSPEC void mrfstr_clears(
    mrfstr_p str, ...);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_realloc(
    mrfstr_t str, mrfstr_size_t size);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_clear_realloc(
    mrfstr_t str, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_expand(
    mrfstr_t str, mrfstr_size_t size);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_clear_expand(
    mrfstr_t str, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_shrink(
    mrfstr_t str, mrfstr_size_t size);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_clear_shrink(
    mrfstr_t str, mrfstr_size_t size);

__MRFSTR_DECLSPEC void mrfstr_swap(
    mrfstr_t str1, mrfstr_t str2);

/* set functions */

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_set(
    mrfstr_t dst, mrfstr_ct src);

#define mrfstr_set_str(dst, src) mrfstr_set_nstr((dst), (src), mrfstr_strlen(src))
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_set_nstr(
    mrfstr_t dst, mrfstr_data_ct src, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_set_chr(
    mrfstr_t dst, mrfstr_chr_t src);

/* get functions */

__MRFSTR_DECLSPEC mrfstr_data_t mrfstr_get_str(
    mrfstr_data_t dst, mrfstr_ct src);
__MRFSTR_DECLSPEC mrfstr_data_t mrfstr_n_get_str(
    mrfstr_data_t dst, mrfstr_ct src, mrfstr_size_t size);

/* concat functions */

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_n_concat(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2, mrfstr_size_t size);

#define mrfstr_concat_str(res, str1, str2) mrfstr_concat_nstr((res), (str1), (str2), mrfstr_strlen(str2))
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_concat_nstr(
    mrfstr_t res, mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size2);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_concat_chr(
    mrfstr_t res, mrfstr_ct str, mrfstr_chr_t chr);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_insert(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2, mrfstr_idx_t idx);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_n_insert(
    mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2, mrfstr_idx_t idx, mrfstr_size_t size);

#define mrfstr_insert_str(res, str1, str2, idx) mrfstr_insert_nstr((res), (str1), (str2), mrfstr_strlen(str2), (idx))
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_insert_nstr(
    mrfstr_t res, mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size2, mrfstr_idx_t idx);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_insert_chr(
    mrfstr_t res, mrfstr_ct str, mrfstr_chr_t chr, mrfstr_idx_t idx);

/* remove functions */

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_remove(
    mrfstr_t res, mrfstr_ct str, mrfstr_idx_t idx);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_n_remove(
    mrfstr_t res, mrfstr_ct str, mrfstr_idx_t idx, mrfstr_size_t count);

/* repeat functions */

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_repeat(
    mrfstr_t res, mrfstr_ct str, mrfstr_size_t count);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_repeat_chr(
    mrfstr_t res, mrfstr_chr_t chr, mrfstr_size_t count);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_fill(
    mrfstr_t res, mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_rfill(
    mrfstr_t res, mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

/* reverse functions */

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_reverse(
    mrfstr_t res, mrfstr_ct str);

/* replace functions */

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_replace_chr(
    mrfstr_t res, mrfstr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_n_replace_chr(
    mrfstr_t res, mrfstr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size);

/* cmp functions */

__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_equal(
    mrfstr_ct str1, mrfstr_ct str2);
__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_n_equal(
    mrfstr_ct str1, mrfstr_ct str2, mrfstr_size_t size);

#define mrfstr_equal_str(str1, str2) mrfstr_equal_nstr((str1), (str2), mrfstr_strlen(str2))
__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_n_equal_str(
    mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_equal_nstr(
    mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size2);
__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_n_equal_nstr(
    mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size2, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_equal_chr(
    mrfstr_ct str, mrfstr_chr_t chr);

/* contain functions */

__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_contain_chr(
    mrfstr_ct str, mrfstr_chr_t chr);
__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_n_contain_chr(
    mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_startswith(
    mrfstr_ct str, mrfstr_ct substr);
__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_startswith_str(
    mrfstr_ct str, mrfstr_data_ct substr);
__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_startswith_nstr(
    mrfstr_ct str, mrfstr_data_ct substr, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_endswith(
    mrfstr_ct str, mrfstr_ct substr);
__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_endswith_str(
    mrfstr_ct str, mrfstr_data_ct substr);
__MRFSTR_DECLSPEC mrfstr_bool_t mrfstr_endswith_nstr(
    mrfstr_ct str, mrfstr_data_ct substr, mrfstr_size_t size);

/* find functions */

__MRFSTR_DECLSPEC mrfstr_idx_t mrfstr_find_chr(
    mrfstr_ct str, mrfstr_chr_t chr);
__MRFSTR_DECLSPEC mrfstr_idx_t mrfstr_n_find_chr(
    mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

/* count functions */

__MRFSTR_DECLSPEC mrfstr_size_t mrfstr_count_chr(
    mrfstr_ct str, mrfstr_chr_t chr);
__MRFSTR_DECLSPEC mrfstr_size_t mrfstr_n_count_chr(
    mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t size);

/* data functions */

#ifdef __MRFSTR_INLINE
__MRFSTR_INLINE mrfstr_data_t mrfstr_get_data(
    mrfstr_ct str)
{
    return MRFSTR_DATA(str);
}

__MRFSTR_INLINE mrfstr_size_t mrfstr_get_size(
    mrfstr_ct str)
{
    return MRFSTR_SIZE(str);
}

__MRFSTR_INLINE mrfstr_size_t mrfstr_get_capa(
    mrfstr_ct str)
{
    return MRFSTR_CAPA(str);
}

__MRFSTR_INLINE void mrfstr_resize(
    mrfstr_t str, mrfstr_size_t size)
{
    if (size > MRFSTR_CAPA(str))
        size = MRFSTR_CAPA(str);

    MRFSTR_SIZE(str) = size;
}

__MRFSTR_INLINE mrfstr_res_t mrfstr_get_chr(
    mrfstr_chr_t *chr, mrfstr_ct str, mrfstr_size_t idx)
{
    if (idx >= MRFSTR_SIZE(str))
        return MRFSTR_RES_IDXOUT_ERROR;

    *chr = MRFSTR_DATA(str)[idx];
    return MRFSTR_RES_NOERROR;
}

__MRFSTR_INLINE mrfstr_res_t mrfstr_modify_chr(
    mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t idx)
{
    if (idx >= MRFSTR_SIZE(str))
        return MRFSTR_RES_IDXOUT_ERROR;

    MRFSTR_DATA(str)[idx] = chr;
    return MRFSTR_RES_NOERROR;
}
#else
__MRFSTR_DECLSPEC mrfstr_data_t mrfstr_get_data(
    mrfstr_ct str);
__MRFSTR_DECLSPEC mrfstr_size_t mrfstr_get_size(
    mrfstr_ct str);
__MRFSTR_DECLSPEC mrfstr_size_t mrfstr_get_capa(
    mrfstr_ct str);

__MRFSTR_DECLSPEC void mrfstr_resize(
    mrfstr_t str, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_get_chr(
    mrfstr_chr_t *chr, mrfstr_ct str, mrfstr_size_t idx);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_modify_chr(
    mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t idx);
#endif

/* io functions */

__MRFSTR_DECLSPEC void mrfstr_export(
    mrfstr_ct str);
__MRFSTR_DECLSPEC void mrfstr_n_export(
    mrfstr_ct str, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_import(
    mrfstr_t str);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_n_import(
    mrfstr_t str, mrfstr_size_t size);

#ifdef MRFSTR_HAVE_FILE
__MRFSTR_DECLSPEC void mrfstr_fexport(
    FILE *stream, mrfstr_ct str);
__MRFSTR_DECLSPEC void mrfstr_n_fexport(
    FILE *stream, mrfstr_ct str, mrfstr_size_t size);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_fimport(
    mrfstr_t str, FILE *stream);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_n_fimport(
    mrfstr_t str, FILE *stream, mrfstr_size_t size);
#endif

/* str functions */

__MRFSTR_DECLSPEC mrfstr_size_t mrfstr_strlen(
    mrfstr_data_ct str);

/* config functions */

enum __MRFSTR_CONFIG_LEVEL_ENUM
{
    MRFSTR_CONFIG_LEVEL_EXTREME,
    MRFSTR_CONFIG_LEVEL_EFFICIENT,
    MRFSTR_CONFIG_LEVEL_MODERATE,
    MRFSTR_CONFIG_LEVEL_LIMITED,
    MRFSTR_CONFIG_LEVEL_OFF
};
typedef mrfstr_ubyte_t mrfstr_config_level_t;

enum __MRFSTR_CONFIG_FUNC_ENUM
{
    MRFSTR_CONFIG_FUNC_MEMORY,
    MRFSTR_CONFIG_FUNC_REPLACE_CHR,
    MRFSTR_CONFIG_FUNC_REVERSE,

    MRFSTR_CONFIG_FUNC_COMPARE,
    MRFSTR_CONFIG_FUNC_SEARCH_CHR,

    MRFSTR_CONFIG_FUNC_STRLEN
};
typedef mrfstr_ubyte_t mrfstr_config_func_t;

enum __MRFSTR_CONFIG_DATA_ENUM
{
    MRFSTR_CONFIG_DATA_THREAD_COUNT,
    MRFSTR_CONFIG_DATA_STDALLOC,
    MRFSTR_CONFIG_DATA_THREAD_PRIORITY
};
typedef mrfstr_ubyte_t mrfstr_config_data_t; 

enum __MRFSTR_CONFIG_SIMD_ENUM
{
    MRFSTR_CONFIG_SIMD_AVX512,
    MRFSTR_CONFIG_SIMD_AVX,
    MRFSTR_CONFIG_SIMD_SSE,
    MRFSTR_CONFIG_SIMD_INT64
};
typedef mrfstr_ubyte_t mrfstr_config_simd_t;

enum __MRFSTR_CONFIG_PRIORITY_ENUM
{
    MRFSTR_CONFIG_PRIORITY_LOWEST,
    MRFSTR_CONFIG_PRIORITY_LOW,
    MRFSTR_CONFIG_PRIORITY_NORMAL,
    MRFSTR_CONFIG_PRIORITY_HIGH,
    MRFSTR_CONFIG_PRIORITY_HIGHEST
};
typedef mrfstr_ubyte_t mrfstr_config_priority_t;

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config(
    mrfstr_config_level_t type);

__MRFSTR_DECLSPEC void mrfstr_config_thread_count(
    mrfstr_ubyte_t tcount);
__MRFSTR_DECLSPEC void mrfstr_config_thread_count_max(
    mrfstr_bool_t use_logical);
__MRFSTR_DECLSPEC void mrfstr_config_stdalloc(
    mrfstr_ushort_t stdalloc);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_thread_priority(
    mrfstr_config_priority_t priority);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_get_ushort(
    mrfstr_ushort_t *value, mrfstr_config_data_t type);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_get_ubyte(
    mrfstr_ubyte_t *value, mrfstr_config_data_t type);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_get_byte(
    mrfstr_byte_t *value, mrfstr_config_data_t type);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_func(
    mrfstr_config_func_t type, mrfstr_config_simd_t single, mrfstr_config_simd_t multi);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_limits1(
    mrfstr_config_func_t type, mrfstr_size_t limit1);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_limits2(
    mrfstr_config_func_t type, mrfstr_size_t limit1, mrfstr_size_t limit2);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_limits3(
    mrfstr_config_func_t type, mrfstr_size_t limit1, mrfstr_size_t limit2, mrfstr_size_t limit3);

__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_limits1_get(
    mrfstr_config_func_t type, mrfstr_size_t *limit1);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_limits2_get(
    mrfstr_config_func_t type, mrfstr_size_t *limit1, mrfstr_size_t *limit2);
__MRFSTR_DECLSPEC mrfstr_res_t mrfstr_config_limits3_get(
    mrfstr_config_func_t type, mrfstr_size_t *limit1, mrfstr_size_t *limit2, mrfstr_size_t *limit3);

#ifdef __cplusplus
}
#endif

#endif
