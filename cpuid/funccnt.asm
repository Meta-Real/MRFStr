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

; mrfstr_byte_t mrfstr_cpuid_funccnt(
;     mrfstr_byte_t *ext)

.data
    _funccnt db 0
    _extcnt db ?
    public _funccnt
    public _extcnt

.code
mrfstr_cpuid_funccnt proc
    cmp byte ptr [_funccnt], 0
    jne SAVED

    push rbx
    mov r8, rcx

    mov eax, 80000000h
    cpuid

    test r8, r8
    jz _EXTCNT_
    mov [r8], al        ; extended function count

_EXTCNT_:
    mov [_extcnt], al

    xor eax, eax
    cpuid

    mov [_funccnt], al  ; function count

    pop rbx
    ret

SAVED:
    test rcx, rcx
    jz _FUNCCNT_S

    mov al, [_extcnt]
    mov [rcx], al

_FUNCCNT_S:
    mov al, [_funccnt]
    ret
mrfstr_cpuid_funccnt endp
end
