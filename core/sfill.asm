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
    extern _mrfstr_mem_ntlimit : dq

.code

; void __mrfstr_avx512_fill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; size = SIZE

__mrfstr_avx512f_fill proc
    imul edx, 01010101h
    vpbroadcastd zmm16, edx

    cmp r8, [_mrfstr_mem_ntlimit]
    ja NTSTORE

    neg r8

LHEAD:
    vmovdqa64 [rcx+r8], zmm16

    add r8, 64
    jnz LHEAD

    ret

NTSTORE:
    neg r8

NTLHEAD:
    vmovntdq zmmword ptr [rcx+r8], zmm16

    add r8, 64
    jnz LHEAD

    sfence
    ret
__mrfstr_avx512f_fill endp

; void __mrfstr_avx512f_ntfill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; size = SIZE

__mrfstr_avx512f_ntfill proc
    imul edx, 01010101h
    vpbroadcastd zmm16, edx
    neg r8

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
; size = SIZE

__mrfstr_avx_fill proc
    imul edx, 01010101h
    movd xmm0, edx
    pshufd xmm0, xmm0, 0
    vinsertf128 ymm0, ymm0, xmm0, 1

    cmp r8, [_mrfstr_mem_ntlimit]
    ja NTSTORE

    neg r8

LHEAD:
    vmovdqa ymmword ptr [rcx+r8], ymm0

    add r8, 32
    jnz LHEAD

    vzeroupper
    ret

NTSTORE:
    neg r8

NTLHEAD:
    vmovntdq ymmword ptr [rcx+r8], ymm0

    add r8, 32
    jnz NTLHEAD

    sfence
    vzeroupper
    ret
__mrfstr_avx_fill endp

; void __mrfstr_avx_ntfill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; size = SIZE

__mrfstr_avx_ntfill proc
    imul edx, 01010101h
    movd xmm0, edx
    pshufd xmm0, xmm0, 0
    vinsertf128 ymm0, ymm0, xmm0, 1
    neg r8

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
; size = SIZE

__mrfstr_sse2_fill proc
    imul edx, 01010101h
    movd xmm0, edx
    pshufd xmm0, xmm0, 0

    cmp r8, [_mrfstr_mem_ntlimit]
    ja NTSTORE

    neg r8

LHEAD:
    movdqa [rcx+r8], xmm0

    add r8, 16
    jnz LHEAD

    ret

NTSTORE:
    neg r8

NTLHEAD:
    movntdq [rcx+r8], xmm0

    add r8, 16
    jnz NTLHEAD

    sfence
    ret
__mrfstr_sse2_fill endp

; void __mrfstr_sse2_ntfill(
;     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
;
; dst = DST+SIZE
; size = SIZE

__mrfstr_sse2_ntfill proc
    imul edx, 01010101h
    movd xmm0, edx
    pshufd xmm0, xmm0, 0
    neg r8

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
; size = SIZE

__mrfstr_i64_fill proc
    imul edx, 01010101h
    mov eax, edx
    shl rdx, 32
    or rdx, rax
    neg r8

LHEAD:
    mov [rcx+r8], rdx

    add r8, 8
    jnz LHEAD

    ret
__mrfstr_i64_fill endp

end
