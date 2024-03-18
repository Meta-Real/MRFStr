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

; mrfstr_byte_t mrfstr_cpuid_simdset(void)
; PRE: mrfstr_cpuid_funccnt

; 1: SSE2 (guaranteed on x64 processors)
; 2: SSSE3
; 3: SSE4.1
; 4: AVX
; 5: AVX2
; 6: AVX512F
; 7: AVX512BW, AVX512DQ, AVX512VL
; 8: AVX512VBMI

    extern _funccnt : db

.data
    _simdset db 0

.code
mrfstr_cpuid_simdset proc
    cmp byte ptr [_simdset], 0
    jne SAVED

    push rbx
    mov r8b, 1

    mov eax, 1
    cpuid

    bt ecx, 9
    jnc LASTINST
    inc r8b             ; SSSE3

    bt ecx, 19
    jnc LASTINST
    inc r8b             ; SSE4.1

    bt ecx, 28
    jnc LASTINST
    bt ecx, 27
    jnc LASTINST
    xor ecx, ecx
    xgetbv
    and eax, 6
    cmp eax, 6
    jne LASTINST
    inc r8b             ; AVX

    cmp byte ptr [_funccnt], 7
    jb LASTINST

    mov eax, 7
    xor ecx, ecx
    cpuid

    bt ebx, 5
    jnc LASTINST
    inc r8b             ; AVX2

    bt ebx, 16
    jnc LASTINST
    push rcx
    xor ecx, ecx
    xgetbv
    pop rcx
    and eax, 0e0h
    cmp eax, 0e0h
    jne LASTINST
    inc r8b             ; AVX512F

    bt ebx, 30
    jnc LASTINST
    inc r8b             ; AVX512BW, AVX512DQ, AVX512VL

    bt ecx, 1
    jnc LASTINST
    inc r8b             ; AVX512VBMI

LASTINST:
    mov al, r8b
    mov [_simdset], al

    pop rbx
    ret

SAVED:
    mov al, [_simdset]
    ret
mrfstr_cpuid_simdset endp
end
