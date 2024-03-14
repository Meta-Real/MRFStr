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
.set str, %rcx
.set ochr, %edx
.set nchr, %r8d
.set size, %r9

.set rochr, %rdx
.set rnchr, %r8

.set res2, %rcx
.set str2, %rdx
.set ochr2, %r8d
.set nchr2, %r9d
.set size2, %rax

.set rochr2, %r8
.set rnchr2, %r9
#else
.set str, %rsi
.set ochr, %edi
.set nchr, %ecx
.set size, %rdx

.set rochr, %rdi
.set rnchr, %rcx

.set res2, %rsi
.set str2, %rdi
.set ochr2, %ecx
.set nchr2, %edx
.set size2, %r8

.set rochr2, %rcx
.set rnchr2, %rdx
#endif

.text

# void __mrfstr_avx512bw_replchr(
#     mrfstr_ptr_t str,
#     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
#     mrfstr_size_t size)
#
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_avx512bw_replchr
__mrfstr_avx512bw_replchr:
    imul $0x01010101, ochr
    vpbroadcastd ochr, %zmm16
    imul $0x01010101, nchr
    vpbroadcastd nchr, %zmm17

LHEAD1:
    vpcmpb $0, %zmm16, (str, size), %k1
    vmovdqu8 %zmm17, (str, size){%k1}

    add $64, size
    jnz LHEAD1

    ret

# void __mrfstr_avx512bw_replchr2(
#     mrfstr_ptr_t res, mrfstr_ptr_ct str,
#     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
#     mrfstr_size_t size)
#
# res = RES+SIZE
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_avx512bw_replchr2
__mrfstr_avx512bw_replchr2:
    imul $0x01010101, ochr2
    vpbroadcastd ochr2, %zmm17
    imul $0x01010101, nchr2
    vpbroadcastd nchr2, %zmm18

#ifdef WIN32
    mov 40(%rsp), size2
#endif

LHEAD2:
    vmovdqu64 (str2, size2), %zmm16

    vpcmpb $0, %zmm17, %zmm16, %k1
    vpblendmb %zmm18, %zmm16, %zmm16{%k1}

    vmovdqa64 %zmm16, (res2, size2)

    add $64, size2
    jnz LHEAD2

    ret

# void __mrfstr_avx2_replchr(
#     mrfstr_ptr_t str,
#     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
#     mrfstr_size_t size)
#
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_avx2_replchr
__mrfstr_avx2_replchr:
    imul $0x01010101, ochr
    movd ochr, %xmm1
    pshufd $0, %xmm1, %xmm1
    vinsertf128 $1, %xmm1, %ymm1, %ymm1

    imul $0x01010101, nchr
    movd nchr, %xmm2
    pshufd $0, %xmm2, %xmm2
    vinsertf128 $1, %xmm2, %ymm2, %ymm2

LHEAD3:
    vmovdqa (str, size), %ymm0

    vpcmpeqb %ymm1, %ymm0, %ymm3
    vpblendvb %ymm3, %ymm2, %ymm0, %ymm0

    vmovdqa %ymm0, (str, size)

    add $32, size
    jnz LHEAD3

    vzeroupper
    ret

# void __mrfstr_avx2_replchr2(
#     mrfstr_ptr_t res, mrfstr_ptr_ct str,
#     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
#     mrfstr_size_t size)
#
# res = RES+SIZE
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_avx2_replchr2
__mrfstr_avx2_replchr2:
    imul $0x01010101, ochr2
    movd ochr2, %xmm1
    pshufd $0, %xmm1, %xmm1
    vinsertf128 $1, %xmm1, %ymm1, %ymm1

    imul $0x01010101, nchr2
    movd nchr2, %xmm2
    pshufd $0, %xmm2, %xmm2
    vinsertf128 $1, %xmm2, %ymm2, %ymm2

#ifdef WIN32
    mov 40(%rsp), size2
#endif

