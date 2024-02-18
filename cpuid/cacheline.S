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

# mrfstr_short_t mrfstr_cpuid_cacheline(void)

.bss
_cacheline:
    .byte 0

.text
    .globl mrfstr_cpuid_cacheline
mrfstr_cpuid_cacheline:
    cmpb $0, _cacheline(%rip)
    jne SAVED

    push %rbx

    mov $1, %eax
    cpuid

    bt $19, %edx
    jnc NOCLFSH

    shr $5, %bx
    and $0x7f8, %bx
    mov %bx, %ax
    mov %bx, _cacheline(%rip)

    pop %rbx
    ret

NOCLFSH:
    mov $64, %ax
    mov %ax, _cacheline(%rip)

    pop %rbx
    ret

SAVED:
    mov _cacheline(%rip), %ax
    ret
