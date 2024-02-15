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

; mrfstr_byte_t mrfstr_cpuid_funccnt(void)

.data
    extern _is_intel : db
    _funccnt db 0
    public _funccnt

.code
mrfstr_cpuid_funccnt proc
    cmp byte ptr [_funccnt], 0
    jne SAVED

    push rbx

    bt word ptr [_is_intel], 0
    jnc AMD

    xor eax, eax
    cpuid

    mov [_funccnt], al  ; function count (on Intel)
    pop rbx
    ret

AMD:
    mov eax, 80000000h
    cpuid

    mov [_funccnt], al  ; function count (on AMD)
    pop rbx
    ret

SAVED:
    mov al, [_funccnt]
    ret
mrfstr_cpuid_funccnt endp
end
