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

#ifdef WIN32
#define dst %rcx
#define src %rdx
#define size %r8
#else
#define dst %rdi
#define src %rsi
#define size %rdx
#endif

    .extern _mrfstr_mem_ntlimit
    .extern _mrfstr_mem_ntrlimit

.text

# void __mrfstr_avx512f_copy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = -SIZE

    .globl __mrfstr_avx512f_copy
__mrfstr_avx512f_copy:
    cmp _mrfstr_mem_ntlimit(%rip), size
    jb NTLHEAD1

LHEAD1:
    vmovdqu64 (src, size), %zmm16
    vmovdqa64 %zmm16, (dst, size)

    add $64, size
    jnz LHEAD1

    ret

NTLHEAD1:
    vmovdqu64 (src, size), %zmm16
    vmovntdq %zmm16, (dst, size)

    add $64, size
    jnz NTLHEAD1

    sfence
    ret

# void __mrfstr_avx512f_vcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = -SIZE

    .globl __mrfstr_avx512f_vcopy
__mrfstr_avx512f_vcopy:
LHEAD2:
    vmovdqu64 (src, size), %zmm16
    vmovdqa64 %zmm16, (dst, size)

    add $64, size
    jnz LHEAD2

    ret

# void __mrfstr_avx512f_ntcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = -SIZE

    .globl __mrfstr_avx512f_ntcopy
__mrfstr_avx512f_ntcopy:
LHEAD3:
    vmovdqu64 (src, size), %zmm16
    vmovntdq %zmm16, (dst, size)

    add $64, size
    jnz LHEAD3

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

    cmp _mrfstr_mem_ntrlimit(%rip), size
    ja NTLHEAD4

LHEAD4:
    vmovdqu64 (src, size), %zmm16
    vmovdqa64 %zmm16, (dst, size)

    sub $64, size
    jnz LHEAD4

    ret

NTLHEAD4:
    vmovdqu64 (src, size), %zmm16
    vmovntdq %zmm16, (dst, size)

    sub $64, size
    jnz NTLHEAD4

    sfence
    ret

# void __mrfstr_avx512f_vrcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST
# src = SRC
# size = SIZE

    .globl __mrfstr_avx512f_vrcopy
__mrfstr_avx512f_vrcopy:
    sub $64, dst
    sub $64, src

LHEAD5:
    vmovdqu64 (src, size), %zmm16
    vmovdqa64 %zmm16, (dst, size)

    sub $64, size
    jnz LHEAD5

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

LHEAD6:
    vmovdqu64 (src, size), %zmm16
    vmovntdq %zmm16, (dst, size)

    sub $64, size
    jnz LHEAD6

    sfence
    ret

# void __mrfstr_avx_copy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = -SIZE

    .globl __mrfstr_avx_copy
__mrfstr_avx_copy:
    cmp _mrfstr_mem_ntlimit(%rip), size
    jb NTLHEAD7

LHEAD7:
    vmovdqu (src, size), %ymm0
    vmovdqa %ymm0, (dst, size)

    add $32, size
    jnz LHEAD7

    vzeroupper
    ret

NTLHEAD7:
    vmovdqu (src, size), %ymm0
    vmovntdq %ymm0, (dst, size)

    add $32, size
    jnz NTLHEAD7

    sfence
    vzeroupper
    ret

# void __mrfstr_avx_vcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = -SIZE

    .globl __mrfstr_avx_vcopy
__mrfstr_avx_vcopy:
LHEAD8:
    vmovdqu (src, size), %ymm0
    vmovdqa %ymm0, (dst, size)

    add $32, size
    jnz LHEAD8

    vzeroupper
    ret

# void __mrfstr_avx_ntcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = -SIZE

    .globl __mrfstr_avx_ntcopy
