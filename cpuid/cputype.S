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

.data
    .globl _is_intel
_is_intel:
    .byte 0xff

.bss
_vendor:
    .zero 1
_family:
    .zero 1
_model:
    .zero 1

.text
    .globl mrfstr_cpuid_cputype
mrfstr_cpuid_cputype:
    cmpb $0xff, _is_intel(%rip)
    jne SAVED

    push %rbx
#ifdef WIN32
    mov %rcx, %r10
    mov %rdx, %r11
#endif

    xor %eax, %eax
    cpuid

    cmp $0x6c65746e, %ecx   # ntel
    je VINTEL

    cmp $0x444d4163, %ecx   # cAMD
    je VAMD

    xor %r8b, %r8b          # Set vendor id to 0 (unknown vendor)
    movb $0, _is_intel(%rip)
    jmp FAMILY_MODEL

VINTEL:
    mov $1, %r8b            # Set vendor id to 1 (Intel)
    movb $1, _is_intel(%rip)
    jmp FAMILY_MODEL

VAMD:
    mov $2, %r8b            # Set vendor id to 2 (AMD)
    movb $0, _is_intel(%rip)
    jmp FAMILY_MODEL

FAMILY_MODEL:
    mov %r8b, _vendor(%rip)

    mov $1, %eax
    cpuid

    mov %eax, %ebx
    mov %eax, %r9d
    shr $8, %ebx
    and $0xf, %ebx          # FamilyID
    shr $20, %r9d
    and $0xff, %r9d         # ExtendedFamilyID
    lea (%ebx, %r9d), %ebx  # FamilyID + ExtendedFamilyID

#ifdef WIN32
    test %r10, %r10
    jz _FAMILY_
    mov %bl, (%r10)
#else
    test %rsi, %rsi
    jz _FAMILY_
    mov %bl, (%rsi)
#endif

_FAMILY_:
    mov %bl, _family(%rip)

    mov %al, %bl
    shr $4, %bl             # Model
    shr $12, %eax
    and $0xf0, %al          # ExtendedModelId << 8
    or %bl, %al             # (ExtendedModelId << 8) | Model

#ifdef WIN32
    test %r11, %r11
    jz _MODEL_
    mov %al, (%r11)
#else
    test %rdi, %rdi
    jz _MODEL_
    mov %al, (%rdi)
#endif

_MODEL_:
    mov %al, _model(%rip)

    mov %r8b, %al
    pop %rbx
    ret

SAVED:
#ifdef WIN32
    test %rcx, %rcx
    jz _MODEL_S

    mov _family(%rip), %al
    mov %al, (%rcx)

_MODEL_S:
    test %rdx, %rdx
    jz _VENDOR_S

    mov _model(%rip), %al
    mov %al, (%rdx)
#else
    test %rsi, %rsi
    jz _MODEL_S

    mov _family(%rip), %al
    mov %al, (%rsi)

_MODEL_S:
    test %rdi, %rdi
    jz _VENDOR_S

    mov _model(%rip), %al
    mov %al, (%rdi)
#endif

_VENDOR_S:
    mov _vendor(%rip), %al
    ret
