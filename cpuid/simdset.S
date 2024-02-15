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

# mrfstr_byte_t mrfstr_cpuid_simdset(void)

# 1: SSE2 (guaranteed on x64 processors)
# 2: SSSE3
# 3: SSE4.1
# 4: AVX
# 5: AVX2
# 6: AVX512F
# 7: AVX512BW, AVX512VL
# 8: AVX512VBMI

    .extern _funccnt

    .bss
_simdset:
    .zero 1

    .text
    .globl mrfstr_cpuid_simdset
mrfstr_cpuid_simdset:
    cmpb $0, _simdset(%rip)
    jne SAVED

    push %rbx
    mov $1, %r8b

    mov $1, %eax
    cpuid

    bt $9, %ecx
    jnc LASTINST
    inc %r8b                # SSSE3

    bt $19, %ecx
    jnc LASTINST
    inc %r8b                # SSE4.1

    bt $28, %ecx
    jnc LASTINST
    bt $27, %ecx
    jnc LASTINST
    xor %ecx, %ecx
    xgetbv
    and $6, %eax
    cmp $6, %eax
    jne LASTINST
    inc %r8b                # AVX

    cmpb $7, _funccnt(%rip)
    jb LASTINST

    mov $7, %eax
    xor %ecx, %ecx
    cpuid

    bt $5, %ebx
    jnc LASTINST
    inc %r8b                # AVX2

    bt $16, %ebx
    jnc LASTINST
    push %rcx
    xor %ecx, %ecx
    xgetbv
    pop %rcx
    and $0xe0, %eax
    cmp $0xe0, %eax
    jne LASTINST
    inc %r8b                # AVX512F

    bt $30, %ebx
    jnc LASTINST
    bt $31, %ebx
    jnc LASTINST
    inc %r8b                # AVX512BW, AVX512VL

    bt $1, %ecx
    jnc LASTINST
    inc %r8b                # AVX512VBMI

LASTINST:
    mov %r8b, %al
    pop %rbx
    ret

SAVED:
    mov _simdset(%rip), %al
    ret
