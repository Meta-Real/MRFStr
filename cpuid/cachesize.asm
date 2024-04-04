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

; mrfstr_size_t mrfstr_cpuid_cachesize(
;     mrfstr_size_t *l2, mrfstr_size_t *l3)
; PRE: mrfstr_cpuid_cputype, mrfstr_cpuid_funccnt

    extern _is_intel : db
    extern _funccnt : db
    extern _extcnt : db

.data
    _l1 dq 0
    _l2 dq 0
    _l3 dq 0

.code

mrfstr_cpuid_cachesize proc
    cmp qword ptr [_l1], 0
    jne SAVED

    mov qword ptr [rcx], 0
    mov qword ptr [rdx], 0

    push rbx
    mov r8, rcx
    mov r9, rdx
    xor r10, r10

    bt word ptr [_is_intel], 0
    jnc AMD

    cmp [_funccnt], 4
    jb RETURN

    xor r11d, r11d
LHEAD:
    mov eax, 4
    mov ecx, r11d
    cpuid

    mov dl, al
    and dl, 31
    jz RETURN

    cmp dl, 2           ; check for instruction cache
    je IINC

    shr al, 5
    and al, 7
    cmp al, 4           ; check for L4 cache
    jge IINC

    inc ecx             ; sets

    mov edx, ebx
    shr edx, 22
    inc dx              ; ways of associativity
    imul ecx, edx

    mov edx, ebx
    shr edx, 12
    and edx, 3ffh
    inc edx             ; partitions
    imul ecx, edx

    and ebx, 0fffh
    inc ebx             ; cache line size
    imul rcx, rbx

    cmp al, 1
    je IL1

    cmp al, 2
    je IL2

    mov [r9], rcx
    mov [_l3], rcx

IINC:
    inc r11d
    cmp r11d, 32
    jne LHEAD

    jmp RETURN

IL1:
    mov r10, rcx
    mov [_l1], rcx
    jmp IINC

IL2:
    mov [r8], rcx
    mov [_l2], rcx
    jmp IINC

AMD:
    cmp [_extcnt], 6
    jb RETURN 

    mov eax, 80000005h
    cpuid

    shr ecx, 24
    shl ecx, 10         ; L1 cache
    mov r10, rcx
    mov [_l1], rcx

    mov eax, 80000006h
    cpuid

    shr ecx, 16
    shl ecx, 10         ; L2 cache
    mov [r8], rcx
    mov [_l2], rcx

    mov ecx, edx
    shr ecx, 18
    shl rcx, 19         ; L3 cache
    mov [r9], rcx
    mov [_l3], rcx

RETURN:
    mov rax, r10
    pop rbx
    ret

SAVED:
    mov rax, [_l2]
    mov [rcx], rax

    mov rax, [_l3]
    mov [rdx], rax

    mov rax, [_l1]
    ret
mrfstr_cpuid_cachesize endp

end
