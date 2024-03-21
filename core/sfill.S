# MIT License
#
# Copyright (c) 2023 MetaReal
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

#ifdef WIN32
#define dst %rcx
#define chr %edx
#define size %r8
#else
#define dst %rsi
#define chr %edi
#define size %rcx
#endif

    .extern _mrfstr_mem_ntlimit

.text

# void __mrfstr_avx512_fill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx512f_fill
__mrfstr_avx512f_fill:
    vmovd chr, %xmm16
    vpbroadcastb %xmm16, %ymm16
    vinserti64x4 $1, %ymm16, %zmm16, %zmm16

    cmp _mrfstr_mem_ntlimit(%rip), size
    jb NTLHEAD1

LHEAD1:
    vmovdqa64 %zmm16, (dst, size)

    add $64, size
    jnz LHEAD1

    ret

NTLHEAD1:
    vmovntdq %zmm16, (dst, size)

    add $64, size
    jnz NTLHEAD1

    sfence
    ret

# void __mrfstr_avx512f_vfill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx512f_vfill
__mrfstr_avx512f_vfill:
    vmovd chr, %xmm16
    vpbroadcastb %xmm16, %ymm16
    vinserti64x4 $1, %ymm16, %zmm16, %zmm16

LHEAD2:
    vmovdqa64 %zmm16, (dst, size)

    add $64, size
    jnz LHEAD2

    ret

# void __mrfstr_avx512f_ntfill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx512f_ntfill
__mrfstr_avx512f_ntfill:
    vmovd chr, %xmm16
    vpbroadcastb %xmm16, %ymm16
    vinserti64x4 $1, %ymm16, %zmm16, %zmm16

LHEAD3:
    vmovntdq %zmm16, (dst, size)

    add $64, size
    jnz LHEAD3

    sfence
    ret

# void __mrfstr_avx_fill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx_fill
__mrfstr_avx_fill:
    vpxor %xmm1, %xmm1, %xmm1
    vmovd chr, %xmm0
    vpshufb %xmm1, %xmm0, %xmm0
    vinsertf128 $1, %xmm0, %ymm0, %ymm0

    cmp _mrfstr_mem_ntlimit(%rip), size
    jb NTLHEAD4

LHEAD4:
    vmovdqa %ymm0, (dst, size)

    add $32, size
    jnz LHEAD4

    vzeroupper
    ret

NTLHEAD4:
    vmovntdq %ymm0, (dst, size)

    add $32, size
    jnz NTLHEAD4

    sfence
    vzeroupper
    ret

# void __mrfstr_avx_vfill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx_vfill
__mrfstr_avx_vfill:
    vpxor %xmm1, %xmm1, %xmm1
    vmovd chr, %xmm0
    vpshufb %xmm1, %xmm0, %xmm0
    vinsertf128 $1, %xmm0, %ymm0, %ymm0

LHEAD5:
    vmovdqa %ymm0, (dst, size)

    add $32, size
    jnz LHEAD5

    vzeroupper
    ret

# void __mrfstr_avx_ntfill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx_ntfill
__mrfstr_avx_ntfill:
    vpxor %xmm1, %xmm1, %xmm1
    vmovd chr, %xmm0
    vpshufb %xmm1, %xmm0, %xmm0
    vinsertf128 $1, %xmm0, %ymm0, %ymm0

LHEAD6:
    vmovntdq %ymm0, (dst, size)

    add $32, size
    jnz LHEAD6

    sfence
    vzeroupper
    ret

# void __mrfstr_sse2_fill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_sse2_fill
__mrfstr_sse2_fill:
    movd chr, %xmm0
    punpcklbw %xmm0, %xmm0
    punpcklwd %xmm0, %xmm0
    pshufd $0, %xmm0, %xmm0

    cmp _mrfstr_mem_ntlimit(%rip), size
    jb NTLHEAD7

LHEAD7:
    movdqa %xmm0, (dst, size)

    add $16, size
    jnz LHEAD7

    ret

NTLHEAD7:
    movntdq %xmm0, (dst, size)

    add $16, size
    jnz NTLHEAD7

    sfence
    ret

# void __mrfstr_sse2_vfill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_sse2_vfill
__mrfstr_sse2_vfill:
    movd chr, %xmm0
    punpcklbw %xmm0, %xmm0
    punpcklwd %xmm0, %xmm0
    pshufd $0, %xmm0, %xmm0

LHEAD8:
    movdqa %xmm0, (dst, size)

    add $16, size
    jnz LHEAD8

    ret

# void __mrfstr_sse2_ntfill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_sse2_ntfill
__mrfstr_sse2_ntfill:
    movd chr, %xmm0
    punpcklbw %xmm0, %xmm0
    punpcklwd %xmm0, %xmm0
    pshufd $0, %xmm0, %xmm0

LHEAD9:
    movntdq %xmm0, (dst, size)

    add $16, size
    jnz LHEAD9

    sfence
    ret

# void __mrfstr_i64_fill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_i64_fill
__mrfstr_i64_fill:
    mov $0x0101010101010101, %rax

#ifdef WIN32
    movzx %dl, %rdx
    imul %rax, %rdx

LHEAD10:
    mov %rdx, (dst, size)
#else
    movzx %dil, %rdi
    imul %rax, %rdi

LHEAD10:
    mov %rdi, (dst, size)
#endif

    add $8, size
    jnz LHEAD10

    ret
