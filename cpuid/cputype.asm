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
    mov r9, rcx
    mov r10, rdx

    xor eax, eax
    cpuid

    cmp ecx, 6c65746eh  ; ntel
    je VINTEL

    cmp ecx, 444d4163h  ; cAMD
    je VAMD

    xor r8b, r8b        ; Set vendor id to 0 (unknown vendor)
    mov byte ptr [_is_intel], 0
    jmp FAMILY_MODEL

VINTEL:
    mov r8b, 1          ; Set vendor id to 1 (Intel)
    mov byte ptr [_is_intel], 1
    jmp FAMILY_MODEL

VAMD:
    mov r8b, 2          ; Set vendor id to 2 (AMD)
    mov byte ptr [_is_intel], 0
    jmp FAMILY_MODEL

FAMILY_MODEL:
    mov [_vendor], r8b

    mov eax, 1
    cpuid

    mov ebx, eax
    mov edi, eax
    shr ebx, 8
    and ebx, 0fh        ; FamilyID
    shr edi, 20         ; ExtendedFamilyID
    lea ebx, [ebx+edi]  ; FamilyID + ExtendedFamilyID

    test r9, r9
    jz _FAMILY_ 
    mov [r9], bl

_FAMILY_:
    mov [_family], bl

    mov bl, al
    shr bl, 4           ; Model
    shr eax, 12
    and al, 0f0h        ; ExtendedModelId << 8
    or al, bl           ; (ExtendedModelId << 8) | Model

    test r10, r10
    jz _MODEL_
    mov [r10], al

_MODEL_:
    mov [_model], al

    mov al, r8b
    pop rbx
    ret

SAVED:
    test rcx, rcx
    jz _MODEL_S

    mov al, [_family]
    mov [rcx], al

_MODEL_S:
    test rcx, rcx
    jz _VENDOR_S

    mov al, [_model]
    mov [rdx], al

_VENDOR_S:
    mov al, [_vendor]
    ret
mrfstr_cpuid_cputype endp
end
