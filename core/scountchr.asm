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

; mrfstr_size_t __mrfstr_avx512bw_countchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx512bw_countchr proc
    vpbroadcastb zmm16, edx
    xor rax, rax

LHEAD:
    vpcmpeqb k0, zmm16, [rcx+r8]

    kmovq rdx, k0
    popcnt rdx, rdx
    lea rax, [rax+rdx]

    add r8, 64
    jnz LHEAD

    ret
__mrfstr_avx512bw_countchr endp

; mrfstr_size_t __mrfstr_avx2_countchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx2_countchr proc
    vpxor xmm1, xmm1, xmm1
    vmovd xmm0, edx
    vpshufb xmm0, xmm0, xmm1
    vinsertf128 ymm0, ymm0, xmm0, 1
    xor rax, rax

LHEAD:
    vpcmpeqb ymm1, ymm0, [rcx+r8]

    vpmovmskb edx, ymm1
    popcnt edx, edx
    lea rax, [rax+rdx]

    add r8, 32
    jnz LHEAD

    vzeroupper
    ret
__mrfstr_avx2_countchr endp

; mrfstr_size_t __mrfstr_sse42_countchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_sse42_countchr proc
    movd xmm1, edx
    punpcklbw xmm1, xmm1
    punpcklwd xmm1, xmm1
    pshufd xmm1, xmm1, 0
    xor rax, rax

LHEAD:
    movdqa xmm0, [rcx+r8]
    pcmpeqb xmm0, xmm1

    pmovmskb edx, xmm0
    popcnt dx, dx
    lea rax, [rax+rdx]

    add r8, 16
    jnz LHEAD

    ret
__mrfstr_sse42_countchr endp

; mrfstr_size_t __mrfstr_sse2_countchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_sse2_countchr proc
    movd xmm1, edx
    punpcklbw xmm1, xmm1
    punpcklwd xmm1, xmm1
    pshufd xmm1, xmm1, 0
    xor rax, rax

LHEAD:
    movdqa xmm0, [rcx+r8]
    pcmpeqb xmm0, xmm1

    pmovmskb edx, xmm0
    mov r9d, edx
    shr r9d, 1
    and r9d, 55555555h
    sub edx, r9d

    mov r9d, edx
    and r9d, 33333333h
    and edx, 33333333h
    lea edx, [edx+r9d]

    mov r9d, edx
    shr r9d, 4
    lea edx, [edx+r9d]
    and edx, 0f0f0f0fh

    imul edx, 01010101h
    shr edx, 24
    lea rax, [rax+rdx]

    add r8, 16
    jnz LHEAD

    ret
__mrfstr_sse2_countchr endp

; mrfstr_size_t __mrfstr_i64_sse42_countchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_i64_sse42_countchr proc
    mov rax, 0101010101010101h
    mov r11, 7f7f7f7f7f7f7f7fh

    movzx rdx, dl
    imul rdx, rax
    xor rax, rax

LHEAD:
    mov r9, [rcx+r8]
    xor r9, rdx

    mov r10, r9
    and r9, r11
    lea r9, [r9+r11]
    or r9, r10
    or r9, r11
    not r9

    popcnt r9, r9
    lea rax, [rax+r9]

    add r8, 8
    jnz LHEAD

    ret
__mrfstr_i64_sse42_countchr endp

; mrfstr_size_t __mrfstr_i64_countchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_i64_countchr proc
    push rbx
    push rbp
    push rsi
    push rdi

    mov r11, 7f7f7f7f7f7f7f7fh
    mov rbx, 0101010101010101h
    mov rbp, 5555555555555555h
    mov rsi, 3333333333333333h
    mov rdi, 0f0f0f0f0f0f0f0fh

    movzx rdx, dl
    imul rdx, rbx
    xor rax, rax

LHEAD:
    mov r9, [rcx+r8]
    xor r9, rdx

    mov r10, r9
    and r9, r11
    lea r9, [r9+r11]
    or r9, r10
    or r9, r11
    not r9

    mov r10, r9
    shr r10, 1
    and r10, rbp
    sub r9, r10

    mov r10, r9
    shr r10, 2
    and r10, rsi
    and r9, rsi
    lea r9, [r9+r10]

    mov r10, r9
    shr r10, 4
    lea r9, [r9+r10]
    and r9, rdi

    imul r9, rbx
    shr r9, 56
    lea rax, [rax+r9]

    add r8, 8
    jnz LHEAD

    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret
__mrfstr_i64_countchr endp

end