__mrfstr_avx_ntcopy:
LHEAD9:
    vmovdqu (src, size), %ymm0
    vmovntdq %ymm0, (dst, size)

    add $32, size
    jnz LHEAD9

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

    cmp _mrfstr_mem_ntrlimit(%rip), size
    ja NTLHEAD10

LHEAD10:
    vmovdqu (src, size), %ymm0
    vmovdqa %ymm0, (dst, size)

    sub $32, size
    jnz LHEAD10

    vzeroupper
    ret

NTLHEAD10:
    vmovdqu (src, size), %ymm0
    vmovntdq %ymm0, (dst, size)

    sub $32, size
    jnz NTLHEAD10

    sfence
    vzeroupper
    ret

# void __mrfstr_avx_vrcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST
# src = SRC
# size = SIZE

    .globl __mrfstr_avx_vrcopy
__mrfstr_avx_vrcopy:
    sub $32, dst
    sub $32, src

LHEAD11:
    vmovdqu (src, size), %ymm0
    vmovdqa %ymm0, (dst, size)

    sub $32, size
    jnz LHEAD11

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

LHEAD12:
    vmovdqu (src, size), %ymm0
    vmovntdq %ymm0, (dst, size)

    sub $32, size
    jnz LHEAD12

    sfence
    vzeroupper
    ret

# void __mrfstr_sse2_copy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = -SIZE

    .globl __mrfstr_sse2_copy
__mrfstr_sse2_copy:
    cmp _mrfstr_mem_ntlimit(%rip), size
    jb NTLHEAD13

LHEAD13:
    movdqu (src, size), %xmm0
    movdqa %xmm0, (dst, size)

    add $16, size
    jnz LHEAD13

    ret

NTLHEAD13:
    movdqu (src, size), %xmm0
    movntdq %xmm0, (dst, size)

    add $16, size
    jnz NTLHEAD13

    sfence
    ret

# void __mrfstr_sse2_vcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = -SIZE

    .globl __mrfstr_sse2_vcopy
__mrfstr_sse2_vcopy:
LHEAD14:
    movdqu (src, size), %xmm0
    movdqa %xmm0, (dst, size)

    add $16, size
    jnz LHEAD14

    ret

# void __mrfstr_sse2_ntcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = -SIZE

    .globl __mrfstr_sse2_ntcopy
__mrfstr_sse2_ntcopy:
LHEAD15:
    movdqu (src, size), %xmm0
    movntdq %xmm0, (dst, size)

    add $16, size
    jnz LHEAD15

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

    cmp _mrfstr_mem_ntrlimit(%rip), size
    ja NTLHEAD16

LHEAD16:
    movdqu (src, size), %xmm0
    movdqa %xmm0, (dst, size)

    sub $16, size
    jnz LHEAD16

    ret

NTLHEAD16:
    movdqu (src, size), %xmm0
    movntdq %xmm0, (dst, size)

    sub $16, size
    jnz NTLHEAD16

    sfence
    ret

# void __mrfstr_sse2_vrcopy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST
# src = SRC
# size = SIZE

    .globl __mrfstr_sse2_vrcopy
__mrfstr_sse2_vrcopy:
    sub $16, dst
    sub $16, src

LHEAD17:
    movdqu (src, size), %xmm0
    movdqa %xmm0, (dst, size)

    sub $16, size
    jnz LHEAD17

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

LHEAD18:
    movdqu (src, size), %xmm0
    movntdq %xmm0, (dst, size)

    sub $16, size
    jnz LHEAD18

    sfence
    ret

# void __mrfstr_i64_copy(
#     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
#
# dst = DST+SIZE
# src = SRC+SIZE
# size = -SIZE

    .globl __mrfstr_i64_copy
__mrfstr_i64_copy:
LHEAD19:
    mov (src, size), %rax
    mov %rax, (dst, size)

    add $8, size
    jnz LHEAD19

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

LHEAD20:
    mov (src, size), %rax
    mov %rax, (dst, size)

    add $8, size
    jnz LHEAD20

    ret
