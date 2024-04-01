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

    extern _mrfstr_searchchr_load : dq

.code

; mrfstr_bool_t __mrfstr_avx512bw_contchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx512bw_contchr proc
    vpbroadcastb zmm16, edx

LHEAD:
    vpcmpeqb k0, zmm16, [rcx+r8]

    kortestq k0, k0
    jnz TRUE

    add r8, 64
    jnz LHEAD

    xor al, al
    ret

TRUE:
    mov al, 1
    ret
__mrfstr_avx512bw_contchr endp

; void __mrfstr_avx512bw_tcontchr(
;     volatile mrfstr_bool_t *res,
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; res = RES
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx512bw_tcontchr proc
    mov r10b, [rcx]
    test r10b, r10b
    jnz RETURN

    vpbroadcastb zmm16, r8d
    mov rax, [_mrfstr_searchchr_load]
    cmp r9, rax
    jae LASTLOAD

OLHEAD:
    mov r10, r9
    sub r10, rax

ILHEAD:
    vpcmpeqb k0, zmm16, [rdx+r9]

    kortestq k0, k0
    jnz TRUE

    add r9, 64
    cmp r9, r10
    jne ILHEAD

    mov r10b, [rcx]
    test r10b, r10b
    jnz RETURN

    cmp r9, rax
    jb OLHEAD

LASTLOAD:
    vpcmpeqb k0, zmm16, [rdx+r9]

    kortestq k0, k0
    jnz TRUE

    add r9, 64
    jnz LASTLOAD

RETURN:
    ret

TRUE:
    mov byte ptr [rcx], 1
    ret
__mrfstr_avx512bw_tcontchr endp

; mrfstr_bool_t __mrfstr_avx2_contchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx2_contchr proc
    vpxor xmm1, xmm1, xmm1
    vmovd xmm0, edx
    vpshufb xmm0, xmm0, xmm1
    vinsertf128 ymm0, ymm0, xmm0, 1

LHEAD:
    vpcmpeqb ymm1, ymm0, [rcx+r8]

    vpmovmskb eax, ymm1
    test eax, eax
    jnz TRUE

    add r8, 32
    jnz LHEAD

    xor al, al
    vzeroupper
    ret

TRUE:
    mov al, 1
    vzeroupper
    ret
__mrfstr_avx2_contchr endp

; void __mrfstr_avx2_tcontchr(
;     volatile mrfstr_bool_t *res,
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; res = RES
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx2_tcontchr proc
    mov r10b, [rcx]
    test r10b, r10b
    jnz RETURN

    vpxor xmm1, xmm1, xmm1
    vmovd xmm0, r8d
    vpshufb xmm0, xmm0, xmm1
    vinsertf128 ymm0, ymm0, xmm0, 1

    mov rax, [_mrfstr_searchchr_load]
    cmp r9, rax
    jae LASTLOAD

OLHEAD:
    mov r10, r9
    sub r10, rax

ILHEAD:
    vpcmpeqb ymm1, ymm0, [rdx+r9]

    vpmovmskb r11d, ymm1
    test r11d, r11d
    jnz TRUE

    add r9, 32
    cmp r9, r10
    jne ILHEAD

    mov r10b, [rcx]
    test r10b, r10b
    jnz RETURN

    cmp r9, rax
    jb OLHEAD

LASTLOAD:
    vpcmpeqb ymm1, ymm0, [rdx+r9]

    vpmovmskb r11d, ymm1
    test r11d, r11d
    jnz TRUE

    add r9, 32
    jnz LASTLOAD

RETURN:
    vzeroupper
    ret

TRUE:
    mov byte ptr [rcx], 1
    vzeroupper
    ret
__mrfstr_avx2_tcontchr endp

; mrfstr_bool_t __mrfstr_sse2_contchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_sse2_contchr proc
    movd xmm1, edx
    punpcklbw xmm1, xmm1
    punpcklwd xmm1, xmm1
    pshufd xmm1, xmm1, 0

LHEAD:
    movdqa xmm0, [rcx+r8]
    pcmpeqb xmm0, xmm1

    pmovmskb eax, xmm0
    test ax, ax
    jnz TRUE

    add r8, 16
    jnz LHEAD

    xor al, al
    ret

TRUE:
    mov al, 1
    ret
__mrfstr_sse2_contchr endp

; void __mrfstr_sse2_tcontchr(
;     volatile mrfstr_bool_t *res,
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; res = RES
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_sse2_tcontchr proc
    mov r10b, [rcx]
    test r10b, r10b
    jnz RETURN

    movd xmm1, r8d
    punpcklbw xmm1, xmm1
    punpcklwd xmm1, xmm1
    pshufd xmm1, xmm1, 0

    mov rax, [_mrfstr_searchchr_load]
    cmp r9, rax
    jae LASTLOAD

OLHEAD:
    mov r10, r9
    sub r10, rax

ILHEAD:
    movdqa xmm0, [rdx+r9]
    pcmpeqb xmm0, xmm1

    pmovmskb r11d, xmm0
    test r11w, r11w
    jnz TRUE

    add r9, 16
    cmp r9, r10
    jne ILHEAD

    mov r10b, [rcx]
    test r10b, r10b
    jnz RETURN

    cmp r9, rax
    jb OLHEAD

LASTLOAD:
    movdqa xmm0, [rdx+r9]
    pcmpeqb xmm0, xmm1

    pmovmskb r11d, xmm0
    test r11w, r11w
    jnz TRUE

    add r9, 16
    jnz LASTLOAD

RETURN:
    ret

TRUE:
    mov byte ptr [rcx], 1
    ret
__mrfstr_sse2_tcontchr endp

; mrfstr_bool_t __mrfstr_i64_contchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_i64_contchr proc
    mov r10, 0101010101010101h
    mov r11, 8080808080808080h

    movzx rdx, dl
    imul rdx, r10

LHEAD:
    mov rax, [rcx+r8]
    xor rax, rdx

    mov r9, rax
    sub rax, r10
    not r9
    and rax, r9
    and rax, r11
    jnz TRUE

    add r8, 8
    jnz LHEAD

    xor al, al
    ret

TRUE:
    mov al, 1
    ret
__mrfstr_i64_contchr endp

; void __mrfstr_i64_tcontchr(
;    volatile mrfstr_bool_t *res,
;    mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; res = RES
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_i64_tcontchr proc
    push rbx
    push rbp
    push rsi
    push rdi

    mov bpl, [rcx]
    test bpl, bpl
    jnz RETURN

    mov r10, 0101010101010101h
    mov r11, 8080808080808080h

    movzx r8, r8b
    imul r8, r10

    mov rax, [_mrfstr_searchchr_load]
    cmp r9, rax
    jae LASTLOAD

OLHEAD:
    mov rbp, r9
    sub rbp, rax

ILHEAD:
    mov rsi, [rdx+r9]
    xor rsi, r8

    mov rdi, rsi
    sub rsi, r10
    not rdi
    and rsi, rdi
    and rsi, r11
    jnz TRUE

    add r9, 8
    cmp r9, rbp
    jne ILHEAD

    mov bpl, [rcx]
    test bpl, bpl
    jnz RETURN

    cmp r9, rax
    jb OLHEAD

LASTLOAD:
    mov rsi, [rdx+r9]
    xor rsi, r8

    mov rdi, rsi
    sub rsi, r10
    not rdi
    and rsi, rdi
    and rsi, r11
    jnz TRUE

    add r9, 8
    jnz LASTLOAD

RETURN:
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

TRUE:
    mov byte ptr [rcx], 1
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret
__mrfstr_i64_tcontchr endp

end
