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

# mrfstr_byte_t mrfstr_cpuid_cputype(
#     mrfstr_byte_t *family, mrfstr_byte_t *model)

    .globl _mrfstr_is_intel
    .bss
_mrfstr_is_intel:
    .space 1

    .text
    .globl mrfstr_cpuid_cputype
mrfstr_cpuid_cputype:
    push %rbx
#ifdef WIN32
    mov %rcx, %r8
    mov %rdx, %r9
#endif

    xor %eax, %eax
    cpuid

    cmp $0x6c65746e, %ecx   # ntel
    je VINTEL

    cmp $0x444d4163, %ecx   # cAMD
    je VAMD

    xor %r10d, %r10d        # Set vendor id to 0 (unknown vendor)
    jmp FAMILY_MODEL

VINTEL:
    mov $1, %r10d           # Set vendor id to 1 (Intel)
    movb $1, _mrfstr_is_intel(%rip)
    jmp FAMILY_MODEL

VAMD:
    mov $2, %r10d           # Set vendor id to 2 (AMD)
    jmp FAMILY_MODEL

FAMILY_MODEL:
    mov $1, %eax
    cpuid

    mov %eax, %ebx
    mov %eax, %r11d
    shr $8, %ebx
    and $0xf, %ebx          # FamilyID
    shr $20, %r11d
    and $0xff, %r11d        # ExtendedFamilyID
    add %r11d, %ebx         # FamilyID + ExtendedFamilyID
#ifdef WIN32
    mov %ebx, (%r8)
#else
    mov %ebx, (%rsi)
#endif

    mov %eax, %ebx
    shr $4, %ebx
    and $0xf, %ebx          # Model
    shr $12, %eax
    and $0xf0, %eax         # ExtendedModelId << 8
    or %ebx, %eax           # (ExtendedModelId << 8) | Model
#ifdef WIN32
    mov %eax, (%r9)
#else
    mov %eax, (%rdi)
#endif

    mov %r10d, %eax
    pop %rbx
    ret
