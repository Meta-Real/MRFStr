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

; mrfstr_size_t __mrfstr_avx512bw_strlen(
;     mrfstr_ptr_ct str)
;
; str = STR

__mrfstr_avx512bw_strlen proc
    vpxorq zmm16, zmm16, zmm16
    xor rax, rax

LHEAD:
    vpcmpeqb k0, zmm16, [rcx+rax]
    add rax, 64

    kortestq k0, k0
    jz LHEAD

    kmovq rdx, k0
    bsf rdx, rdx
    lea rax, [rax+rdx-64]
    ret
__mrfstr_avx512bw_strlen endp

; mrfstr_size_t __mrfstr_avx2_strlen(
;     mrfstr_ptr_ct str)
;
; str = STR

__mrfstr_avx2_strlen proc
    vpxor ymm0, ymm0, ymm0
    xor rax, rax

LHEAD:
    vpcmpeqb ymm1, ymm0, [rcx+rax]
    add rax, 32

    vpmovmskb edx, ymm1
    test edx, edx
    jz LHEAD

    bsf edx, edx
    lea rax, [rax+rdx-32]

    vzeroupper
    ret
__mrfstr_avx2_strlen endp

; mrfstr_size_t __mrfstr_sse2_strlen(
;     mrfstr_ptr_ct str)
;
; str = STR

__mrfstr_sse2_strlen proc
    pxor xmm1, xmm1
    xor rax, rax

LHEAD:
    movdqa xmm0, [rcx+rax]
    pcmpeqb xmm0, xmm1
    add rax, 16

    pmovmskb edx, xmm0
    test edx, edx
    jz LHEAD

    bsf edx, edx
    lea rax, [rax+rdx-16]
    ret
__mrfstr_sse2_strlen endp

; mrfstr_size_t __mrfstr_i64_strlen(
;     mrfstr_ptr_ct str)
;
; str = STR

__mrfstr_i64_strlen proc
    mov r9, 0101010101010101h
    mov r10, 8080808080808080h
    mov r11, 7f7f7f7f7f7f7f7fh
    xor rax, rax

LHEAD:
    mov rdx, [rcx+rax]
    mov r8, rdx
    add rax, 8

    sub rdx, r9
    not r8
    and rdx, r8
    and rdx, r10

    test rdx, rdx
    jz LHEAD

    not r8
    mov rdx, r8

    and rdx, r11
    lea rdx, [rdx+r11]
    or rdx, r8
    or rdx, r11
    not rdx

    bsf rdx, rdx
    shr rdx, 3
    lea rax, [rax+rdx]
    ret
__mrfstr_i64_strlen endp

end
