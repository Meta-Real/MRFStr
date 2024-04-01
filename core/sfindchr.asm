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

; mrfstr_idx_t __mrfstr_avx512bw_findchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx512bw_findchr proc
    vpbroadcastb zmm16, edx
    mov rax, r8
    neg rax

LHEAD:
    vpcmpeqb k0, zmm16, [rcx+r8]

    kortestq k0, k0
    jnz IDX

    add r8, 64
    jnz LHEAD

    mov rax, -1
    ret

IDX:
    kmovq rdx, k0
    bsf rdx, rdx

    lea rax, [rax+r8]
    lea rax, [rax+rdx]
    ret
__mrfstr_avx512bw_findchr endp

; mrfstr_idx_t __mrfstr_avx512bw_tfindchr(
;     volatile mrfstr_idx_t *res, mrfstr_idx_t start,
;     mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; res = RES
; start = START
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx512bw_tfindchr proc
    cmp [rcx], rdx
    jb RETURN

    vpbroadcastb zmm16, r9d

    mov r9, [rsp+40]
    mov r11, [_mrfstr_searchchr_load]
    mov rax, r9
    neg rax

    cmp r9, r11
    jae LASTLOAD

OLHEAD:
    mov r10, r9
    sub r10, r11

ILHEAD:
    vpcmpeqb k0, zmm16, [r8+r9]

    kortestq k0, k0
    jnz IDX

    add r9, 64
    cmp r9, r10
    jne ILHEAD

    cmp [rcx], rdx
    jb RETURN

    cmp r9, r11
    jb OLHEAD

LASTLOAD:
    vpcmpeqb k0, zmm16, [r8+r9]

    kortestq k0, k0
    jnz IDX

    add r9, 64
    jnz LASTLOAD

RETURN:
    mov rax, -1
    ret

IDX:
    lea rax, [rax+r9]
    lea rax, [rax+rdx]

    kmovq rdx, k0
    bsf rdx, rdx

    lea rax, [rax+rdx]
    ret
__mrfstr_avx512bw_tfindchr endp

; mrfstr_idx_t __mrfstr_avx2_findchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx2_findchr proc
    vpxor xmm1, xmm1, xmm1
    vmovd xmm0, edx
    vpshufb xmm0, xmm0, xmm1
    vinsertf128 ymm0, ymm0, xmm0, 1

    mov rax, r8
    neg rax

LHEAD:
    vpcmpeqb ymm1, ymm0, [rcx+r8]

    vpmovmskb edx, ymm1
    test edx, edx
    jnz IDX

    add r8, 32
    jnz LHEAD

    mov rax, -1
    vzeroupper
    ret

IDX:
    bsf edx, edx
    lea rax, [rax+r8]
    lea rax, [rax+rdx]

    vzeroupper
    ret
__mrfstr_avx2_findchr endp

; mrfstr_idx_t __mrfstr_avx2_tfindchr(
;     volatile mrfstr_idx_t *res, mrfstr_idx_t start,
;     mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; res = RES
; start = START
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_avx2_tfindchr proc
    cmp [rcx], rdx
    jb RETURN

    vpxor xmm1, xmm1, xmm1
    vmovd xmm0, r9d
    vpshufb xmm0, xmm0, xmm1
    vinsertf128 ymm0, ymm0, xmm0, 1

    mov r9, [rsp+40]
    mov r11, [_mrfstr_searchchr_load]
    cmp r9, r11
    jae LASTLOAD

OLHEAD:
    mov r10, r9
    sub r10, r11

ILHEAD:
    vpcmpeqb ymm1, ymm0, [r8+r9]

    vpmovmskb eax, ymm1
    test eax, eax
    jnz IDX

    add r9, 32
    cmp r9, r10
    jne ILHEAD

    cmp [rcx], rdx
    jb RETURN

    cmp r9, r11
    jb OLHEAD

LASTLOAD:
    vpcmpeqb ymm1, ymm0, [r8+r9]

    vpmovmskb eax, ymm1
    test eax, eax
    jnz IDX

    add r9, 32
    jnz LASTLOAD

RETURN:
    mov rax, -1
    vzeroupper
    ret

IDX:
    bsf eax, eax
    lea rax, [rax+rdx]

    mov rdx, [rsp+40]
    neg rdx

    lea rax, [rax+rdx]
    lea rax, [rax+r9]
    vzeroupper
    ret
__mrfstr_avx2_tfindchr endp

; mrfstr_idx_t __mrfstr_sse2_findchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_sse2_findchr proc
    movd xmm1, edx
    punpcklbw xmm1, xmm1
    punpcklwd xmm1, xmm1
    pshufd xmm1, xmm1, 0

    mov rax, r8
    neg rax

LHEAD:
    movdqa xmm0, [rcx+r8]
    pcmpeqb xmm0, xmm1

    pmovmskb edx, xmm0
    test dx, dx
    jnz IDX

    add r8, 16
    jnz LHEAD

    mov rax, -1
    ret

IDX:
    bsf dx, dx

    lea rax, [rax+r8]
    lea rax, [rax+rdx]
    ret
__mrfstr_sse2_findchr endp

; mrfstr_idx_t __mrfstr_sse2_tfindchr(
;     volatile mrfstr_idx_t *res, mrfstr_idx_t start,
;     mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; res = RES
; start = START
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_sse2_tfindchr proc
    cmp [rcx], rdx
    jb RETURN

    movd xmm1, r9d
    punpcklbw xmm1, xmm1
    punpcklwd xmm1, xmm1
    pshufd xmm1, xmm1, 0

    mov r9, [rsp+40]
    mov r11, [_mrfstr_searchchr_load]
    cmp r9, r11
    jae LASTLOAD

OLHEAD:
    mov r10, r9
    sub r10, r11

ILHEAD:
    movdqa xmm0, [r8+r9]
    pcmpeqb xmm0, xmm1

    pmovmskb eax, xmm0
    test ax, ax
    jnz IDX

    add r9, 16
    cmp r9, r10
    jne ILHEAD

    cmp [rcx], rdx
    jb RETURN

    cmp r9, r11
    jb OLHEAD

LASTLOAD:
    movdqa xmm0, [r8+r9]
    pcmpeqb xmm0, xmm1

    pmovmskb eax, xmm0
    test ax, ax
    jnz IDX

    add r9, 16
    jnz LASTLOAD

RETURN:
    mov rax, -1
    ret

IDX:
    bsf ax, ax
    lea rax, [rax+rdx]

    mov rdx, [rsp+40]
    neg rdx

    lea rax, [rax+rdx]
    lea rax, [rax+r9]
    ret
__mrfstr_sse2_tfindchr endp

; mrfstr_idx_t __mrfstr_i64_findchr(
;     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_i64_findchr proc
    push rbx

    mov r10, 0101010101010101h
    mov r11, 8080808080808080h

    movzx rdx, dl
    imul rdx, r10

    mov rax, r8
    neg rax

LHEAD:
    mov rbx, [rcx+r8]
    xor rbx, rdx

    mov r9, rbx
    sub rbx, r10
    not r9
    and rbx, r9
    and rbx, r11
    jnz IDX

    add r8, 8
    jnz LHEAD

    mov rax, -1
    pop rbx
    ret

IDX:
    push rbp
    mov rbp, 7f7f7f7f7f7f7f7fh

    not r9
    mov rbx, r9

    and rbx, rbp
    lea rbx, [rbx+rbp]
    or rbx, r9
    or rbx, rbp
    not rbx

    bsf rbx, rbx
    shr rbx, 3

    lea rax, [rax+r8]
    lea rax, [rax+rbx]

    pop rbp
    pop rbx
    ret
__mrfstr_i64_findchr endp

; mrfstr_idx_t __mrfstr_i64_tfindchr(
;     volatile mrfstr_idx_t *res, mrfstr_idx_t start,
;     mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
;
; res = RES
; start = START
; str = STR+SIZE
; chr = CHR
; size = -SIZE

__mrfstr_i64_tfindchr proc
    push rbx
    push rbp
    push rsi
    push rdi

    cmp [rcx], rdx
    jb RETURN

    mov r10, 0101010101010101h
    mov r11, 8080808080808080h

    movzx r9, r9b
    imul r9, r10

    mov rbx, [rsp+72]
    mov rbp, [_mrfstr_searchchr_load]
    cmp rbx, rbp
    jae LASTLOAD

OLHEAD:
    mov rsi, rbx
    sub rsi, rbp

ILHEAD:
    mov rax, [r8+rbx]
    xor rax, r9

    mov rdi, rax
    sub rax, r10
    not rdi
    and rax, rdi
    and rax, r11
    jnz IDX

    add rbx, 8
    cmp rbx, rsi
    jne ILHEAD

    cmp [rcx], rdx
    jb RETURN

    cmp rbx, rbp
    jb OLHEAD

LASTLOAD:
    mov rax, [r8+rbx]
    xor rax, r9

    mov rdi, rax
    sub rax, r10
    not rdi
    and rax, rdi
    and rax, r11
    jnz IDX

    add rbx, 8
    jnz LASTLOAD

RETURN:
    mov rax, -1

    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

IDX:
    mov rbp, 7f7f7f7f7f7f7f7fh

    not rdi
    mov rax, rdi

    and rax, rbp
    lea rax, [rax+rbp]
    or rax, rdi
    or rax, rbp
    not rax

    bsf rax, rax
    shr rax, 3
    lea rax, [rax+rdx]

    mov rdx, [rsp+72]
    neg rdx

    lea rax, [rax+rdx]
    lea rax, [rax+rbx]

    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret
__mrfstr_i64_tfindchr endp

end
