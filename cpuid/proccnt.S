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

# mrfstr_byte_t mrfstr_cpuid_proccnt(
#     mrfstr_byte_t *logical)

    .extern _is_intel

.bss
_corecnt:
    .byte 0
_logicnt:
    .zero 1

.text
    .globl mrfstr_cpuid_proccnt
mrfstr_cpuid_proccnt:
    cmpb $0, _corecnt(%rip)
    jne SAVED

    push %rbx
#ifdef WIN32
    mov %rcx, %r8
#endif

    btw $0, _is_intel(%rip)
    jnc AMD

    mov $0xb, %eax
    mov $1, %ecx
    cpuid

#ifdef WIN32
    test %r8, %r8
    jz _CORECNT_
    mov %bl, (%r8)
#else
    test %rsi, %rsi
    jz _CORECNT_
    mov %bl, (%rsi)         # logical processors (on Intel)
#endif

_CORECNT_:
    mov %al, _corecnt(%rip) # cores (on Intel)
    mov %bl, _logicnt(%rip)

    pop %rbx
    ret

AMD:
    mov $1, %eax
    cpuid

    shr $16, %ebx

#ifdef WIN32
    test %r8, %r8
    jz _LOGICNT_
    mov %bl, (%r8)
#else
    test %rsi, %rsi
    jz _LOGICNT_
    mov %bl, (%rsi)         # logical processors (on AMD)
#endif

_LOGICNT_:
    mov %bl, _logicnt(%rip)

    mov $0x80000008, %eax
    cpuid

    lea 1(%ecx), %eax       # cores (on AMD)
    mov %al, _corecnt(%rip)

    pop %rbx
    ret

SAVED:
#ifdef WIN32
    test %rcx, %rcx
    jz _CORECNT_S

    mov _logicnt(%rip), %al
    mov %al, (%rcx)
#else
    test %rsi, %rsi
    jz _CORECNT_S

    mov _logicnt(%rip), %al
    mov %al, (%rsi)
#endif

_CORECNT_S:
    mov _corecnt(%rip), %al
    ret
