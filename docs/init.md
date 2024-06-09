# MRFStr version 1.2.0 documentation

```c
void mrfstr_init(mrfstr_t str)
```

Initializes the `str` structure with `NULL` value. \
This function (or other init functions) must be called before any usage of the structure.

**First version**: 1.0.0
**Last change**: 1.3.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_init` function

Version 1.3.0:

1. Return type changed from `mrfstr_t` to `void`
2. Arguments changed from `void` to `mrfstr_t`

---

```c
void mrfstr_inits(mrfstr_p str, ...)
```

Initializes a list of `mrfstr_t` structures with `NULL` value. The list must end with a `NULL` argument. \
This function (or other init functions) must be called before any usage of the structure(s).

**Example**:

```c
mrfstr_t str1, str2, str3;
mrfstr_inits(str1, str2, str3, NULL);
```

**First version**: 1.3.0
**Last change**: 1.3.0
**Changelog**:

Version 1.3.0:

1. Added the `mrfstr_inits` function

---

```c
#define mrfstr_init_str(str, data)
```

Initializes the `str` structure with a given data. The data must be null-terminated. \
This function (or other init functions) must be called before any usage of the structure.

**First version**: 1.0.0
**Last change**: 1.3.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_init2` function

Version 1.3.0:

1. Changed the function to macro
2. Changed the function name from `mrfstr_init2` to `mrfstr_init_str`

---

```c
void mrfstr_init_nstr(mrfstr_t str, mrfstr_data_t data, mrfstr_size_t size)
```

Initializes the `str` structure with a given data and its length in bytes. The data doesn't need to be null-terminated. \
This function (or other init functions) must be called before any usage of the structure.

**First version**: 1.0.0
**Last change**: 1.3.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_init3` function

Version 1.3.0:

1. Changed the function name from `mrfstr_init3` to `mrfstr_init_nstr`

---

```c
mrfstr_res_t mrfstr_init_alloc(mrfstr_t str, mrfstr_size_t size)
```

Initializes the `str` structure with empty value and allocates `size` bytes from memory for it. \
This function only serves for performance purposes since the MRFStr library handles memory allocation internally. \
This function (or other init functions) must be called before any usage of the structure.

**First version**: 1.3.0
**Last change**: 1.3.0
**Changelog**:

Version 1.3.0:

1. Added the `mrfstr_init_alloc` function

---

```c
mrfstr_res_t mrfstr_alloc(mrfstr_t str,  mrfstr_size_t size)
```

Allocates `size` bytes from memory for the `str`. The value of `str` should be empty or it will lead to memory leakage. \
This function only serves for performance purposes since the MRFStr library handles memory allocation internally. \

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate `size` bytes from memory.

**First version**: 1.0.0
**Last change**: 1.0.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_alloc` function

---

```c
void mrfstr_free(mrfstr_t str)
```

Frees the `str` from memory. If this function is called, the `str` must be initialized again for reuse.

**First version**: 1.0.0
**Last change**: 1.0.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_free` function

---

```c
void mrfstr_frees(mrfstr_p str, ...)
```

Frees a list of `mrfstr_t` structures from memory. If this function is called, the `mrfstr_t` structure(s) must be initialized again for reuse. \

**Example**:

```c
mrfstr_t str1, str2, str3;
mrfstr_inits(str1, str2, str3, NULL);

...

mrfstr_frees(str1, str2, str3, NULL);
```

**First version**: 1.3.0
**Last change**: 1.3.0
**Changelog**:

Version 1.3.0:

1. Added the `mrfstr_frees` function

---

```c
void mrfstr_clear(mrfstr_t str)
```

Clears value of the `str` structure but unlike `mrfstr_free` function, the structure can be used again.

**First version**: 1.0.0
**Last change**: 1.0.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_clear` function

---

```c
mrfstr_res_t mrfstr_realloc(mrfstr_t str, mrfstr_size_t size)
```