LHEAD4:
    vmovdqu (str2, size2), %ymm0

    vpcmpeqb %ymm1, %ymm0, %ymm3
    vpblendvb %ymm3, %ymm2, %ymm0, %ymm0

    vmovdqa %ymm0, (str2, size2)

    add $32, size2
    jnz LHEAD4

    vzeroupper
    ret

# void __mrfstr_sse4_1_replchr(
#     mrfstr_ptr_t str,
#     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
#     mrfstr_size_t size)
#
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_sse4_1_replchr
__mrfstr_sse4_1_replchr:
    imul $0x01010101, ochr
    movd ochr, %xmm2
    pshufd $0, %xmm2, %xmm2

    imul $0x01010101, nchr
    movd nchr, %xmm3
    pshufd $0, %xmm3, %xmm3

LHEAD5:
    movdqa (str, size), %xmm1
    movdqa %xmm1, %xmm0

    pcmpeqb %xmm2, %xmm0
    pblendvb %xmm0, %xmm3, %xmm1

    movdqa %xmm1, (str, size)

    add $16, size
    jnz LHEAD5

    ret

# void __mrfstr_sse4_1_replchr2(
#     mrfstr_ptr_t res, mrfstr_ptr_ct str,
#     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
#     mrfstr_size_t size)
#
# res = RES+SIZE
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_sse4_1_replchr2
__mrfstr_sse4_1_replchr2:
    imul $0x01010101, ochr2
    movd ochr2, %xmm2
    pshufd $0, %xmm2, %xmm2

    imul $0x01010101, nchr2
    movd nchr2, %xmm3
    pshufd $0, %xmm3, %xmm3

#ifdef WIN32
    mov 40(%rsp), size2
#endif

LHEAD6:
    movdqu (str2, size2), %xmm1
    movdqa %xmm1, %xmm0

    pcmpeqb %xmm2, %xmm0
    pblendvb %xmm0, %xmm3, %xmm1

    movdqa %xmm1, (res2, size2)

    add $16, size2
    jnz LHEAD6

    ret

# void __mrfstr_i64_replchr(
#     mrfstr_ptr_t str,
#     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
#     mrfstr_size_t size)
#
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_i64_replchr
__mrfstr_i64_replchr:
    push %rbx

    imul $0x01010101, ochr
    mov ochr, %r11d
    shl $32, %r11
    or rochr, %r11
    mov $0x7f7f7f7f7f7f7f7f, %rbx

LHEAD7:
    mov (str, size), %r10
    mov %r10, rochr

    xor %r11, rochr
    mov rochr, %rax

    and %rbx, %rax
    lea (%rax, %rbx), %rax
    or rochr, %rax
    or %rbx, %rax
    not %rax
    shr $7, %rax

    mov %rax, rochr
    imul $255, rochr
    not rochr
    and rochr, %r10

    imul rnchr, %rax
    or %rax, %r10
    mov %r10, (str, size)

    add $8, size
    jnz LHEAD7

    pop %rbx
    ret

# void __mrfstr_i64_replchr2(
#     mrfstr_ptr_t res, mrfstr_ptr_ct str,
#     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
#     mrfstr_size_t size)
#
# res = RES+SIZE
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_i64_replchr2
__mrfstr_i64_replchr2:
    push %rbx
    push %rbp

    imul $0x01010101, ochr2
    mov ochr2, %eax
    shl $32, rochr2
    or %rax, rochr2
    mov $0x7f7f7f7f7f7f7f7f, %rbx

#ifdef WIN32
    mov 56(%rsp), %rbp
#endif

LHEAD8:
    mov (str2, %rbp), %r10
    mov %r10, %r11

    xor rochr2, %r11
    mov %r11, %rax

    and %rbx, %rax
    lea (%rax, %rbx), %rax
    or %r11, %rax
    or %rbx, %rax
    not %rax
    shr $7, %rax

    mov %rax, %r11
    imul $255, %r11
    not %r11
    and %r11, %r10

    imul rnchr2, %rax
    or %rax, %r10
    mov %r10, (res2, %rbp)

    add $8, %rbp
    jnz LHEAD8

    pop %rbp
    pop %rbx
    ret
