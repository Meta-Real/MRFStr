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

.code

; void __mrfstr_avx512f_copy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST+SIZE
; src = SRC+SIZE
; size = -SIZE

__mrfstr_avx512f_copy proc
LHEAD:
    vmovdqu64 zmm16, [rcx+r8]
    vmovdqa64 [rdx+r8], zmm16

    add r8, 64
    jnz LHEAD

    ret
__mrfstr_avx512f_copy endp

; void __mrfstr_avx512f_ntcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST+SIZE
; src = SRC+SIZE
; size = -SIZE

__mrfstr_avx512f_ntcopy proc
LHEAD:
    vmovdqu64 zmm16, [rcx+r8]
    vmovntdq zmmword ptr [rdx+r8], zmm16

    add r8, 64
    jnz LHEAD

    sfence
    ret
__mrfstr_avx512f_ntcopy endp

; void __mrfstr_avx512f_rcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST
; src = SRC
; size = SIZE

__mrfstr_avx512f_rcopy proc
    sub rcx, 64
    sub rdx, 64

LHEAD:
    vmovdqu64 zmm16, [rcx+r8]
    vmovdqa64 [rdx+r8], zmm16

    sub r8, 64
    jnz LHEAD

    ret
__mrfstr_avx512f_rcopy endp

; void __mrfstr_avx512f_ntrcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST
; src = SRC
; size = SIZE

__mrfstr_avx512f_ntrcopy proc
    sub rcx, 64
    sub rdx, 64

LHEAD:
    vmovdqu64 zmm16, [rcx+r8]
    vmovntdq zmmword ptr [rdx+r8], zmm16

    sub r8, 64
    jnz LHEAD

    sfence
    ret
__mrfstr_avx512f_ntrcopy endp

; void __mrfstr_avx_copy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST+SIZE
; src = SRC+SIZE
; size = -SIZE

__mrfstr_avx_copy proc
LHEAD:
    vmovdqu ymm0, ymmword ptr [rcx+r8]
    vmovdqa ymmword ptr [rdx+r8], ymm0

    add r8, 32
    jnz LHEAD

    vzeroupper
    ret
__mrfstr_avx_copy endp

; void __mrfstr_avx_ntcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST+SIZE
; src = SRC+SIZE
; size = -SIZE

__mrfstr_avx_ntcopy proc
LHEAD:
    vmovdqu ymm0, ymmword ptr [rcx+r8]
    vmovntdq ymmword ptr [rdx+r8], ymm0

    add r8, 32
    jnz LHEAD

    sfence
    vzeroupper
    ret
__mrfstr_avx_ntcopy endp

; void __mrfstr_avx_rcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST
; src = SRC
; size = SIZE

__mrfstr_avx_rcopy proc
    sub rcx, 32
    sub rdx, 32

LHEAD:
    vmovdqu ymm0, ymmword ptr [rcx+r8]
    vmovdqa ymmword ptr [rdx+r8], ymm0

    sub r8, 32
    jnz LHEAD

    vzeroupper
    ret
__mrfstr_avx_rcopy endp

; void __mrfstr_avx_ntrcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST
; src = SRC
; size = SIZE

__mrfstr_avx_ntrcopy proc
    sub rcx, 32
    sub rdx, 32

LHEAD:
    vmovdqu ymm0, ymmword ptr [rcx+r8]
    vmovntdq ymmword ptr [rdx+r8], ymm0

    sub r8, 32
    jnz LHEAD

    sfence
    vzeroupper
    ret
__mrfstr_avx_ntrcopy endp

; void __mrfstr_sse2_copy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST+SIZE
; src = SRC+SIZE
; size = -SIZE

__mrfstr_sse2_copy proc
LHEAD:
    movdqu xmm0, xmmword ptr [rcx+r8]
    movdqa xmmword ptr [rdx+r8], xmm0

    add r8, 16
    jnz LHEAD

    ret
__mrfstr_sse2_copy endp

; void __mrfstr_sse2_ntcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST+SIZE
; src = SRC+SIZE
; size = -SIZE

__mrfstr_sse2_ntcopy proc
LHEAD:
    movdqu xmm0, xmmword ptr [rcx+r8]
    movntdq xmmword ptr [rdx+r8], xmm0

    add r8, 16
    jnz LHEAD

    sfence
    ret
__mrfstr_sse2_ntcopy endp

; void __mrfstr_sse2_rcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST
; src = SRC
; size = SIZE

__mrfstr_sse2_rcopy proc
    sub rcx, 16
    sub rdx, 16

LHEAD:
    movdqu xmm0, xmmword ptr [rcx+r8]
    movdqa xmmword ptr [rdx+r8], xmm0

    sub r8, 16
    jnz LHEAD

    ret
__mrfstr_sse2_rcopy endp

; void __mrfstr_sse2_ntrcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST
; src = SRC
; size = SIZE

__mrfstr_sse2_ntrcopy proc
    sub rcx, 16
    sub rdx, 16

LHEAD:
    movdqu xmm0, xmmword ptr [rcx+r8]
    movntdq xmmword ptr [rdx+r8], xmm0

    sub r8, 16
    jnz LHEAD

    sfence
    ret
__mrfstr_sse2_ntrcopy endp

; void __mrfstr_i64_copy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST+SIZE
; src = SRC+SIZE
; size = -SIZE

__mrfstr_i64_copy proc
LHEAD:
    mov rax, [rcx+r8]
    mov [rdx+r8], rax

    add r8, 32
    jnz LHEAD

    ret
__mrfstr_i64_copy endp

; void __mrfstr_i64_rcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)
;
; dst = DST
; src = SRC
; size = SIZE

__mrfstr_i64_rcopy proc
    sub rcx, 8
    sub rdx, 8

LHEAD:
    mov rax, [rcx+r8]
    mov [rdx+r8], rax

    sub r8, 8
    jnz LHEAD

    ret
__mrfstr_i64_rcopy endp

end
