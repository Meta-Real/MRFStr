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

; mrfstr_byte_t mrfstr_cpuid_cputype(
;     mrfstr_byte_t *family, mrfstr_byte_t *model)

.data
    _is_intel db 0ffh
    public _is_intel

    _vendor db ?
    _family db ?
    _model db ?

.code
mrfstr_cpuid_cputype proc
    cmp byte ptr [_is_intel], 0ffh
    jne SAVED

    push rbx
    mov r8, rcx
    mov r9, rdx

    xor eax, eax
    cpuid

    cmp ecx, 6c65746eh  ; ntel
    je VINTEL

    cmp ecx, 444d4163h  ; cAMD
    je VAMD

    xor sil, sil        ; Set vendor id to 0 (unknown vendor)
    mov byte ptr [_is_intel], 0
    jmp FAMILY_MODEL

VINTEL:
    mov sil, 1          ; Set vendor id to 1 (Intel)
    mov byte ptr [_is_intel], 1
    jmp FAMILY_MODEL

VAMD:
    mov sil, 2          ; Set vendor id to 2 (AMD)
    mov byte ptr [_is_intel], 0
    jmp FAMILY_MODEL

FAMILY_MODEL:
    mov [_vendor], sil

    mov eax, 1
    cpuid

    mov ebx, eax
    mov edi, eax
    shr ebx, 8
    and ebx, 0fh        ; FamilyID
    shr edi, 20         ; ExtendedFamilyID
    lea ebx, [ebx+edi]  ; FamilyID + ExtendedFamilyID
    mov [r8], bl
    mov [_family], bl

    mov dil, al
    shr dil, 4          ; Model
    shr eax, 12
    and al, 0f0h        ; ExtendedModelId << 8
    or al, dil          ; (ExtendedModelId << 8) | Model
    mov [r9], al
    mov [_model], al

    mov al, sil
    pop rbx
    ret

SAVED:
    mov sil, [_family]
    mov [rcx], sil
    mov sil, [_model]
    mov [rdx], sil
    mov al, [_vendor]
    ret
mrfstr_cpuid_cputype endp
end
