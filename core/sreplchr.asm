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

; void __mrfstr_avx512bw_replchr(
;     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_avx512bw_replchr proc
    vpbroadcastb zmm16, edx
    vpbroadcastb zmm17, r8d

LHEAD:
    vpcmpeqb k0, zmm16, [rcx+r9]
    vmovdqu8 [rcx+r9]{k0}, zmm17

    add r9, 64
    jnz LHEAD

    ret
__mrfstr_avx512bw_replchr endp

; void __mrfstr_avx512bw_replchr2(
;     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; res = RES+SIZE
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_avx512bw_replchr2 proc
    vpbroadcastb zmm17, r8d
    vpbroadcastb zmm18, r9d
    mov rax, [rsp+40]

LHEAD:
    vmovdqu64 zmm16, [rdx+rax]

    vpcmpeqb k0, zmm16, zmm17
    vpblendmb zmm16{k0}, zmm16, zmm18

    vmovdqa64 [rcx+rax], zmm16

    add rax, 64
    jnz LHEAD

    ret
__mrfstr_avx512bw_replchr2 endp

; void __mrfstr_avx2_avx512bw_replchr(
;     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_avx2_avx512bw_replchr proc
    vpbroadcastb ymm16, edx
    vpbroadcastb ymm17, r8d

LHEAD:
    vpcmpeqb k0, ymm16, [rcx+r9]
    vmovdqu8 [rcx+r9]{k0}, ymm17

    add r9, 32
    jnz LHEAD

    ret
__mrfstr_avx2_avx512bw_replchr endp

; void __mrfstr_avx2_avx512bw_replchr2(
;     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; res = RES+SIZE
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_avx2_avx512bw_replchr2 proc
    vpbroadcastb ymm17, r8d
    vpbroadcastb ymm18, r9d
    mov rax, [rsp+40]

LHEAD:
    vmovdqu64 ymm16, [rdx+rax]

    vpcmpeqb k0, ymm16, ymm17
    vpblendmb ymm16{k0}, ymm16, ymm18

    vmovdqa64 [rcx+rax], ymm16

    add rax, 32
    jnz LHEAD

    ret
__mrfstr_avx2_avx512bw_replchr2 endp

; void __mrfstr_avx2_replchr(
;     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_avx2_replchr proc
    vpxor xmm3, xmm3, xmm3

    vmovd xmm1, edx
    vpshufb xmm1, xmm1, xmm3
    vinsertf128 ymm1, ymm1, xmm1, 1

    vmovd xmm2, r8d
    vpshufb xmm2, xmm2, xmm3
    vinsertf128 ymm2, ymm2, xmm2, 1

LHEAD:
    vmovdqa ymm0, ymmword ptr [rcx+r9]

    vpcmpeqb ymm3, ymm0, ymm1
    vpblendvb ymm0, ymm0, ymm2, ymm3

    vmovdqa ymmword ptr [rcx+r9], ymm0

    add r9, 32
    jnz LHEAD

    vzeroupper
    ret
__mrfstr_avx2_replchr endp

; void __mrfstr_avx2_replchr2(
;     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; res = RES+SIZE
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_avx2_replchr2 proc
    vpxor xmm3, xmm3, xmm3
    mov rax, [rsp+40]

    vmovd xmm1, edx
    vpshufb xmm1, xmm1, xmm3
    vinsertf128 ymm1, ymm1, xmm1, 1

    vmovd xmm2, r8d
    vpshufb xmm2, xmm2, xmm3
    vinsertf128 ymm2, ymm2, xmm2, 1

LHEAD:
    vmovdqu ymm0, ymmword ptr [rdx+rax]

    vpcmpeqb ymm3, ymm0, ymm1
    vpblendvb ymm0, ymm0, ymm2, ymm3

    vmovdqa ymmword ptr [rcx+rax], ymm0

    add rax, 32
    jnz LHEAD

    vzeroupper
    ret
__mrfstr_avx2_replchr2 endp

; void __mrfstr_sse41_avx512bw_replchr(
;     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_sse41_avx512bw_replchr proc
    vpbroadcastb xmm0, edx
    vpbroadcastb xmm1, r8d

LHEAD:
    vpcmpeqb k0, xmm0, [rcx+r9]
    vmovdqu8 [rcx+r9]{k0}, xmm1

    add r9, 16
    jnz LHEAD

    ret
__mrfstr_sse41_avx512bw_replchr endp

; void __mrfstr_sse41_avx512bw_replchr2(
;     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; res = RES+SIZE
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_sse41_avx512bw_replchr2 proc
    vpbroadcastb xmm1, r8d
    vpbroadcastb xmm2, r9d
    mov rax, [rsp+40]

LHEAD:
    vmovdqu64 xmm0, [rdx+rax]

    vpcmpeqb k0, xmm0, xmm1
    vpblendmb xmm0{k0}, xmm0, xmm2

    vmovdqa64 [rcx+rax], xmm0

    add rax, 16
    jnz LHEAD

    ret
__mrfstr_sse41_avx512bw_replchr2 endp

; void __mrfstr_sse41_replchr(
;     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_sse41_replchr proc
    movd xmm2, edx
    punpcklbw xmm2, xmm2
    punpcklwd xmm2, xmm2
    pshufd xmm2, xmm2, 0

    movd xmm3, r8d
    punpcklbw xmm3, xmm3
    punpcklwd xmm3, xmm3
    pshufd xmm3, xmm3, 0

LHEAD:
    movdqa xmm1, [rcx+r9]
    movdqa xmm0, xmm1

    pcmpeqb xmm0, xmm2
    pblendvb xmm1, xmm3, xmm0

    movdqa [rcx+r9], xmm1

    add r9, 16
    jnz LHEAD

    ret
__mrfstr_sse41_replchr endp

; void __mrfstr_sse41_replchr2(
;     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; res = RES+SIZE
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_sse41_replchr2 proc
    movd xmm2, r8d
    punpcklbw xmm2, xmm2
    punpcklwd xmm2, xmm2
    pshufd xmm2, xmm2, 0

    movd xmm3, r9d
    punpcklbw xmm3, xmm3
    punpcklwd xmm3, xmm3
    pshufd xmm3, xmm3, 0

    mov rax, [rsp+40]

LHEAD:
    movdqu xmm1, [rdx+rax]
    movdqa xmm0, xmm1

    pcmpeqb xmm0, xmm2
    pblendvb xmm1, xmm3, xmm0

    movdqa [rcx+rax], xmm1

    add rax, 16
    jnz LHEAD

    ret
__mrfstr_sse41_replchr2 endp

; void __mrfstr_i64_replchr(
;     mrfstr_ptr_t str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_i64_replchr proc
    push rbx
    mov rbx, 7f7f7f7f7f7f7f7fh

    mov rax, 0101010101010101h
    movzx r11, dl
    imul r11, rax
    movzx r8, r8b

LHEAD:
    mov r10, [rcx+r9]
    mov rdx, r10

    xor rdx, r11
    mov rax, rdx

    and rax, rbx
    lea rax, [rax+rbx]
    or rax, rdx
    or rax, rbx
    not rax
    shr rax, 7

    mov rdx, rax
    imul rdx, 255
    not rdx
    and r10, rdx

    imul rax, r8
    or r10, rax
    mov [rcx+r9], r10

    add r9, 8
    jnz LHEAD

    pop rbx
    ret
__mrfstr_i64_replchr endp

; void __mrfstr_i64_replchr2(
;     mrfstr_ptr_t res, mrfstr_ptr_ct str, mrfstr_chr_t ochr, mrfstr_chr_t nchr, mrfstr_size_t size)
;
; res = RES+SIZE
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_i64_replchr2 proc
    push rbx
    push rbp
    mov rbx, 7f7f7f7f7f7f7f7fh
    mov rbp, [rsp+56]

    mov rax, 0101010101010101h
    movzx r8, r8b
    imul r8, rax
    movzx r9, r9b

LHEAD:
    mov r10, [rdx+rbp]
    mov r11, r10

    xor r11, r8
    mov rax, r11

    and rax, rbx
    lea rax, [rax+rbx]
    or rax, r11
    or rax, rbx
    not rax
    shr rax, 7

    mov r11, rax
    imul r11, 255
    not r11
    and r10, r11

    imul rax, r9
    or r10, rax
    mov [rcx+rbp], r10

    add rbp, 8
    jnz LHEAD

    pop rbp
    pop rbx
    ret
__mrfstr_i64_replchr2 endp

end
