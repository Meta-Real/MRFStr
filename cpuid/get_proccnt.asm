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

; mrfstr_byte_t mrfstr_get_proccnt(mrfstr_byte_t *logical, mrfstr_bool_t is_intel)

.code
mrfstr_get_proccnt proc
    push rbx
    mov r8, rcx

    bt edx, 0
    jc INTEL

    mov eax, 1
    cpuid

    shr ebx, 16
    and ebx, 0ffh
    mov [r8], ebx       ; logical processors (on AMD)

    mov eax, 80000008h
    cpuid

    and ecx, 0ffh
    lea eax, [ecx+1]    ; cores (on AMD)

    pop rbx
    ret

INTEL:
    mov eax, 0bh
    mov ecx, 1
    cpuid

    mov [r8], ebx       ; logical processors (on Intel)
;   mov eax, eax        ; cores (on Intel)

    pop rbx
    ret
mrfstr_get_proccnt endp
end
