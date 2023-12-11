/*
    MRFStr Library version 1.0.0
    MetaReal Fast String Library
*/

#ifndef __MRFSTR__
#define __MRFSTR__

typedef char mrfstr_chr_t;
typedef mrfstr_chr_t *mrfstr_data_t;
typedef const mrfstr_chr_t *mrfstr_data_ct;

typedef unsigned long long mrfstr_size_t;
typedef unsigned char mrfstr_bit_t;
typedef unsigned char mrfstr_bool_t;

#define MRFSTR_TRUE 1
#define MRFSTR_FALSE 0

#define MRFSTR_THREADING 1

#if MRFSTR_THREADING
#define MRFSTR_THREAD_COUNT 6
#endif

struct __MRFSTR_T
{
    mrfstr_data_t data;
    mrfstr_size_t size;
    mrfstr_size_t alloc;
};
typedef struct __MRFSTR_T *mrfstr_t;
typedef const mrfstr_t mrfstr_ct;

enum __MRFSTR_RES_ENUM
{
    MRFSTR_RES_NOERROR,
    MRFSTR_RES_MEM_ERROR
};
typedef unsigned char mrfstr_res_enum_t;

/* init functions */

mrfstr_t mrfstr_init();
mrfstr_t mrfstr_init2(mrfstr_data_t data);
mrfstr_t mrfstr_init3(mrfstr_data_t data, mrfstr_size_t size);

mrfstr_res_enum_t mrfstr_alloc(mrfstr_t str,  mrfstr_size_t size);
void mrfstr_free(mrfstr_t str);
void mrfstr_clear(mrfstr_t str);

mrfstr_res_enum_t mrfstr_realloc(mrfstr_t str, mrfstr_size_t size);
mrfstr_res_enum_t mrfstr_clear_realloc(mrfstr_t str, mrfstr_size_t size);

mrfstr_res_enum_t mrfstr_expand(mrfstr_t str, mrfstr_size_t size);
mrfstr_res_enum_t mrfstr_clear_expand(mrfstr_t str, mrfstr_size_t size);
mrfstr_res_enum_t mrfstr_expand_add(mrfstr_t str, mrfstr_size_t add);
mrfstr_res_enum_t mrfstr_clear_expand_add(mrfstr_t str, mrfstr_size_t add);

mrfstr_res_enum_t mrfstr_shrink(mrfstr_t str, mrfstr_size_t size);
mrfstr_res_enum_t mrfstr_clear_shrink(mrfstr_t str, mrfstr_size_t size);
mrfstr_res_enum_t mrfstr_shrink_sub(mrfstr_t str, mrfstr_size_t sub);
mrfstr_res_enum_t mrfstr_clear_shrink_sub(mrfstr_t str, mrfstr_size_t sub);

void mrfstr_swap(mrfstr_t str1, mrfstr_t str2);

/* set functions */

mrfstr_res_enum_t mrfstr_set(mrfstr_t dst, mrfstr_ct src);

mrfstr_res_enum_t mrfstr_set_str(mrfstr_t dst, mrfstr_data_ct src);
mrfstr_res_enum_t mrfstr_set_nstr(mrfstr_t dst, mrfstr_data_ct src, mrfstr_size_t size);

mrfstr_res_enum_t mrfstr_set_chr(mrfstr_t dst, mrfstr_chr_t src);

/* concat functions */

mrfstr_res_enum_t mrfstr_concat(mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2);

/* repeat functions */

mrfstr_res_enum_t mrfstr_repeat(mrfstr_t res, mrfstr_ct str, mrfstr_size_t count);

mrfstr_res_enum_t mrfstr_repeat_chr(mrfstr_t res, mrfstr_chr_t chr, mrfstr_size_t count);

/* cmp functions */

mrfstr_bool_t mrfstr_equal(mrfstr_ct str1, mrfstr_ct str2);

mrfstr_bool_t mrfstr_equal_str(mrfstr_ct str1, mrfstr_data_ct str2);
mrfstr_bool_t mrfstr_equal_nstr(mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size);

/* contain functions */

mrfstr_bool_t mrfstr_contain_chr(mrfstr_ct str, mrfstr_chr_t chr);

/* reverse functions */

mrfstr_res_enum_t mrfstr_reverse(mrfstr_t res, mrfstr_ct str);

/* util functions */

mrfstr_data_t mrfstr_get_data(mrfstr_ct str);
mrfstr_size_t mrfstr_get_size(mrfstr_ct str);

mrfstr_chr_t mrfstr_get_chr(mrfstr_ct str, mrfstr_size_t idx);
void mrfstr_modify_chr(mrfstr_ct str, mrfstr_chr_t chr, mrfstr_size_t idx);

void mrfstr_print(mrfstr_ct str);

#endif /* __MRFSTR__ */