Reallocates value of the `str` structure and allocates `size` bytes and if value of `str` is empty, it allocates new space for it. Its guaranteed that the old value (first `size` bytes of it) of `str` stays the same. \
This function only serves for performance purposes since the MRFStr library handles memory allocation internally.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate `size` bytes from memory. \
In this case, the value of `str` stays the same.

**First version**: 1.0.0
**Last change**: 1.0.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_realloc` function

---

```c
mrfstr_res_t mrfstr_clear_realloc(mrfstr_t str, mrfstr_size_t size)
```

Clears value of the `str` structure and allocates `size` bytes and if value of `str` is empty, it allocates new space for it. Unlike the `mrfstr_realloc` function, Its not guaranteed that the old value of `str` stays the same. \
This function is faster than `mrfstr_realloc` in certain scenarios and is preferred when the old value is no longer needed. \
This function only serves for performance purposes since the MRFStr library handles memory allocation internally.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate `size` bytes from memory. \
In this case, it's not guaranteed that the value of `str` stays the same.

**First version**: 1.0.0
**Last change**: 1.0.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_clear_realloc` function

---

```c
mrfstr_res_t mrfstr_expand(mrfstr_t str, mrfstr_size_t size)
```

Expands size of the `str` structure and allocates additional space and if value of `str` is empty, it allocates new space for it. The `size` must be larger than size of `str` value. Its guaranteed that the old value of `str` stays the same. \
This function only serves for performance purposes since the MRFStr library handles memory allocation internally.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate `size` bytes from memory. \
In this case, the value of `str` stays the same.

**First version**: 1.0.0
**Last change**: 1.0.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_expand` function

---

```c
mrfstr_res_t mrfstr_clear_expand(mrfstr_t str, mrfstr_size_t size)
```

Expands size of the `str` structure and allocates additional space and if value of `str` is empty, it allocates new space for it. The `size` must be larger than size of `str` value. Unlike the `mrfstr_expand` function, Its not guaranteed that the old value of `str` stays the same. \
This function is faster than `mrfstr_expand` in certain scenarios and is preferred when the old value is no longer needed. \
This function only serves for performance purposes since the MRFStr library handles memory allocation internally.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate `size` bytes from memory. \
In this case, it's not guaranteed that the value of `str` stays the same.

**First version**: 1.0.0
**Last change**: 1.0.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_clear_expand` function

---

```c
mrfstr_res_t mrfstr_shrink(mrfstr_t str, mrfstr_size_t size)
```

Shrinks size of the `str` structure. The `size` must be smaller than size of `str` value. Its guaranteed that the old value (first `size` bytes of it) of `str` stays the same. \
This function only serves for performance purposes since the MRFStr library handles memory allocation internally.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate `size` bytes from memory. \
In this case, the value of `str` (all of it) stays the same.

**First version**: 1.0.0
**Last change**: 1.0.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_shrink` function

---

```c
mrfstr_res_t mrfstr_clear_shrink(mrfstr_t str, mrfstr_size_t size)
```

Shrinks size of the `str` structure. The `size` must be smaller than size of `str` value. Unlike the `mrfstr_shrink` function, Its not guaranteed that the old value of `str` stays the same. \
This function is slower than `mrfstr_shrink` and is preferred when the old value must be deleted. \
This function only serves for performance purposes since the MRFStr library handles memory allocation internally.

**Error codes**:

1. `MRFSTR_RES_MEM_ERROR`: The function failed to allocate `size` bytes from memory. \
In this case, it's not guaranteed that the value of `str` stays the same.

**First version**: 1.0.0
**Last change**: 1.0.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_clear_shrink` function

---

```c
void mrfstr_swap(mrfstr_t str1, mrfstr_t str2)
```

Swaps all of the data of `str1` and `str2`.

**First version**: 1.0.0
**Last change**: 1.0.0
**Changelog**:

Version 1.0.0:

1. Added the `mrfstr_swap` function
