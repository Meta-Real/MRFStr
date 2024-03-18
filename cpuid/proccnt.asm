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

; mrfstr_byte_t mrfstr_cpuid_proccnt(
;     mrfstr_byte_t *logical)
; PRE: mrfstr_cpuid_cputype

    extern _is_intel : db

.data
    _corecnt db 0
    _logicnt db ?

.code
mrfstr_cpuid_proccnt proc
    cmp byte ptr [_corecnt], 0
    jne SAVED

    push rbx
    mov r8, rcx

    bt word ptr [_is_intel], 0
    jnc AMD

    mov eax, 0bh
    mov ecx, 1
    cpuid

    test r8, r8
    jz _CORECNT_
    mov [r8], bl        ; logical processors (on Intel)

_CORECNT_:
    mov [_corecnt], al  ; cores (on Intel)
    mov [_logicnt], bl

    pop rbx
    ret

AMD:
    mov eax, 1
    cpuid

    shr ebx, 16

    test r8, r8
    jz _LOGICNT_
    mov [r8], bl        ; logical processors (on AMD)

_LOGICNT_:
    mov [_logicnt], bl

    mov eax, 80000008h
    cpuid

    lea eax, [ecx+1]    ; cores (on AMD)
    mov [_corecnt], al

    pop rbx
    ret

SAVED:
    test rcx, rcx
    jz _CORECNT_S

    mov al, [_logicnt]
    mov [rcx], al

_CORECNT_S:
    mov al, [_corecnt]
    ret
mrfstr_cpuid_proccnt endp
end
