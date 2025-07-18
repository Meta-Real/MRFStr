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

# mrfstr_size_t mrfstr_cpuid_cachesize(
#     mrfstr_size_t *l2, mrfstr_size_t *l3)
# PRE: mrfstr_cpuid_cputype, mrfstr_cpuid_funccnt

#ifdef WIN32
#define l21 %rcx
#define l31 %rdx
#define l22 %r8
#define l32 %r9
#else
#define l21 %rdi
#define l31 %rsi
#define l22 %rdi
#define l32 %rsi
#endif

    .extern _is_intel
    .extern _funccnt
    .extern _extcnt

.bss
_l1:
    .quad 0
_l2:
    .quad 0
_l3:
    .quad 0

.text
    .globl mrfstr_cpuid_cachesize
mrfstr_cpuid_cachesize:
    cmpq $0, _l1(%rip)
    jne SAVED

    movq $0, (l21)
    movq $0, (l31)

    push %rbx
#ifdef WIN32
    mov l21, l22
    mov l31, l32
#endif
    xor %r10, %r10

    bt $0, _is_intel(%rip)
    jnc AMD

    cmpb $4, _funccnt(%rip)
    jb RETURN

    xor %r11d, %r11d
LHEAD:
    mov $4, %eax
    mov %r11d, %ecx
    cpuid

    mov %al, %dl
    and $31, %dl
    jz RETURN

    cmp $2, %dl             # check for instruction cache
    je IINC

    shr $5, %al
    and $7, %al
    cmp $4, %al             # check for L4 cache
    jge IINC

    inc %ecx                # sets

    mov %ebx, %edx
    shr $22, %edx
    inc %dx                 # ways of associativity
    imul %edx, %ecx

    mov %ebx, %edx
    shr $12, %edx
    and $0x33f, %edx
    inc %edx                # partitions
    imul %edx, %ecx

    and $0xfff, %ebx
    inc %ebx                # cache line size
    imul %rbx, %rcx

    cmp $1, %rcx
    je IINC

    cmp $1, %al
    je IL1

    cmp $2, %al
    je IL2

    mov %rcx, (l32)
    mov %rcx, _l3(%rip)

IINC:
    inc %r11d
    cmp $32, %r11d
    jne LHEAD

    jmp RETURN

IL1:
    mov %rcx, %r10
    mov %rcx, _l1(%rip)
    jmp IINC

IL2:
    mov %rcx, (l22)
    mov %rcx, _l2(%rip)
    jmp IINC

AMD:
    cmpb $6, _extcnt(%rip)
    jb RETURN

    mov $0x80000005, %eax
    cpuid

    shr $24, %ecx
    shl $10, %ecx           # L1 cache
    mov %rcx, %r10
    mov %rcx, _l1(%rip)

    mov $0x80000006, %eax
    cpuid

    shr $16, %ecx
    shl $10, %ecx           # L2 cache
    mov %rcx, (l22)
    mov %rcx, _l2(%rip)

    mov %edx, %ecx
    shr $18, %ecx
    shl $19, %rcx           # L3 cache
    mov %rcx, (l32)
    mov %rcx, _l3(%rip)

RETURN:
    mov %r10, %rax
    pop %rbx
    ret

SAVED:
    mov _l2(%rip), %rax
    mov %rax, (l21)

    mov _l3(%rip), %rax
    mov %rax, (l31)

    mov _l1(%rip), %rax
    ret
