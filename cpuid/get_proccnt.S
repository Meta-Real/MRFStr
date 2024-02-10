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

# mrfstr_byte_t mrfstr_get_proccnt(mrfstr_byte_t *logical, mrfstr_bool_t is_intel)

    .globl mrfstr_get_proccnt

mrfstr_get_proccnt:
    push %rbx

#ifdef WIN32
    mov %rcx, %r8
#endif

    bt $0, %edx
    jc INTEL

    mov $1, %eax
    cpuid

    shr $16, %ebx
    and $0xff, %ebx
#ifdef WIN32
    mov %ebx, (%r8)         # logical processors (on AMD)
#else
    mov %ebx, (%rsi)        # logical processors (on AMD)
#endif

    mov $0x80000008, %eax
    cpuid

    and $0xff, %ecx
    lea 1(%ecx), %eax       # cores (on AMD)

    pop %rbx
    ret

INTEL:
    mov $0xb, %eax
    mov $1, %ecx
    cpuid

#ifdef WIN32
    mov %ebx, (%r8)         # logical processors (on Intel)
#else
    mov %ebx, (%rsi)        # logical processors (on Intel)
#endif
#   mov %eax, %eax          # cores (on Intel)

    pop %rbx
    ret
