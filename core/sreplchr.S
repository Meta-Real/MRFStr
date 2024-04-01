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
#define str %rcx
#define ochr %edx
#define nchr %r8d
#define size %r9

#define rochr %rdx
#define rnchr %r8

#define res2 %rcx
#define str2 %rdx
#define ochr2 %r8d
#define nchr2 %r9d
#define size2 %rax

#define rochr2 %r8
#define rnchr2 %r9
#else
#define str %rsi
#define ochr %edi
#define nchr %ecx
#define size %rdx

#define rochr %rdi
#define rnchr %rcx

#define res2 %rsi
#define str2 %rdi
#define ochr2 %ecx
#define nchr2 %edx
#define size2 %r8

#define rochr2 %rcx
#define rnchr2 %rdx
#endif

.text

# void __mrfstr_avx512bw_replchr(
#     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_avx512bw_replchr
__mrfstr_avx512bw_replchr:
    vpbroadcastb ochr, %zmm16
    vpbroadcastb nchr, %zmm17

LHEAD1:
    vpcmpeqb (str, size), %zmm16, %k1
    vmovdqu8 %zmm17, (str, size){%k1}

    add $64, size
    jnz LHEAD1

    ret

# void __mrfstr_avx512bw_replchr2(
#     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# res = RES+SIZE
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_avx512bw_replchr2
__mrfstr_avx512bw_replchr2:
    vpbroadcastb ochr2, %zmm17
    vpbroadcastb nchr2, %zmm18

#ifdef WIN32
    mov 40(%rsp), size2
#endif

LHEAD2:
    vmovdqu64 (str2, size2), %zmm16

    vpcmpeqb %zmm17, %zmm16, %k1
    vpblendmb %zmm18, %zmm16, %zmm16{%k1}

    vmovdqa64 %zmm16, (res2, size2)

    add $64, size2
    jnz LHEAD2

    ret

# void __mrfstr_avx2_avx512bw_replchr(
#     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_avx2_avx512bw_replchr
__mrfstr_avx2_avx512bw_replchr:
    vpbroadcastb ochr, %ymm16
    vpbroadcastb nchr, %ymm17

LHEAD3:
    vpcmpeqb (str, size), %ymm16, %k1
    vmovdqu8 %ymm17, (str, size){%k1}

    add $32, size
    jnz LHEAD3

    ret

# void __mrfstr_avx2_avx512bw_replchr2(
#     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# res = RES+SIZE
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_avx2_avx512bw_replchr2
__mrfstr_avx2_avx512bw_replchr2:
    vpbroadcastb ochr2, %ymm17
    vpbroadcastb nchr2, %ymm18

#ifdef WIN32
    mov 40(%rsp), size2
#endif

LHEAD4:
    vmovdqu64 (str2, size2), %ymm16

    vpcmpeqb %ymm17, %ymm16, %k1
    vpblendmb %ymm18, %ymm16, %ymm16{%k1}

    vmovdqa64 %ymm16, (res2, size2)

    add $32, size2
    jnz LHEAD4

    ret

# void __mrfstr_avx2_replchr(
#     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_avx2_replchr
__mrfstr_avx2_replchr:
    vpxor %xmm3, %xmm3, %xmm3

    vmovd ochr, %xmm1
    vpshufb %xmm3, %xmm1, %xmm1
    vinsertf128 $1, %xmm1, %ymm1, %ymm1

    vmovd nchr, %xmm2
    vpshufb %xmm3, %xmm2, %xmm2
    vinsertf128 $1, %xmm2, %ymm2, %ymm2

LHEAD5:
    vmovdqa (str, size), %ymm0

    vpcmpeqb %ymm1, %ymm0, %ymm3
    vpblendvb %ymm3, %ymm2, %ymm0, %ymm0

    vmovdqa %ymm0, (str, size)

    add $32, size
    jnz LHEAD5

    vzeroupper
    ret

# void __mrfstr_avx2_replchr2(
#     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# res = RES+SIZE
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_avx2_replchr2
__mrfstr_avx2_replchr2:
    vpxor %xmm3, %xmm3, %xmm3

    vmovd ochr, %xmm1
    vpshufb %xmm3, %xmm1, %xmm1
    vinsertf128 $1, %xmm1, %ymm1, %ymm1

    vmovd nchr, %xmm2
    vpshufb %xmm3, %xmm2, %xmm2
    vinsertf128 $1, %xmm2, %ymm2, %ymm2

#ifdef WIN32
    mov 40(%rsp), size2
#endif

LHEAD6:
    vmovdqu (str2, size2), %ymm0

    vpcmpeqb %ymm1, %ymm0, %ymm3
    vpblendvb %ymm3, %ymm2, %ymm0, %ymm0

    vmovdqa %ymm0, (str2, size2)

    add $32, size2
    jnz LHEAD6

    vzeroupper
    ret

