# MIT License
#
# Copyright (c) 2023 MetaReal
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

    .extern _mrfstr_mem_ntlimit

#ifdef WIN32
.set dst, %rcx
.set src, %rdx
.set size, %r8
#else
.set dst, %rsi
.set src, %rdi
.set size, %rcx
#endif

.text

# void __mrfstr_avx512f_copy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = SIZE

    .globl __mrfstr_avx512f_copy
__mrfstr_avx512f_copy:
    cmp _mrfstr_mem_ntlimit(%rip), size
    ja NTSTORE1

    neg size

LHEAD1:
    vmovdqu64 (src, size), %zmm16
    vmovdqa64 %zmm16, (dst, size)

    add $64, size
    jnz LHEAD1

    ret

NTSTORE1:
    neg size

NTLHEAD1:
    vmovdqu64 (src, size), %zmm16
    vmovntdq %zmm16, (dst, size)

    add $64, size
    jnz NTLHEAD1

    sfence
    ret

# void __mrfstr_avx512f_ntcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = SIZE

    .globl __mrfstr_avx512f_ntcopy
__mrfstr_avx512f_ntcopy:
    neg size

LHEAD2:
    vmovdqu64 (src, size), %zmm16
    vmovntdq %zmm16, (dst, size)

    add $64, size
    jnz LHEAD2

    sfence
    ret

# void __mrfstr_avx512f_rcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST
# src = SRC
# size = SIZE

    .globl __mrfstr_avx512f_rcopy
__mrfstr_avx512f_rcopy:
    sub $64, dst
    sub $64, src

    cmp _mrfstr_mem_ntlimit(%rip), size
    ja NTLHEAD2

LHEAD3:
    vmovdqu64 (src, size), %zmm16
    vmovdqa64 %zmm16, (dst, size)

    sub $64, size
    jnz LHEAD3

    ret

NTLHEAD2:
    vmovdqu64 (src, size), %zmm16
    vmovntdq %zmm16, (dst, size)

    sub $64, size
    jnz NTLHEAD2

    sfence
    ret

# void __mrfstr_avx512f_ntrcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST
# src = SRC
# size = SIZE

    .globl __mrfstr_avx512f_ntrcopy
__mrfstr_avx512f_ntrcopy:
    sub $64, dst
    sub $64, src

LHEAD4:
    vmovdqu64 (src, size), %zmm16
    vmovntdq %zmm16, (dst, size)

    sub $64, size
    jnz LHEAD4

    sfence
    ret

# void __mrfstr_avx_copy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = SIZE

    .globl __mrfstr_avx_copy
__mrfstr_avx_copy:
    cmp _mrfstr_mem_ntlimit(%rip), size
    ja NTSTORE2

    neg size

LHEAD5:
    vmovdqu (src, size), %ymm0
    vmovdqa %ymm0, (dst, size)

    add $32, size
    jnz LHEAD5

    vzeroupper
    ret

NTSTORE2:
    neg size

NTLHEAD3:
    vmovdqu (src, size), %ymm0
    vmovntdq %ymm0, (dst, size)

    add $32, size
    jnz NTLHEAD3

    sfence
    vzeroupper
    ret

# void __mrfstr_avx_ntcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = SIZE

    .globl __mrfstr_avx_ntcopy
__mrfstr_avx_ntcopy:
    neg size

LHEAD6:
    vmovdqu (src, size), %ymm0
    vmovntdq %ymm0, (dst, size)

    add $32, size
    jnz LHEAD6

    sfence
    vzeroupper
    ret

# void __mrfstr_avx_rcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST
# src = SRC
# size = SIZE

    .globl __mrfstr_avx_rcopy
__mrfstr_avx_rcopy:
    sub $32, dst
    sub $32, src

    cmp _mrfstr_mem_ntlimit(%rip), size
    ja NTLHEAD4

LHEAD7:
    vmovdqu (src, size), %ymm0
    vmovdqa %ymm0, (dst, size)

    sub $32, size
    jnz LHEAD7

    vzeroupper
    ret

NTLHEAD4:
    vmovdqu (src, size), %ymm0
    vmovntdq %ymm0, (dst, size)

    sub $32, size
    jnz NTLHEAD4

    sfence
    vzeroupper
    ret

# void __mrfstr_avx_ntrcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST
# src = SRC
# size = SIZE

    .globl __mrfstr_avx_ntrcopy
__mrfstr_avx_ntrcopy:
    sub $32, dst
    sub $32, src

LHEAD8:
    vmovdqu (src, size), %ymm0
    vmovntdq %ymm0, (dst, size)

    sub $32, size
    jnz LHEAD8

    sfence
    vzeroupper
    ret

# void __mrfstr_sse2_copy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = SIZE

    .globl __mrfstr_sse2_copy
__mrfstr_sse2_copy:
    cmp _mrfstr_mem_ntlimit(%rip), size
    ja NTSTORE3

    neg size

LHEAD9:
    movdqu (src, size), %xmm0
    movdqa %xmm0, (dst, size)

    add $16, size
    jnz LHEAD9

    ret

NTSTORE3:
    neg size

NTLHEAD5:
    movdqu (src, size), %xmm0
    movntdq %xmm0, (dst, size)

    add $16, size
    jnz NTLHEAD5

    sfence
    ret

# void __mrfstr_sse2_ntcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = SIZE

    .globl __mrfstr_sse2_ntcopy
__mrfstr_sse2_ntcopy:
    neg size

LHEAD10:
    movdqu (src, size), %xmm0
    movntdq %xmm0, (dst, size)

    add $16, size
    jnz LHEAD10

    sfence
    ret

# void __mrfstr_sse2_rcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST
# src = SRC
# size = SIZE

    .globl __mrfstr_sse2_rcopy
__mrfstr_sse2_rcopy:
    sub $16, dst
    sub $16, src

    cmp _mrfstr_mem_ntlimit(%rip), size
    ja NTLHEAD6

LHEAD11:
    movdqu (src, size), %xmm0
    movdqa %xmm0, (dst, size)

    sub $16, size
    jnz LHEAD11

    ret

NTLHEAD6:
    movdqu (src, size), %xmm0
    movntdq %xmm0, (dst, size)

    sub $16, size
    jnz NTLHEAD6

    sfence
    ret

# void __mrfstr_sse2_ntrcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST
# src = SRC
# size = SIZE

    .globl __mrfstr_sse2_ntrcopy
__mrfstr_sse2_ntrcopy:
    sub $16, dst
    sub $16, src

LHEAD12:
    movdqu (src, size), %xmm0
    movntdq %xmm0, (dst, size)

    sub $16, size
    jnz LHEAD12

    sfence
    ret

# void __mrfstr_i64_copy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = SIZE

    .globl __mrfstr_i64_copy
__mrfstr_i64_copy:
    neg size

LHEAD13:
    mov (src, size), %rax
    mov %rax, (dst, size)

    add $8, size
    jnz LHEAD13

    ret

# void __mrfstr_i64_rcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST
# src = SRC
# size = SIZE

    .globl __mrfstr_i64_rcopy
__mrfstr_i64_rcopy:
    sub $8, dst
    sub $8, src

LHEAD14:
    mov (src, size), %rax
    mov %rax, (dst, size)

    add $8, size
    jnz LHEAD14

    ret
