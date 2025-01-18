# MRFStr version 1.3.0 documentation

<br></br>

```c
mrfstr_res_t mrfstr_set(mrfstr_t dst, mrfstr_ct src)
```

Copies the value of `src` to the `dst` and allocates space from memory if necessary. \
If the value of `dst` was allocated before (with enough size), it won't be reallocated.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate enough bytes for `dst`. \
In this case, it's not guaranteed that the `dst` stays the same.

**First version**: 1.0.0 \
**Last change**: 1.2.0 \
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_set` function

Version 1.2.0:

1. Return type changed from `void` to `mrfstr_res_t`

---

```c
#define mrfstr_set_str(dst, src)
```

Copies the value of `src` to the `dst` and allocates space from memory if necessary. The `src` must be null-terminated. \
If the value of `dst` was allocated before (with enough size), it won't be reallocated.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate enough bytes for `dst`. \
In this case, it's not guaranteed that the `dst` stays the same.

**First version**: 1.0.0 \
**Last change**: 1.3.0 \
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_set_str` function

Version 1.2.0:

1. Return type changed from `void` to `mrfstr_res_t`

version 1.3.0:

1. Changed the function to macro

---

```c
mrfstr_res_t mrfstr_set_nstr(mrfstr_t dst, mrfstr_data_ct src, mrfstr_size_t size)
```

Copies the value of `src` with its specified length to the `dst` and allocates space from memory if necessary. The `src` doesn't need to be null-terminated. \
If the value of `dst` was allocated before (with enough size), it won't be reallocated.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate enough bytes for `dst` value. \
In this case, it's not guaranteed that the `dst` stays the same.

**First version**: 1.0.0 \
**Last change**: 1.2.0 \
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_set_nstr` function

Version 1.2.0:

1. Return type changed from `void` to `mrfstr_res_t`

---

```c
mrfstr_res_t mrfstr_set_chr(mrfstr_t dst, mrfstr_chr_t src)
```

Copies the value of `src` to the `dst` and allocates 1 byte from memory if necessary. \
If the value of `dst` was allocated before, it won't be reallocated.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate 1 byte for `dst` value.

**First version**: 1.0.0 \
**Last change**: 1.2.0 \
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_set_chr` function

version 1.2.0:

1. Return type changed from `void` to `mrfstr_res_t`

---

```c
mrfstr_data_t mrfstr_get_str(mrfstr_data_t dst, mrfstr_ct src)
```

Returns a copied version of the value of `src`. The copied version is null-terminated. \
If the `dst` is not `NULL`, it is used as a pointer for the copied data. In this case, `dst` must be properly allocated. \
If the `dst` is `NULL`, then a new block is allocated and if the allocation process fails, the function returns a `NULL` pointer.

**First version**: 1.0.0 \
**Last change**: 1.0.0 \
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_get_str` function

---

```c
mrfstr_data_t mrfstr_n_get_str(mrfstr_data_t dst, mrfstr_ct src, mrfstr_size_t size)
```

Returns a copied version of the value of `src` up to `size` bytes. The copied version is null-terminated. \
If the `dst` is not `NULL`, it is used as a pointer for the copied data. In this case, `dst` must be properly allocated. \
If the `dst` is `NULL`, than a new block is allocated and if the allocation process fails, the function returns a `NULL` pointer.

**First version**: 1.0.0 \
**Last change**: 1.0.0 \
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_n_get_str` function
