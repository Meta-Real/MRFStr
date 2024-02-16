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

; mrfstr_short_t mrfstr_cpuid_cacheline(void)

.data
    _cacheline dw 0

.code
mrfstr_cpuid_cacheline proc
    cmp word ptr [_cacheline], 0
    jne SAVED

    push rbx

    mov eax, 1
    cpuid

    bt edx, 19
    jnc NOCLFSH

    shr bx, 5
    and bx, 7f8h
    mov ax, bx
    mov word ptr [_cacheline], bx

    pop rbx
    ret

NOCLFSH:
    mov ax, 32
    mov word ptr [_cacheline], ax

    pop rbx
    ret

SAVED:
    mov ax, word ptr [_cacheline]
    ret
mrfstr_cpuid_cacheline endp
end
