; MIT License
;
; Copyright (c) 2023 MetaReal
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.

; mrfstr_byte_t mrfstr_get_cputype(mrfstr_byte_t *family, mrfstr_byte_t *model)

.code
mrfstr_get_cputype proc
    push rbx

    mov r8, rcx
    mov r9, rdx

    xor eax, eax
    cpuid

    cmp ecx, 6c65746eh  ; ntel
    je VINTEL

    cmp ecx, 444d4163h  ; cAMD
    je VAMD

    xor esi, esi        ; Set vendor id to 0 (unknown vendor)
    jmp FAMILY_MODEL

VINTEL:
    mov esi, 1          ; Set vendor id to 1 (Intel)
    jmp FAMILY_MODEL

VAMD:
    mov esi, 2          ; Set vendor id to 2 (AMD)
    jmp FAMILY_MODEL

FAMILY_MODEL:
    mov eax, 1
    cpuid

    mov ebx, eax
    mov edi, eax

    shr ebx, 8
    and ebx, 0fh        ; FamilyID
    shr edi, 20
    and edi, 0ffh       ; ExtendedFamilyID
    add ebx, edi        ; FamilyID + ExtendedFamilyID
    mov [r8], ebx

    mov edi, eax
    shr edi, 4
    and edi, 0fh        ; Model
    shr eax, 12
    and eax, 0f0h       ; ExtendedModelId << 8
    or eax, edi         ; (ExtendedModelId << 8) | Model
    mov [r9], eax

    mov eax, esi

    pop rbx
    ret
mrfstr_get_cputype endp
end
