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
#define left %rcx
#define right %rdx
#define size %r8
#else
#define left %rdi
#define right %rsi
#define size %rdx
#endif

.text
.align 64
_avx512vbmi:
    .quad 0x38393a3b3c3d3e3f, 0x3031323334353637
    .quad 0x28292a2b2c2d2e2f, 0x2021222324252627
_avx2_avx512vbmi:
    .quad 0x18191a1b1c1d1e1f, 0x1011121314151617
    .quad 0x08090a0b0c0d0e0f, 0x0001020304050607
_avx512bw1:
    .quad 0x08090a0b0c0d0e0f, 0x0001020304050607
    .quad 0x08090a0b0c0d0e0f, 0x0001020304050607
_avx2:
    .quad 0x08090a0b0c0d0e0f, 0x0001020304050607
_ssse3:
    .quad 0x08090a0b0c0d0e0f, 0x0001020304050607
_avx512bw2:
    .quad 6, 7, 4, 5, 2, 3, 0, 1

# void __mrfstr_avx512vbmi_rev(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_avx512vbmi_rev
__mrfstr_avx512vbmi_rev:
    mov size, %rax
    neg %rax
    sub $64, left
    vmovdqa64 _avx512vbmi(%rip), %zmm18

LHEAD1:
    vpermb (right, %rax), %zmm18, %zmm16
    vpermb (left, size), %zmm18, %zmm17

    vmovdqu64 %zmm17, (right, %rax)
    vmovdqa64 %zmm16, (left, size)

    add $64, %rax
    sub $64, size
    jnz LHEAD1

    ret

# void __mrfstr_avx512vbmi_rev2(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_avx512vbmi_rev2
__mrfstr_avx512vbmi_rev2:
    mov size, %rax
    neg %rax
    sub $64, left
    vmovdqa64 _avx512vbmi(%rip), %zmm17

LHEAD2:
    vpermb (right, %rax), %zmm17, %zmm16
    vmovdqa64 %zmm16, (left, size)

    add $64, %rax
    sub $64, size
    jnz LHEAD2

    ret

# void __mrfstr_avx512bw_rev(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_avx512bw_rev
__mrfstr_avx512bw_rev:
    mov size, %rax
    neg %rax
    sub $64, left
    vmovdqa64 _avx512bw1(%rip), %zmm18
    vmovdqa64 _avx512bw2(%rip), %zmm19

LHEAD3:
    vmovdqu64 (right, %rax), %zmm16
    vmovdqa64 (left, size), %zmm17

    vpshufb %zmm18, %zmm16, %zmm16
    vpshufb %zmm18, %zmm17, %zmm17
    vpermq %zmm16, %zmm19, %zmm16
    vpermq %zmm17, %zmm19, %zmm17

    vmovdqu64 %zmm17, (right, %rax)
    vmovdqa64 %zmm16, (left, size)

    add $64, %rax
    sub $64, size
    jnz LHEAD3

    ret

# void __mrfstr_avx512bw_rev2(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_avx512bw_rev2
__mrfstr_avx512bw_rev2:
    mov size, %rax
    neg %rax
    sub $64, left
    vmovdqa64 _avx512bw1(%rip), %zmm17
    vmovdqa64 _avx512bw2(%rip), %zmm18

LHEAD4:
    vmovdqu64 (right, %rax), %zmm16

    vpshufb %zmm17, %zmm16, %zmm16
    vpermq %zmm16, %zmm18, %zmm16

    vmovdqa64 %zmm16, (left, size)

    add $64, %rax
    sub $64, size
    jnz LHEAD4

    ret

# void __mrfstr_avx2_avx512vbmi_rev(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_avx2_avx512vbmi_rev
__mrfstr_avx2_avx512vbmi_rev:
    mov size, %rax
    neg %rax
    sub $32, left
    vmovdqa64 _avx2_avx512vbmi(%rip), %ymm18

