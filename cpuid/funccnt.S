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

# mrfstr_byte_t mrfstr_cpuid_funccnt(void)

    .extern _mrfstr_is_intel
    .globl _mrfstr_funccnt
    .bss
_mrfstr_funccnt:
    .space 1

    .text
    .globl mrfstr_cpuid_funccnt
mrfstr_cpuid_funccnt:
    push %rbx

    btw $0, _mrfstr_is_intel(%rip)
    jnc AMD

    xor %eax, %eax
    cpuid

    movb %al, _mrfstr_funccnt(%rip)
    pop %rbx
    ret

AMD:
    mov $0x80000000, %eax
    cpuid

    movb %al, _mrfstr_funccnt(%rip)
    pop %rbx
    ret
