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

    extern _mrfstr_mem_ntlimit : dq

.code

; void __mrfstr_avx512f_fill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx512f_fill proc
    vmovd xmm16, edx
    vpbroadcastb ymm16, xmm16
    vinserti64x4 zmm16, zmm16, ymm16, 1

    cmp r8, [_mrfstr_mem_ntlimit]
    jb NTLHEAD

LHEAD:
    vmovdqa64 [rcx+r8], zmm16

    add r8, 64
    jnz LHEAD

    ret

NTLHEAD:
    vmovntdq zmmword ptr [rcx+r8], zmm16

    add r8, 64
    jnz NTLHEAD

    sfence
    ret
__mrfstr_avx512f_fill endp

; void __mrfstr_avx512f_vfill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx512f_vfill proc
    vmovd xmm16, edx
    vpbroadcastb ymm16, xmm16
    vinserti64x4 zmm16, zmm16, ymm16, 1

LHEAD:
    vmovdqa64 [rcx+r8], zmm16

    add r8, 64
    jnz LHEAD

    ret
__mrfstr_avx512f_vfill endp

; void __mrfstr_avx512f_ntfill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx512f_ntfill proc
    vmovd xmm16, edx
    vpbroadcastb ymm16, xmm16
    vinserti64x4 zmm16, zmm16, ymm16, 1

LHEAD:
    vmovntdq zmmword ptr [rcx+r8], zmm16

    add r8, 64
    jnz LHEAD

    sfence
    ret
__mrfstr_avx512f_ntfill endp

; void __mrfstr_avx_fill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx_fill proc
    vpxor xmm1, xmm1, xmm1
    vmovd xmm0, edx
    vpshufb xmm0, xmm0, xmm1
    vinsertf128 ymm0, ymm0, xmm0, 1

    cmp r8, [_mrfstr_mem_ntlimit]
    jb NTLHEAD

LHEAD:
    vmovdqa ymmword ptr [rcx+r8], ymm0

    add r8, 32
    jnz LHEAD

    vzeroupper
    ret

NTLHEAD:
    vmovntdq ymmword ptr [rcx+r8], ymm0

    add r8, 32
    jnz NTLHEAD

    sfence
    vzeroupper
    ret
__mrfstr_avx_fill endp

; void __mrfstr_avx_vfill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx_vfill proc
    vpxor xmm1, xmm1, xmm1
    vmovd xmm0, edx
    vpshufb xmm0, xmm0, xmm1
    vinsertf128 ymm0, ymm0, xmm0, 1

LHEAD:
    vmovdqa ymmword ptr [rcx+r8], ymm0

    add r8, 32
    jnz LHEAD

    vzeroupper
    ret
__mrfstr_avx_vfill endp

; void __mrfstr_avx_ntfill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx_ntfill proc
    vpxor xmm1, xmm1, xmm1
    vmovd xmm0, edx
    vpshufb xmm0, xmm0, xmm1
    vinsertf128 ymm0, ymm0, xmm0, 1

LHEAD:
    vmovntdq ymmword ptr [rcx+r8], ymm0

    add r8, 32
    jnz LHEAD

    sfence
    vzeroupper
    ret
__mrfstr_avx_ntfill endp

; void __mrfstr_sse2_fill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_sse2_fill proc
    movd xmm0, edx
    punpcklbw xmm0, xmm0
    punpcklwd xmm0, xmm0
    pshufd xmm0, xmm0, 0

    cmp r8, [_mrfstr_mem_ntlimit]
    jb NTLHEAD

LHEAD:
    movdqa [rcx+r8], xmm0

    add r8, 16
    jnz LHEAD

    ret

NTLHEAD:
    movntdq [rcx+r8], xmm0

    add r8, 16
    jnz NTLHEAD

    sfence
    ret
__mrfstr_sse2_fill endp

; void __mrfstr_sse2_vfill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_sse2_vfill proc
    movd xmm0, edx
    punpcklbw xmm0, xmm0
    punpcklwd xmm0, xmm0
    pshufd xmm0, xmm0, 0

LHEAD:
    movdqa [rcx+r8], xmm0

    add r8, 16
    jnz LHEAD

    ret
__mrfstr_sse2_vfill endp

; void __mrfstr_sse2_ntfill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_sse2_ntfill proc
    movd xmm0, edx
    punpcklbw xmm0, xmm0
    punpcklwd xmm0, xmm0
    pshufd xmm0, xmm0, 0

LHEAD:
    movntdq [rcx+r8], xmm0

    add r8, 16
    jnz LHEAD

    sfence
    ret
__mrfstr_sse2_ntfill endp

; void __mrfstr_i64_fill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_i64_fill proc
    mov rax, 0101010101010101h
    movzx rdx, dl
    imul rdx, rax

LHEAD:
    mov [rcx+r8], rdx

    add r8, 8
    jnz LHEAD

    ret
__mrfstr_i64_fill endp

end
