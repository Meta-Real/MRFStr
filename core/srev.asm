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
    _avx512vbmi dq 38393a3b3c3d3e3fh, 3031323334353637h,
        28292a2b2c2d2e2fh, 2021222324252627h
    _avx2_avx512vbmi dq 18191a1b1c1d1e1fh, 1011121314151617h,
        08090a0b0c0d0e0fh, 0001020304050607h
    _avx512bw1 dq 08090a0b0c0d0e0fh, 0001020304050607h,
        08090a0b0c0d0e0fh, 0001020304050607h
    _avx2 dq 08090a0b0c0d0e0fh, 0001020304050607h
    _ssse3 dq 08090a0b0c0d0e0fh, 0001020304050607h
    _avx512bw2 dq 6, 7, 4, 5, 2, 3, 0, 1
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
    vpermb zmm16, zmm18, [rdx+rax]
    vpermb zmm17, zmm18, [rcx+r8]

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
    vpermb zmm16, zmm17, [rdx+rax]
    vmovdqa64 [rcx+r8], zmm16

    add rax, 64
    sub r8, 64
    jnz LHEAD

    ret
__mrfstr_avx512vbmi_rev2 endp

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
    vpshufb zmm17, zmm17, zmm18
    vpermq zmm16, zmm19, zmm16
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

; void __mrfstr_avx2_avx512vbmi_rev(
;     mrfstr_ptr_t left, mrfstr_ptr_t right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx2_avx512vbmi_rev proc
    mov rax, r8
    neg rax
    sub rcx, 32
    vmovdqa64 ymm18, [_avx2_avx512vbmi]

LHEAD:
    vpermb ymm16, ymm18, [rdx+rax]
    vpermb ymm17, ymm18, [rcx+r8]

    vmovdqu64 [rdx+rax], ymm17
    vmovdqa64 [rcx+r8], ymm16

    add rax, 32
    sub r8, 32
    jnz LHEAD

    ret
__mrfstr_avx2_avx512vbmi_rev endp

; void __mrfstr_avx2_avx512vbmi_rev2(
;     mrfstr_ptr_t left, mrfstr_ptr_ct right, mrfstr_size_t size)
;
; left = LEFT
; right = RIGHT
; size = SIZE

__mrfstr_avx2_avx512vbmi_rev2 proc
    mov rax, r8
    neg rax
    sub rcx, 32
    vmovdqa64 ymm17, [_avx2_avx512vbmi]

LHEAD:
    vpermb ymm16, ymm17, [rdx+rax]
    vmovdqa64 [rcx+r8], ymm16

    add rax, 32
    sub r8, 32
    jnz LHEAD

    ret
__mrfstr_avx2_avx512vbmi_rev2 endp

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
    vpshufb ymm1, ymm1, ymm2
    vperm2i128 ymm0, ymm0, ymm0, 1
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
    movdqu xmm0, [rdx+rax]
    movdqa xmm1, [rcx+r8]

    pshufb xmm0, xmm2
    pshufb xmm1, xmm2

    movdqu [rdx+rax], xmm1
    movdqa [rcx+r8], xmm0

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
    movdqu xmm0, [rdx+rax]
    pshufb xmm0, xmm1
    movdqa [rcx+r8], xmm0

    add rax, 16
    sub r8, 16
    jnz LHEAD

    ret
__mrfstr_ssse3_rev2 endp

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

    mov [rdx+rax], r10
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