LHEAD5:
    vpermb (right, %rax), %ymm18, %ymm16
    vpermb (left, size), %ymm18, %ymm17

    vmovdqu64 %ymm17, (right, %rax)
    vmovdqa64 %ymm16, (left, size)

    add $32, %rax
    sub $32, size
    jnz LHEAD5

    ret

# void __mrfstr_avx2_avx512vbmi_rev2(
#     mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_avx2_avx512vbmi_rev2
__mrfstr_avx2_avx512vbmi_rev2:
    mov size, %rax
    neg %rax
    sub $32, left
    vmovdqa64 _avx2_avx512vbmi(%rip), %ymm17

LHEAD6:
    vpermb (right, %rax), %ymm17, %ymm16
    vmovdqa64 %ymm16, (left, size)

    add $32, %rax
    sub $32, size
    jnz LHEAD6

    ret

# void __mrfstr_avx2_rev(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_avx2_rev
__mrfstr_avx2_rev:
    mov size, %rax
    neg %rax
    sub $32, left
    vmovdqa _avx2(%rip), %ymm2

LHEAD7:
    vmovdqu (right, %rax), %ymm0
    vmovdqa (left, size), %ymm1

    vpshufb %ymm2, %ymm0, %ymm0
    vpshufb %ymm2, %ymm1, %ymm1
    vperm2i128 $1, %ymm0, %ymm0, %ymm0
    vperm2i128 $1, %ymm1, %ymm1, %ymm1

    vmovdqu %ymm1, (right, %rax)
    vmovdqa %ymm0, (left, size)

    add $32, %rax
    sub $32, size
    jnz LHEAD7

    vzeroupper
    ret

# void __mrfstr_avx2_rev2(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_avx2_rev2
__mrfstr_avx2_rev2:
    mov size, %rax
    neg %rax
    sub $32, left
    vmovdqa _avx2(%rip), %ymm1

LHEAD8:
    vmovdqu (right, %rax), %ymm0

    vpshufb %ymm1, %ymm0, %ymm0
    vperm2i128 $1, %ymm0, %ymm0, %ymm0

    vmovdqa %ymm0, (left, size)

    add $32, %rax
    sub $32, size
    jnz LHEAD8

    vzeroupper
    ret

# void __mrfstr_ssse3_rev(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_ssse3_rev
__mrfstr_ssse3_rev:
    mov size, %rax
    neg %rax
    sub $16, left
    movdqa _ssse3(%rip), %xmm2

LHEAD9:
    movdqu (right, %rax), %xmm0
    movdqa (left, size), %xmm1

    pshufb %xmm2, %xmm0
    pshufb %xmm2, %xmm1

    movdqu %xmm1, (right, %rax)
    movdqa %xmm0, (left, size)

    add $16, %rax
    sub $16, size
    jnz LHEAD9

    ret

# void __mrfstr_ssse3_rev2(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_ssse3_rev2
__mrfstr_ssse3_rev2:
    mov size, %rax
    neg %rax
    sub $16, left
    movdqa _ssse3(%rip), %xmm1

LHEAD10:
    movdqu (right, %rax), %xmm0
    pshufb %xmm1, %xmm0
    movdqa %xmm0, (left, size)

    add $16, %rax
    sub $16, size
    jnz LHEAD10

    ret

# void __mrfstr_i64_rev(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_i64_rev
__mrfstr_i64_rev:
    mov size, %rax
    neg %rax
    sub $8, left

LHEAD11:
    mov (right, %rax), %r9
    mov (left, size), %r10

    bswap %r9
    bswap %r10

    mov %r10, (right, %rax)
    mov %r9, (left, size)

    add $8, %rax
    sub $8, size
    jnz LHEAD11

    ret

# void __mrfstr_i64_rev2(
#     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
#
# left = LEFT
# right = RIGHT
# size = SIZE

    .globl __mrfstr_i64_rev2
__mrfstr_i64_rev2:
    mov size, %rax
    neg %rax
    sub $8, left

LHEAD12:
    mov (right, %rax), %r9
    bswap %r9
    mov %r9, (left, size)

    add $8, %rax
    sub $8, size
    jnz LHEAD12

    ret
