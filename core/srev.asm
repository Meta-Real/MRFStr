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

simdd segment read align(64)
align 64
    _avx512vbmi dq 0001020304050607h, 08090a0b0c0d0e0fh,
        1011121314151617h, 18191a1b1c1d1e1fh,
        2021222324252627h, 28292a2b2c2d2e2fh,
        3031323334353637h, 38393a3b3c3d3e3fh
    _avx512bw1 dq 0001020304050607h, 08090a0b0c0d0e0fh,
        0001020304050607h, 08090a0b0c0d0e0fh
    _avx2 dq 0001020304050607h, 08090a0b0c0d0e0fh
    _ssse3 dd 00010203h, 04050607h, 08090a0bh, 0c0d0e0fh
    _avx512bw2 dq 1, 0, 3, 2, 5, 4, 7, 6
simdd ends

.code

; void __mrfstr_avx512vbmi_rev(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx512vbmi_rev proc
    mov rax, r8
    neg rax

    sub rcx, 64
    vmovdqa64 zmm18, [_avx512vbmi]

LHEAD:
    vmovdqu64 zmm16, [rdx+rax]
    vmovdqa64 zmm17, [rcx+r8]

    vpermb zmm16, zmm18, zmm16
    vpermb zmm17, zmm18, zmm17

    vmovdqu64 [rdx+rax], zmm17
    vmovdqa64 [rcx+r8], zmm16

    add rax, 64
    sub r8, 64
    jnz LHEAD

    ret
__mrfstr_avx512vbmi_rev endp

; void __mrfstr_avx512vbmi_rev2(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx512vbmi_rev2 proc
    mov rax, r8
    neg rax

    sub rcx, 64
    vmovdqa64 zmm17, [_avx512vbmi]

LHEAD:
    vmovdqu64 zmm16, [rdx+rax]
    vpermb zmm16, zmm17, zmm16
    vmovdqa64 [rcx+r8], zmm16

    add rax, 64
    sub r8, 64
    jnz LHEAD

    ret
__mrfstr_avx512vbmi_rev2 endp

; void __mrfstr_avx512vbmi_ntrev2(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx512vbmi_ntrev2 proc
    mov rax, r8
    neg rax

    sub rcx, 64
    vmovdqa64 zmm17, [_avx512vbmi]

LHEAD:
    vmovdqu64 zmm16, [rdx+rax]
    vpermb zmm16, zmm17, zmm16
    vmovntdq zmmword ptr [rcx+r8], zmm16

    add rax, 64
    sub r8, 64
    jnz LHEAD

    sfence
    ret
__mrfstr_avx512vbmi_ntrev2 endp

; void __mrfstr_avx512bw_rev(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx512bw_rev proc
    mov rax, r8
    neg rax

    sub rcx, 64
    vmovdqa64 zmm18, [_avx512bw1]
    vmovdqa64 zmm19, [_avx512bw2]

LHEAD:
    vmovdqu64 zmm16, [rdx+rax]
    vmovdqa64 zmm17, [rcx+r8]

    vpshufb zmm16, zmm16, zmm18
    vpermq zmm16, zmm19, zmm16
    vpshufb zmm17, zmm17, zmm18
    vpermq zmm17, zmm19, zmm17

    vmovdqu64 [rdx+rax], zmm17
    vmovdqa64 [rcx+r8], zmm16

    add rax, 64
    sub r8, 64
    jnz LHEAD

    ret
__mrfstr_avx512bw_rev endp

; void __mrfstr_avx512bw_rev2(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx512bw_rev2 proc
    mov rax, r8
    neg rax

    sub rcx, 64
    vmovdqa64 zmm17, [_avx512bw1]
    vmovdqa64 zmm18, [_avx512bw2]

LHEAD:
    vmovdqu64 zmm16, [rdx+rax]

    vpshufb zmm16, zmm16, zmm17
    vpermq zmm16, zmm18, zmm16

    vmovdqa64 [rcx+r8], zmm16

    add rax, 64
    sub r8, 64
    jnz LHEAD

    ret
__mrfstr_avx512bw_rev2 endp

; void __mrfstr_avx512bw_ntrev2(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx512bw_ntrev2 proc
    mov rax, r8
    neg rax

    sub rcx, 64
    vmovdqa64 zmm17, [_avx512bw1]
    vmovdqa64 zmm18, [_avx512bw2]

LHEAD:
    vmovdqu64 zmm16, [rdx+rax]

    vpshufb zmm16, zmm16, zmm17
    vpermq zmm16, zmm18, zmm16

    vmovntdq zmmword ptr [rcx+r8], zmm16

    add rax, 64
    sub r8, 64
    jnz LHEAD

    sfence
    ret
__mrfstr_avx512bw_ntrev2 endp

; void __mrfstr_avx2_rev(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx2_rev proc
    mov rax, r8
    neg rax

    sub rcx, 32
    vmovdqa ymm2, ymmword ptr [_avx2]

LHEAD:
    vmovdqu ymm0, ymmword ptr [rdx+rax]
    vmovdqa ymm1, ymmword ptr [rcx+r8]

    vpshufb ymm0, ymm0, ymm2
    vperm2i128 ymm0, ymm0, ymm0, 1
    vpshufb ymm1, ymm1, ymm2
    vperm2i128 ymm1, ymm1, ymm1, 1

    vmovdqu ymmword ptr [rdx+rax], ymm1
    vmovdqa ymmword ptr [rcx+r8], ymm0

    add rax, 32
    sub r8, 32
    jnz LHEAD

    vzeroupper
    ret
__mrfstr_avx2_rev endp

; void __mrfstr_avx2_rev2(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx2_rev2 proc
    mov rax, r8
    neg rax

    sub rcx, 32
    vmovdqa ymm1, ymmword ptr [_avx2]

LHEAD:
    vmovdqu ymm0, ymmword ptr [rdx+rax]

    vpshufb ymm0, ymm0, ymm1
    vperm2i128 ymm0, ymm0, ymm0, 1

    vmovdqa ymmword ptr [rcx+r8], ymm0

    add rax, 32
    sub r8, 32
    jnz LHEAD

    vzeroupper
    ret
__mrfstr_avx2_rev2 endp

; void __mrfstr_avx2_ntrev2(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx2_ntrev2 proc
    mov rax, r8
    neg rax

    sub rcx, 32
    vmovdqa ymm1, ymmword ptr [_avx2]

LHEAD:
    vmovdqu ymm0, ymmword ptr [rdx+rax]

    vpshufb ymm0, ymm0, ymm1
    vperm2i128 ymm0, ymm0, ymm0, 1

    vmovntdq ymmword ptr [rcx+r8], ymm0

    add rax, 32
    sub r8, 32
    jnz LHEAD

    sfence
    vzeroupper
    ret
__mrfstr_avx2_ntrev2 endp

; void __mrfstr_ssse3_rev(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_ssse3_rev proc
    mov rax, r8
    neg rax

    sub rcx, 16
    movdqa xmm2, xmmword ptr [_ssse3]

LHEAD:
    movdqu xmm0, xmmword ptr [rdx+rax]
    movdqa xmm1, xmmword ptr [rcx+r8]

    pshufb xmm0, xmm2
    pshufb xmm1, xmm2

    movdqu xmmword ptr [rdx+rax], xmm1
    movdqa xmmword ptr [rcx+r8], xmm0

    add rax, 16
    sub r8, 16
    jnz LHEAD

    ret
__mrfstr_ssse3_rev endp

; void __mrfstr_ssse3_rev2(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_ssse3_rev2 proc
    mov rax, r8
    neg rax

    sub rcx, 16
    movdqa xmm1, xmmword ptr [_ssse3]

LHEAD:
    movdqu xmm0, xmmword ptr [rdx+rax]
    pshufb xmm0, xmm1
    movdqa xmmword ptr [rcx+r8], xmm0

    add rax, 16
    sub r8, 16
    jnz LHEAD

    ret
__mrfstr_ssse3_rev2 endp

; void __mrfstr_ssse3_ntrev2(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_ssse3_ntrev2 proc
    mov rax, r8
    neg rax

    sub rcx, 16
    movdqa xmm1, xmmword ptr [_ssse3]

LHEAD:
    movdqu xmm0, xmmword ptr [rdx+rax]
    pshufb xmm0, xmm1
    movntdq xmmword ptr [rcx+r8], xmm0

    add rax, 16
    sub r8, 16
    jnz LHEAD

    sfence
    ret
__mrfstr_ssse3_ntrev2 endp

; void __mrfstr_i64_rev(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_i64_rev proc
    mov rax, r8
    neg rax

    sub rcx, 8

LHEAD:
    mov r9, [rdx+rax]
    mov r10, [rcx+r8]

    bswap r9
    bswap r10

    mov [rdx+rax], r8
    mov [rcx+r8], r9

    add rax, 8
    sub r8, 8
    jnz LHEAD

    ret
__mrfstr_i64_rev endp

; void __mrfstr_i64_rev2(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_i64_rev2 proc
    mov rax, r8
    neg rax

    sub rcx, 8

LHEAD:
    mov r9, [rdx+rax]
    bswap r9
    mov [rcx+r8], r9

    add rax, 8
    sub r8, 8
    jnz LHEAD

    ret
__mrfstr_i64_rev2 endp

end
