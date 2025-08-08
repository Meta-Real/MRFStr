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
#define chr %edx
#define size %r8

#define rchr %rdx
#define mask1 %rsi
#define mask2 %rdi

#define chrl %dl
#else
#define str %rdi
#define chr %esi
#define size %rdx

#define rchr %rsi
#define mask1 %rcx
#define mask2 %r8

#define chrl %sil
#endif

.text

# mrfstr_size_t __mrfstr_avx512bw_countchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx512bw_countchr
__mrfstr_avx512bw_countchr:
    vpbroadcastb chr, %zmm16
    xor %rax, %rax

LHEAD1:
    vpcmpeqb (str, size), %zmm16, %k0

    kmovq %k0, rchr
    popcnt rchr, rchr
    lea (%rax, rchr), %rax

    add $64, size
    jnz LHEAD1

    ret

# mrfstr_size_t __mrfstr_avx2_countchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx2_countchr
__mrfstr_avx2_countchr:
    vpxor %xmm1, %xmm1, %xmm1
    vmovd chr, %xmm0
    vpshufb %xmm1, %xmm0, %xmm0
    vinsertf128 $1, %xmm0, %ymm0, %ymm0
    xor %rax, %rax

LHEAD2:
    vpcmpeqb (str, size), %ymm0, %ymm1

    vpmovmskb %ymm1, chr
    popcnt chr, chr
    lea (%rax, rchr), %rax

    add $32, size
    jnz LHEAD2

    vzeroupper
    ret

# mrfstr_size_t __mrfstr_sse42_countchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_sse42_countchr
__mrfstr_sse42_countchr:
    movd chr, %xmm1
    punpcklbw %xmm1, %xmm1
    punpcklwd %xmm1, %xmm1
    pshufd $0, %xmm1, %xmm1
    xor %rax, %rax

LHEAD3:
    movdqa (str, size), %xmm0
    pcmpeqb %xmm1, %xmm0

    pmovmskb %xmm0, chr
#ifdef WIN32
    popcnt %dx, %dx
#else
    popcnt %si, %si
#endif
    lea (%rax, rchr), %rax

    add $16, size
    jnz LHEAD3

    ret

# mrfstr_size_t __mrfstr_sse2_countchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_sse2_countchr
__mrfstr_sse2_countchr:
    movd chr, %xmm1
    punpcklbw %xmm1, %xmm1
    punpcklwd %xmm1, %xmm1
    pshufd $0, %xmm1, %xmm1
    xor %rax, %rax

LHEAD4:
    movdqa (str, size), %xmm0
    pcmpeqb %xmm1, %xmm0

    pmovmskb %xmm0, chr
    mov chr, %r9d
    shr $1, %r9d
    and $0x55555555, %r9d
    sub %r9d, chr

    mov chr, %r9d
    and $0x33333333, %r9d
    and $0x33333333, chr
    lea (chr, %r9d), chr

    mov chr, %r9d
    shr $4, %r9d
    lea (chr, %r9d), chr
    and $0x0f0f0f0f, chr

    imul $0x01010101, chr
    shr $24, chr
    lea (rchr, %rax), %rax

    add $16, size
    jnz LHEAD4

    ret

# mrfstr_size_t __mrfstr_i64_sse42_countchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_i64_sse42_countchr
__mrfstr_i64_sse42_countchr:
    mov $0x0101010101010101, %rax
    mov $0x7f7f7f7f7f7f7f7f, %r11

    movzx chrl, rchr
    imul %rax, rchr
    xor %rax, %rax

LHEAD5:
    mov (str, size), %r9
    xor rchr, %r9

    mov %r9, %r10
    and %r11, %r9
    lea (%r9, %r11), %r9
    or %r10, %r9
    or %r11, %r9
    not %r9

    popcnt %r9, %r9
    lea (%rax, %r9), %rax

    add $8, size
    jnz LHEAD5

    ret

# mrfstr_size_t __mrfstr_i64_countchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_i64_countchr
__mrfstr_i64_countchr:
    push %rbx
    push %rbp
#ifdef WIN32
    push %rsi
    push %rdi
#endif

    mov $0x7f7f7f7f7f7f7f7f, %r11
    mov $0x0101010101010101, %rbx
    mov $0x5555555555555555, %rbp
    mov $0x3333333333333333, mask1
    mov $0x0f0f0f0f0f0f0f0f, mask2

    movzx chrl, rchr
    imul %rbx, rchr
    xor %rax, %rax

LHEAD6:
    mov (str, size), %r9
    xor rchr, %r9

    mov %r9, %r10
    and %r11, %r9
    lea (%r9, %r11), %r9
    or %r10, %r9
    or %r11, %r9
    not %r9

    mov %r9, %r10
    shr $1, %r10
    and %rbp, %r10
    sub %r10, %r9

    mov %r9, %r10
    shr $2, %r10
    and mask1, %r10
    and mask1, %r9
    lea (%r9, %r10), %r9

    mov %r9, %r10
    shr $4, %r10
    lea (%r9, %r10), %r9
    and mask2, %r9

    imul %rbx, %r9
    shr $56, %r9
    lea (%rax, %r9), %rax

    add $8, size
    jnz LHEAD6

#ifdef WIN32
    pop %rdi
    pop %rsi
#endif
    pop %rbp
    pop %rbx
    ret