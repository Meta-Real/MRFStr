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

; mrfstr_byte_t mrfstr_get_simdset(void)

; 1: SSE2
; 2: SSSE3
; 3: SSE4.1
; 4: AVX
; 5: AVX2
; 6: AVX512F
; 7: AVX512BW, AVX512VL
; 8: AVX512VBMI

.code
mrfstr_get_simdset proc
    push rbx
    mov esi, 1

    mov eax, 1
    cpuid

    bt ecx, 9
    jnc LASTINST
    inc esi             ; SSSE3

    bt ecx, 19
    jnc LASTINST
    inc esi             ; SSE4.1

    bt ecx, 28
    jnc LASTINST
    bt ecx, 27
    jnc LASTINST
    xor ecx, ecx
    xgetbv
    and eax, 6
    cmp eax, 6
    jne LASTINST
    inc esi             ; AVX

    mov eax, 7
    xor ecx, ecx
    cpuid

    bt ebx, 5
    jnc LASTINST
    inc esi             ; AVX2

    bt ebx, 16
    jnc LASTINST
    push rcx
    xor ecx, ecx
    xgetbv
    pop rcx
    and eax, 0e0h
    cmp eax, 0e0h
    jne LASTINST
    inc esi             ; AVX512F

    bt ebx, 30
    jnc LASTINST
    bt ebx, 31
    jnc LASTINST
    inc esi             ; AVX512BW, AVX512VL

    bt ecx, 1
    jnc LASTINST
    inc esi             ; AVX512VBMI

LASTINST:
    mov eax, esi

    pop rbx
    ret
mrfstr_get_simdset endp
end
