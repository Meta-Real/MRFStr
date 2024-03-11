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
;     mrfstr_ptr_t str,
;     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
;     mrfstr_size_t size)
;
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_avx512bw_replchr proc
    imul edx, 01010101h
    vpbroadcastd zmm17, edx
    imul r8d, 01010101h
    vpbroadcastd zmm18, r8d

LHEAD:
    vmovdqa64 zmm16, [rcx+r9]
    vpcmpb k1, zmm16, zmm17, 0
    vmovdqu8 [rcx+r9]{k1}, zmm18

    add r9, 64
    jnz LHEAD

    ret
__mrfstr_avx512bw_replchr endp

; void __mrfstr_avx512bw_replchr2(
;     mrfstr_ptr_t res, mrfstr_ptr_ct str,
;     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
;     mrfstr_size_t size)
;
; res = RES+SIZE
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_avx512bw_replchr2 proc
    imul r8d, 01010101h
    vpbroadcastd zmm17, r8d
    imul r9d, 01010101h
    vpbroadcastd zmm18, r9d

    mov rax, [rsp+40]

LHEAD:
    vmovdqu64 zmm16, [rdx+rax]

    vpcmpb k1, zmm16, zmm17, 0
    vpblendmb zmm16{k1}, zmm16, zmm18

    vmovdqa64 [rcx+rax], zmm16

    add rax, 64
    jnz LHEAD

    ret
__mrfstr_avx512bw_replchr2 endp

; void __mrfstr_avx2_replchr(
;     mrfstr_ptr_t str,
;     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
;     mrfstr_size_t size)
;
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_avx2_replchr proc
    imul edx, 01010101h
    movd xmm1, edx
    pshufd xmm1, xmm1, 0
    vinsertf128 ymm1, ymm1, xmm1, 1

    imul r8d, 01010101h
    movd xmm2, r8d
    pshufd xmm2, xmm2, 0
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
;     mrfstr_ptr_t res, mrfstr_ptr_ct str,
;     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
;     mrfstr_size_t size)
;
; res = RES+SIZE
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_avx2_replchr2 proc
    imul r8d, 01010101h
    movd xmm1, r8d
    pshufd xmm1, xmm1, 0
    vinsertf128 ymm1, ymm1, xmm1, 1

    imul r9d, 01010101h
    movd xmm2, r9d
    pshufd xmm2, xmm2, 0
    vinsertf128 ymm2, ymm2, xmm2, 1

    mov rax, [rsp+40]

LHEAD:
    vmovdqu ymm0, ymmword ptr [rcx+rax]

    vpcmpeqb ymm3, ymm0, ymm1
    vpblendvb ymm0, ymm0, ymm2, ymm3

    vmovdqa ymmword ptr [rdx+rax], ymm0

    add rax, 32
    jnz LHEAD

    vzeroupper
    ret
__mrfstr_avx2_replchr2 endp

; void __mrfstr_sse4_1_replchr(
;     mrfstr_ptr_t str,
;     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
;     mrfstr_size_t size)
;
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_sse4_1_replchr proc
    imul edx, 01010101h
    movd xmm2, edx
    pshufd xmm2, xmm2, 0

    imul r8d, 01010101h
    movd xmm3, r8d
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
__mrfstr_sse4_1_replchr endp

; void __mrfstr_sse4_1_replchr2(
;     mrfstr_ptr_t res, mrfstr_ptr_ct str,
;     mrfstr_chr_t ochr, mrfstr_chr_t nchr,
;     mrfstr_size_t size)
;
; res = RES+SIZE
; str = STR+SIZE
; ochr = OCHR
; nchr = NCHR
; size = -SIZE

__mrfstr_sse4_1_replchr2 proc
    imul r8d, 01010101h
    movd xmm2, r8d
    pshufd xmm2, xmm2, 0

    imul r9d, 01010101h
    movd xmm3, r9d
    pshufd xmm3, xmm3, 0

    mov rax, [rsp+40]

LHEAD:
    movdqu xmm1, [rcx+rax]
    movdqa xmm0, xmm1

    pcmpeqb xmm0, xmm2
    pblendvb xmm1, xmm3, xmm0

    movdqa [rdx+rax], xmm1

    add rax, 16
    jnz LHEAD

    ret
__mrfstr_sse4_1_replchr2 endp

end
