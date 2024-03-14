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

    .extern _mrfstr_mem_ntlimit

#ifdef WIN32
#define dst %rcx
#define chr %edx
#define size %r8
#else
#define dst %rsi
#define chr %edi
#define size %rcx
#endif

.text

# void __mrfstr_avx512_fill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# size = SIZE

    .globl __mrfstr_avx512f_fill
__mrfstr_avx512f_fill:
#ifdef __clang__
    and $255, chr
#endif

    imul $0x01010101, chr
    vpbroadcastd chr, %zmm16

    cmp _mrfstr_mem_ntlimit(%rip), size
    ja NTSTORE1

    neg size

LHEAD1:
    vmovdqa64 %zmm16, (dst, size)

    add $64, size
    jnz LHEAD1

    ret

NTSTORE1:
    neg size

NTLHEAD1:
    vmovntdq %zmm16, (dst, size)

    add $64, size
    jnz NTLHEAD1

    sfence
    ret

# void __mrfstr_avx512f_ntfill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# size = SIZE

    .globl __mrfstr_avx512f_ntfill
__mrfstr_avx512f_ntfill:
#ifdef __clang__
    and $255, chr
#endif

    imul $0x01010101, chr
    vpbroadcastd chr, %zmm16
    neg size

LHEAD2:
    vmovntdq %zmm16, (dst, size)

    add $64, size
    jnz LHEAD2

    sfence
    ret

# void __mrfstr_avx_fill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# size = SIZE

    .globl __mrfstr_avx_fill
__mrfstr_avx_fill:
#ifdef __clang__
    and $255, chr
#endif

    imul $0x01010101, chr
    movd chr, %xmm0
    pshufd $0, %xmm0, %xmm0
    vinsertf128 $1, %xmm0, %ymm0, %ymm0

    cmp _mrfstr_mem_ntlimit(%rip), size
    ja NTSTORE2

    neg size

LHEAD3:
    vmovdqa %ymm0, (dst, size)

    add $32, size
    jnz LHEAD3

    vzeroupper
    ret

NTSTORE2:
    neg size

NTLHEAD2:
    vmovntdq %ymm0, (dst, size)

    add $32, size
    jnz NTLHEAD2

    sfence
    vzeroupper
    ret

# void __mrfstr_avx_ntfill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# size = SIZE

    .globl __mrfstr_avx_ntfill
__mrfstr_avx_ntfill:
#ifdef __clang__
    and $255, chr
#endif

    imul $0x01010101, chr
    movd chr, %xmm0
    pshufd $0, %xmm0, %xmm0
    vinsertf128 $1, %xmm0, %ymm0, %ymm0
    neg size

LHEAD4:
    vmovntdq %ymm0, (dst, size)

    add $32, size
    jnz LHEAD4

    sfence
    vzeroupper
    ret

# void __mrfstr_sse2_fill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# size = SIZE

    .globl __mrfstr_sse2_fill
__mrfstr_sse2_fill:
#ifdef __clang__
    and $255, chr
#endif

    imul $0x01010101, chr
    movd chr, %xmm0
    pshufd $0, %xmm0, %xmm0

    cmp _mrfstr_mem_ntlimit(%rip), size
    ja NTSTORE3

    neg size

LHEAD5:
    movdqa %xmm0, (dst, size)

    add $16, size
    jnz LHEAD5

    ret

NTSTORE3:
    neg size

NTLHEAD3:
    movntdq %xmm0, (dst, size)

    add $16, size
    jnz NTLHEAD3

    sfence
    ret

# void __mrfstr_sse2_ntfill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# size = SIZE

    .globl __mrfstr_sse2_ntfill
__mrfstr_sse2_ntfill:
#ifdef __clang__
    and $255, chr
#endif

    imul $0x01010101, chr
    movd chr, %xmm0
    pshufd $0, %xmm0, %xmm0
    neg size

LHEAD6:
    movntdq %xmm0, (dst, size)

    add $16, size
    jnz LHEAD6

    sfence
    ret

# void __mrfstr_i64_fill(
#     mrfstr_ptr_t dst, mrfstr_chr_t chr, mrfstr_size_t size)
#
# dst = DST+SIZE
# size = SIZE

    .globl __mrfstr_i64_fill
__mrfstr_i64_fill:
#ifdef __clang__
    and $255, chr
#endif

    imul $0x01010101, chr
    mov chr, %eax
    neg size

#ifdef WIN32
    shl $32, %rdx
    or %rax, %rdx

LHEAD7:
    mov %rdx, (dst, size)
#else
    shl $32, %rdi
    or %rax, %rdi

LHEAD7:
    mov %rdi, (dst, size)
#endif

    add $8, size
    jnz LHEAD7

    ret