# void __mrfstr_sse41_avx512bw_replchr(
#     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_sse41_avx512bw_replchr
__mrfstr_sse41_avx512bw_replchr:
    vpbroadcastb ochr, %xmm0
    vpbroadcastb nchr, %xmm1

LHEAD7:
    vpcmpeqb (str, size), %xmm0, %k1
    vmovdqu8 %xmm1, (str, size){%k1}

    add $16, size
    jnz LHEAD7

    ret

# void __mrfstr_sse41_avx512bw_replchr2(
#     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# res = RES+SIZE
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_sse41_avx512bw_replchr2
__mrfstr_sse41_avx512bw_replchr2:
    vpbroadcastb ochr2, %xmm1
    vpbroadcastb nchr2, %xmm2

#ifdef WIN32
    mov 40(%rsp), size2
#endif

LHEAD8:
    vmovdqu64 (str2, size2), %xmm0

    vpcmpeqb %xmm1, %xmm0, %k1
    vpblendmb %xmm2, %xmm0, %xmm0{%k1}

    vmovdqa64 %xmm0, (str2, size2)

    add $16, size2
    jnz LHEAD8

    ret

# void __mrfstr_sse41_replchr(
#     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_sse41_replchr
__mrfstr_sse41_replchr:
    movd ochr, %xmm2
    punpcklbw %xmm2, %xmm2
    punpcklwd %xmm2, %xmm2
    pshufd $0, %xmm2, %xmm2

    movd nchr, %xmm3
    punpcklbw %xmm3, %xmm3
    punpcklwd %xmm3, %xmm3
    pshufd $0, %xmm3, %xmm3

LHEAD9:
    movdqa (str, size), %xmm1
    movdqa %xmm1, %xmm0

    pcmpeqb %xmm2, %xmm0
    pblendvb %xmm0, %xmm3, %xmm1

    movdqa %xmm1, (str, size)

    add $16, size
    jnz LHEAD9

    ret

# void __mrfstr_sse41_replchr2(
#     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# res = RES+SIZE
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_sse41_replchr2
__mrfstr_sse41_replchr2:
    movd ochr2, %xmm2
    punpcklbw %xmm2, %xmm2
    punpcklwd %xmm2, %xmm2
    pshufd $0, %xmm2, %xmm2

    movd nchr2, %xmm3
    punpcklbw %xmm3, %xmm3
    punpcklwd %xmm3, %xmm3
    pshufd $0, %xmm3, %xmm3

#ifdef WIN32
    mov 40(%rsp), size2
#endif

LHEAD10:
    movdqu (str2, size2), %xmm1
    movdqa %xmm1, %xmm0

    pcmpeqb %xmm2, %xmm0
    pblendvb %xmm0, %xmm3, %xmm1

    movdqa %xmm1, (res2, size2)

    add $16, size2
    jnz LHEAD10

    ret

# void __mrfstr_i64_replchr(
#     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
#
# str = STR+SIZE
# ochr = OCHR
# nchr = NCHR
# size = -SIZE

    .globl __mrfstr_i64_replchr
__mrfstr_i64_replchr:
    push %rbx
    mov $0x7f7f7f7f7f7f7f7f, %rbx
    mov $0x0101010101010101, %rax

#ifdef WIN32
    movzx %dl, %r11
    imul %rax, %r11

    movzx %r8b, rnchr
#else
    movzx %dil, %r11
    imul %rax, %r11

    movzx %cl, rnchr
#endif

LHEAD11:
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
    jnz LHEAD11

    pop %rbx
    ret

# void __mrfstr_i64_replchr2(
#     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
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
    mov $0x7f7f7f7f7f7f7f7f, %rbx
    mov $0x0101010101010101, %rax

#ifdef WIN32
    movzx %r8b, rochr2
    imul %rax, rochr2
    movzx %r9b, rnchr2

    mov 56(%rsp), size2
#else
    movzx %cl, rochr2
    imul %rax, rochr2
    movzx %dl, rnchr2
#endif

LHEAD12:
    mov (str2, size2), %r10
    mov %r10, %r11

    xor rochr2, %r11
    mov %r11, %rbp

    and %rbx, %rbp
    lea (%rbp, %rbx), %rbp
    or %r11, %rbp
    or %rbx, %rbp
    not %rbp
    shr $7, %rbp

    mov %rbp, %r11
    imul $255, %r11
    not %r11
    and %r11, %r10

    imul rnchr2, %rbp
    or %rbp, %r10
    mov %r10, (res2, size2)

    add $8, size2
    jnz LHEAD12

    pop %rbp
    pop %rbx
    ret
