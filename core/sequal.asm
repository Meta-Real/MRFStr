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

.data
    extern _mrfstr_cmp_load : dq

.code

; mrfstr_bool_t __mrfstr_avx512dq_equal(
;     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
;
; str1 = STR1+SIZE
; str2 = STR2+SIZE
; size = -SIZE

__mrfstr_avx512dq_equal proc
LHEAD:
    vmovdqu64 zmm16, [rcx+r8]
    vpcmpq k1, zmm16, [rdx+r8], 4

    kortestb k1, k1
    jnz FALSE

    add r8, 64
    jnz LHEAD

    mov al, 1
    ret

FALSE:
    xor al, al
    ret
__mrfstr_avx512dq_equal endp

; void __mrfstr_avx512dq_tequal(
;     volatile mrfstr_bool_t *res,
;     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
;
; res = RES
; str1 = STR1+SIZE
; str2 = STR2+SIZE
; size = -SIZE

__mrfstr_avx512dq_tequal proc
    mov rax, [_mrfstr_cmp_load]

    cmp r9, rax
    jg LASTLOAD

    mov r10b, [rcx]
    test r10b, r10b
    jz RETURN

OLHEAD:
    mov r10, r9
    sub r10, rax

ILHEAD:
    vmovdqu64 zmm16, [rdx+r9]
    vpcmpq k1, zmm16, [r8+r9], 4

    kortestb k1, k1
    jnz FALSE

    add r9, 64
    cmp r9, r10
    jne ILHEAD

    mov r10b, [rcx]
    test r10b, r10b
    jz RETURN

    cmp r9, rax
    jl OLHEAD

LASTLOAD:
    vmovdqu64 zmm16, [rdx+r9]
    vpcmpq k1, zmm16, [r8+r9], 4

    kortestb k1, k1
    jnz FALSE

    add r9, 64
    jnz LASTLOAD

RETURN:
    ret

FALSE:
    mov byte ptr [rcx], 0
    ret
__mrfstr_avx512dq_tequal endp

; mrfstr_bool_t __mrfstr_avx512f_equal(
;     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
;
; str1 = STR1+SIZE
; str2 = STR2+SIZE
; size = -SIZE

__mrfstr_avx512f_equal proc
LHEAD:
    vmovdqu64 zmm16, [rcx+r8]
    vpcmpq k1, zmm16, [rdx+r8], 4

    kmovw eax, k1
    test al, al
    jnz FALSE

    add r8, 64
    jnz LHEAD

    mov al, 1
    ret

FALSE:
    xor al, al
    ret
__mrfstr_avx512f_equal endp

; void __mrfstr_avx512f_tequal(
;     volatile mrfstr_bool_t *res,
;     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
;
; res = RES
; str1 = STR1+SIZE
; str2 = STR2+SIZE
; size = -SIZE

__mrfstr_avx512f_tequal proc
    mov rax, [_mrfstr_cmp_load]

    cmp r9, rax
    jg LASTLOAD

    mov r10b, [rcx]
    test r10b, r10b
    jz RETURN

OLHEAD:
    mov r10, r9
    sub r10, rax

ILHEAD:
    vmovdqu64 zmm16, [rdx+r9]
    vpcmpq k1, zmm16, [r8+r9], 4

    kmovw r11d, k1
    test r11b, r11b
    jnz FALSE

    add r9, 64
    cmp r9, r10
    jne ILHEAD

    mov r10b, [rcx]
    test r10b, r10b
    jz RETURN

    cmp r9, rax
    jl OLHEAD

LASTLOAD:
    vmovdqu64 zmm16, [rdx+r9]
    vpcmpq k1, zmm16, [r8+r9], 4

    kmovw r11d, k1
    test r11b, r11b
    jnz FALSE

    add r9, 64
    jnz LASTLOAD

RETURN:
    ret

FALSE:
    mov byte ptr [rcx], 0
    ret
__mrfstr_avx512f_tequal endp

; mrfstr_bool_t __mrfstr_avx2_equal(
;     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
;
; str1 = STR1+SIZE
; str2 = STR2+SIZE
; size = -SIZE

__mrfstr_avx2_equal proc
LHEAD:
    vmovdqu ymm0, ymmword ptr [rcx+r8]
    vpcmpeqq ymm1, ymm0, [rdx+r8]

    vpmovmskb eax, ymm1
    cmp eax, 0ffffffffh
    jne FALSE

    add r8, 32
    jnz LHEAD

    mov al, 1
    vzeroupper
    ret

FALSE:
    xor al, al
    vzeroupper
    ret
__mrfstr_avx2_equal endp

; void __mrfstr_avx2_tequal(
;     volatile mrfstr_bool_t *res,
;     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
;
; res = RES
; str1 = STR1+SIZE
; str2 = STR2+SIZE
; size = -SIZE

__mrfstr_avx2_tequal proc
    mov rax, [_mrfstr_cmp_load]

    cmp r9, rax
    jg LASTLOAD

    mov r10b, [rcx]
    test r10b, r10b
    jz RETURN

OLHEAD:
    mov r10, r9
    sub r10, rax

ILHEAD:
    vmovdqu ymm0, ymmword ptr [rdx+r9]
    vpcmpeqq ymm1, ymm0, [r8+r9]

    vpmovmskb r11d, ymm1
    cmp r11d, 0ffffffffh
    jne FALSE

    add r9, 32
    cmp r9, r10
    jne ILHEAD

    mov r10b, [rcx]
    test r10b, r10b
    jz RETURN

    cmp r9, rax
    jl OLHEAD

LASTLOAD:
    vmovdqu ymm0, ymmword ptr [rdx+r9]
    vpcmpeqq ymm1, ymm0, [r8+r9]

    vpmovmskb r11d, ymm1
    cmp r11d, 0ffffffffh
    jne FALSE

    add r9, 32
    jnz LASTLOAD

RETURN:
    vzeroupper
    ret

FALSE:
    mov byte ptr [rcx], 0
    vzeroupper
    ret
__mrfstr_avx2_tequal endp

; mrfstr_bool_t __mrfstr_sse2_equal(
;     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
;
; str1 = STR1+SIZE
; str2 = STR2+SIZE
; size = -SIZE

__mrfstr_sse2_equal proc
LHEAD:
    movdqu xmm0, [rcx+r8]
    pcmpeqd xmm0, [rdx+r8]

    pmovmskb eax, xmm0
    cmp ax, 0ffffh
    jne FALSE

    add r8, 16
    jnz LHEAD

    mov al, 1
    ret

FALSE:
    xor al, al
    ret
__mrfstr_sse2_equal endp

; void __mrfstr_sse2_tequal(
;     volatile mrfstr_bool_t *res,
;     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
;
; res = RES
; str1 = STR1+SIZE
; str2 = STR2+SIZE
; size = -SIZE

__mrfstr_sse2_tequal proc
    mov rax, [_mrfstr_cmp_load]

    cmp r9, rax
    jg LASTLOAD

    mov r10b, [rcx]
    test r10b, r10b
    jz RETURN

OLHEAD:
    mov r10, r9
    sub r10, rax

ILHEAD:
    movdqu xmm0, [rdx+r9]
    pcmpeqd xmm0, [r8+r9]

    pmovmskb r11d, xmm0
    cmp r11w, 0ffffh
    jne FALSE

    add r9, 16
    cmp r9, r10
    jne ILHEAD

    mov r10b, [rcx]
    test r10b, r10b
    jz RETURN

    cmp r9, rax
    jl OLHEAD

LASTLOAD:
    movdqu xmm0, [rdx+r9]
    pcmpeqd xmm0, [r8+r9]

    pmovmskb r11d, xmm0
    cmp r11w, 0ffffh
    jne FALSE

    add r9, 16
    jnz LASTLOAD

RETURN:
    ret

FALSE:
    mov byte ptr [rcx], 0
    ret
__mrfstr_sse2_tequal endp

; mrfstr_bool_t __mrfstr_i64_equal(
;     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
;
; str1 = STR1+SIZE
; str2 = STR2+SIZE
; size = -SIZE

__mrfstr_i64_equal proc
LHEAD:
    mov rax, [rcx+r8]
    cmp rax, [rdx+r8]
    jne FALSE

    add r8, 8
    jnz LHEAD

    mov al, 1
    ret

FALSE:
    xor al, al
    ret
__mrfstr_i64_equal endp

; void __mrfstr_i64_tequal(
;     volatile mrfstr_bool_t *res,
;     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
;
; res = RES
; str1 = STR1+SIZE
; str2 = STR2+SIZE
; size = -SIZE

__mrfstr_i64_tequal proc
    mov rax, [_mrfstr_cmp_load]

    cmp r9, rax
    jg LASTLOAD

    mov r10b, [rcx]
    test r10b, r10b
    jz RETURN

OLHEAD:
    mov r10, r9
    sub r10, rax

ILHEAD:
    mov r11, [rdx+r9]
    cmp r11, [r8+r9]
    jne FALSE

    add r9, 8
    cmp r9, r10
    jne ILHEAD

    mov r10b, [rcx]
    test r10b, r10b
    jz RETURN

    cmp r9, rax
    jl OLHEAD

LASTLOAD:
    mov r11, [rdx+r9]
    cmp r11, [r8+r9]
    jne FALSE

    add r9, 8
    jnz LASTLOAD

RETURN:
    ret

FALSE:
    mov byte ptr [rcx], 0
    ret
__mrfstr_i64_tequal endp

end
