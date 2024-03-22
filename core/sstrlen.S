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
#else
#define str %rsi
#endif

.text

# mrfstr_size_t __mrfstr_avx512bw_strlen(
#     mrfstr_ptr_ct str)
#
# str = STR

    .globl __mrfstr_avx512bw_strlen
__mrfstr_avx512bw_strlen:
    vpxorq %zmm16, %zmm16, %zmm16
    xor %rax, %rax

LHEAD1:
    vpcmpeqb (str, %rax), %zmm16, %k0
    add $64, %rax

    kortestq %k0, %k0
    jz LHEAD1

    kmovq %k0, %rdx
    bsf %rdx, %rdx
    lea -64(%rax, %rdx), %rax
    ret

# mrfstr_size_t __mrfstr_avx2_strlen(
#     mrfstr_ptr_ct str)
#
# str = STR

    .globl __mrfstr_avx2_strlen
__mrfstr_avx2_strlen:
    vpxor %ymm0, %ymm0, %ymm0
    xor %rax, %rax

LHEAD2:
    vpcmpeqb (str, %rax), %ymm0, %ymm1
    add $32, %rax

    vpmovmskb %ymm1, %edx
    test %edx, %edx
    jz LHEAD2

    bsf %edx, %edx
    lea -32(%rax, %rdx), %rax
    
    vzeroupper
    ret

# mrfstr_size_t __mrfstr_sse2_strlen(
#     mrfstr_ptr_ct str)
#
# str = STR

    .globl __mrfstr_sse2_strlen
__mrfstr_sse2_strlen:
    pxor %xmm1, %xmm1
    xor %rax, %rax

LHEAD3:
    movdqa (str, %rax), %xmm0
    pcmpeqb %xmm1, %xmm0
    add $16, %rax

    pmovmskb %xmm0, %edx
    test %edx, %edx
    jz LHEAD3

    bsf %edx, %edx
    lea -16(%rax, %rdx), %rax
    ret

# mrfstr_size_t __mrfstr_i64_strlen(
#     mrfstr_ptr_ct str)
#
# str = STR

    .globl __mrfstr_i64_strlen
__mrfstr_i64_strlen:
    mov $0x0101010101010101, %r9
    mov $0x8080808080808080, %r10
    mov $0x7f7f7f7f7f7f7f7f, %r11
    xor %rax, %rax

LHEAD4:
    mov (str, %rax), %rdx
    mov %rdx, %r8
    add $8, %rax

    sub %r9, %rdx
    not %r8
    and %r8, %rdx
    and %r10, %rdx

    test %rdx, %rdx
    jz LHEAD4

    not %r8
    mov %r8, %rdx

    and %r11, %rdx
    lea (%rdx, %r11), %rdx
    or %r8, %rdx
    or %r11, %rdx
    not %rdx

    bsf %rdx, %rdx
    shr $3, %rdx
    lea -8(%rax, %rdx), %rax
    ret
