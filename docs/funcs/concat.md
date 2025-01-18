# MRFStr version 1.3.0 documentation

<br></br>

```c
mrfstr_res_t mrfstr_concat(mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2)
```

Concatenates the `str1` and `str2` and puts the result inside `res`. \
The pointers of `res`, `str1`, and `str2` can be equal. \
If the value of `res` was allocated before (with enough size), it won't be reallocated.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate enough bytes for `res`. \
In this case, it's not guaranteed that the `res` stays the same unless its either the `str1` or the `str2`.
2. `MRFSTR_RES_OVERFLOW_ERROR`: Size of the result of concatenation exceeds the limit (`MRFSTR_SIZEMAX`).

**First version**: 1.0.0 \
**Last change**: 1.0.0 \
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_concat` function

---

```c
mrfstr_res_t mrfstr_n_concat(mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2, mrfstr_size_t size)
```

Concatenates the `str1` and `size` bytes of the `str2` and puts the result inside `res`. \
The pointers of `res`, `str1`, and `str2` can be equal. \
If the value of `res` was allocated before (with enough size), it won't be reallocated.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate enough bytes for `res`. \
In this case, it's not guaranteed that the `res` stays the same unless its either the `str1` or the `str2`.
2. `MRFSTR_RES_OVERFLOW_ERROR`: Size of the result of concatenation exceeds the limit (`MRFSTR_SIZEMAX`).

**First version**: 1.0.0 \
**Last change**: 1.0.0 \
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_n_concat` function

---

```c
#define mrfstr_concat_str(res, str1, str2)
```

Concatenates the `str1` and `str2` and puts the result inside `res`. The `str2` must be null-terminated. \
The pointers of `res` and `str1` can be equal. \
If the value of `res` was allocated before (with enough size), it won't be reallocated.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate enough bytes for `res`. \
In this case, it's not guaranteed that the `res` stays the same unless its the `str1`.
2. `MRFSTR_RES_OVERFLOW_ERROR`: Size of the result of concatenation exceeds the limit (`MRFSTR_SIZEMAX`).

**First version**: 1.3.0 \
**Last change**: 1.3.0 \
**Changelog**:

Version 1.3.0:

1. Added the `mrfstr_concat_str` macro

---

```c
mrfstr_res_t mrfstr_concat_nstr(mrfstr_t res, mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size)
```

Concatenates the `str1` and `size` bytes of the `str2` and puts the result inside `res`. The `str2` doesn't need to be null-terminated. \
The pointers of `res` and `str1` can be equal. \
If the value of `res` was allocated before (with enough size), it won't be reallocated.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate enough bytes for `res`. \
In this case, it's not guaranteed that the `res` stays the same unless its the `str1`.
2. `MRFSTR_RES_OVERFLOW_ERROR`: Size of the result of concatenation exceeds the limit (`MRFSTR_SIZEMAX`).

**First version**: 1.3.0 \
**Last change**: 1.3.0 \
**Changelog**:

Version 1.3.0:

1. Added the `mrfstr_concat_nstr` function

---

```c
mrfstr_res_t mrfstr_concat_chr(mrfstr_t res, mrfstr_ct str, mrfstr_chr_t chr)
```

**First version**: 1.3.0 \
**Last change**: 1.3.0 \
**Changelog**:

Concatenates the `str` and `chr` and puts the result insde `res`. \
The pointers of `res` and `str` can be equal. \
If the value of `res` was allocated before (with enough size), it won't be reallocated.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate enough bytes for `res`. \
In this case, it's not guaranteed that the `res` stays the same unless its the `str1`.
2. `MRFSTR_RES_OVERFLOW_ERROR`: Size of the result of concatenation exceeds the limit (`MRFSTR_SIZEMAX`).

Version 1.3.0:

1. Added the `mrfstr_concat_chr` function

---

```c
mrfstr_res_t mrfstr_insert(mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2, mrfstr_idx_t idx)
```

**First version**: 1.2.0 \
**Last change**: 1.2.0 \
**Changelog**:

Version 1.2.0:

1. Added the `mrfstr_insert` function

---

```c
mrfstr_res_t mrfstr_n_insert(mrfstr_t res, mrfstr_ct str1, mrfstr_ct str2, mrfstr_idx_t idx, mrfstr_size_t size)
```

**First version**: 1.3.0 \
**Last change**: 1.3.0 \
**Changelog**:

Version 1.3.0:

1. Added the `mrfstr_n_insert` function

---

```c
#define mrfstr_insert_str(res, str1, str2, idx)
```

**First version**: 1.3.0 \
**Last change**: 1.3.0 \
**Changelog**:

Version 1.3.0:

1. Added the `mrfstr_insert_str` macro

---

```c
mrfstr_res_t mrfstr_insert_nstr(mrfstr_t res, mrfstr_ct str1, mrfstr_data_ct str2, mrfstr_size_t size2, mrfstr_idx_t idx)
```

**First version**: 1.3.0 \
**Last change**: 1.3.0 \
**Changelog**:

Version 1.3.0:

1. Added the `mrfstr_insert_nstr` function

---

```c
mrfstr_res_t mrfstr_insert_chr(mrfstr_t res, mrfstr_ct str, mrfstr_chr_t chr, mrfstr_idx_t idx)
```

**First version**: 1.3.0 \
**Last change**: 1.3.0 \
**Changelog**:

Version 1.3.0:

1. Added the `mrfstr_insert_nstr` function
