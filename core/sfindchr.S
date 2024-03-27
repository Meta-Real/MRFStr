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
#define str %rcx
#define chr %edx
#define size %r8

#define rchr %rdx
#define block %rbx
#define num %rbp

#define res2 %rcx
#define start2 %rdx
#define str2 %r8
#define chr2 %r9d
#define size2 %r9

#define rchr2 %r9
#define size3 %rbx
#define limit2 %rsi
#define bcopy2 %rdi
#else
#define str %rsi
#define chr %edi
#define size %rcx

#define rchr %rdi
#define block %rdx
#define num %r8

#define res2 %rsi
#define start2 %rdi
#define str2 %rcx
#define chr2 %edx
#define size2 %r8

#define rchr2 %rdx
#define size3 %r8
#define limit2 %r9
#define bcopy2 %rbx
#endif

    .extern _mrfstr_search_load

.text

# mrfstr_idx_t __mrfstr_avx512bw_findchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx512bw_findchr
__mrfstr_avx512bw_findchr:
    vpbroadcastb chr, %zmm16
    mov size, %rax
    neg %rax

LHEAD1:
    vpcmpeqb (str, size), %zmm16, %k0

    kortestq %k0, %k0
    jnz IDX1

    add $64, size
    jnz LHEAD1

    mov $-1, %rax
    ret

IDX1:
    kmovq %k0, %rdx
    bsf %rdx, %rdx

    lea (%rax, size), %rax
    lea (%rax, %rdx), %rax
    ret

# mrfstr_idx_t __mrfstr_avx512bw_tfindchr(
#     volatile mrfstr_idx_t *res, mrfstr_idx_t start,
#     mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# res = RES
# start = START
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx512bw_tfindchr
__mrfstr_avx512bw_tfindchr:
    cmp start2, (res2)
    jb RETURN2

    vpbroadcastb chr2, %zmm16

#ifdef WIN32
    mov 40(%rsp), size2
#endif

    mov _mrfstr_search_load(%rip), %r11
    mov size2, %rax
    neg %rax

    cmp %r11, size2
    jae LASTLOAD2

OLHEAD2:
    mov size2, %r10
    sub %r11, %r10

ILHEAD2:
    vpcmpeqb (str2, size2), %zmm16, %k0

    kortestq %k0, %k0
    jnz IDX2

    add $64, size2
    cmp %r10, size2
    jne ILHEAD2

    cmp start2, (res2)
    jb RETURN2

    cmp %r11, size2
    jb OLHEAD2

LASTLOAD2:
    vpcmpeqb (str2, size2), %zmm16, %k0

    kortestq %k0, %k0
    jnz IDX2

    add $64, size2
    jnz LASTLOAD2

RETURN2:
    mov $-1, %rax
    ret

IDX2:
    lea (%rax, size2), %rax
    lea (%rax, start2), %rax

    kmovq %k0, %rdx
    bsf %rdx, %rdx

    lea (%rax, %rdx), %rax
    ret

# mrfstr_idx_t __mrfstr_avx2_findchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx2_findchr
__mrfstr_avx2_findchr:
    vpxor %xmm1, %xmm1, %xmm1
    vmovd chr, %xmm0
    vpshufb %xmm1, %xmm0, %xmm0
    vinsertf128 $1, %xmm0, %ymm0, %ymm0

    mov size, %rax
    neg %rax

LHEAD3:
    vpcmpeqb (str, size), %ymm0, %ymm1

    vpmovmskb %ymm1, %edx
    test %edx, %edx
    jnz IDX3

    add $32, size
    jnz LHEAD3

    mov $-1, %rax
    vzeroupper
    ret

IDX3:
    bsf %edx, %edx
    lea (%rax, size), %rax
    lea (%rax, %rdx), %rax

    vzeroupper
    ret

# mrfstr_idx_t __mrfstr_avx2_tfindchr(
#     volatile mrfstr_idx_t *res, mrfstr_idx_t start,
#     mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# res = RES
# start = START
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx2_tfindchr
__mrfstr_avx2_tfindchr:
    cmp start2, (res2)
    jb RETURN4

    vpxor %xmm1, %xmm1, %xmm1
    vmovd chr2, %xmm0
    vpshufb %xmm1, %xmm0, %xmm0
    vinsertf128 $1, %xmm0, %ymm0, %ymm0

#ifdef WIN32
    mov 40(%rsp), size2
#else
    mov size2, %rax
    neg %rax
#endif

    mov _mrfstr_search_load(%rip), %r11
    cmp %r11, size2
    jae LASTLOAD4

OLHEAD4:
    mov size2, %r10
    sub %r11, %r10

ILHEAD4:
    vpcmpeqb (str2, size2), %ymm0, %ymm1

#ifdef WIN32
    vpmovmskb %ymm1, %eax
    test %eax, %eax
#else
    vpmovmskb %ymm1, %r9d
    test %r9d, %r9d
#endif
    jnz IDX4

    add $32, size2
    cmp %r10, size2
    jne ILHEAD4

    cmp start2, (res2)
    jb RETURN4

    cmp %r11, size2
    jb OLHEAD4

LASTLOAD4:
    vpcmpeqb (str2, size2), %ymm0, %ymm1

#ifdef WIN32
    vpmovmskb %ymm1, %eax
    test %eax, %eax
#else
    vpmovmskb %ymm1, %r9d
    test %r9d, %r9d
#endif
    jnz IDX4

    add $32, size2
    jnz LASTLOAD4

RETURN4:
    mov $-1, %rax
    vzeroupper
    ret

IDX4:
#ifdef WIN32
    bsf %eax, %eax
    lea (%rax, start2), %rax

    mov 40(%rsp), %rdx
    neg %rdx

    lea (%rax, %rdx), %rax
#else
    bsf %r9d, %r9d
    lea (%rax, %r9), %rax
    lea (%rax, start2), %rax
#endif

    lea (%rax, size2), %rax
    vzeroupper
    ret

# mrfstr_idx_t __mrfstr_sse2_findchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_sse2_findchr
__mrfstr_sse2_findchr:
    movd chr, %xmm1
    punpcklbw %xmm1, %xmm1
    punpcklwd %xmm1, %xmm1
    pshufd $0, %xmm1, %xmm1

    mov size, %rax
    neg %rax

LHEAD5:
    movdqa (str, size), %xmm0
    pcmpeqb %xmm1, %xmm0

    pmovmskb %xmm0, %edx
    test %dx, %dx
    jnz IDX5

    add $16, size
    jnz LHEAD5

    mov $-1, %rax
    ret

IDX5:
    bsf %dx, %dx

    lea (%rax, size), %rax
    lea (%rax, %rdx), %rax
    ret

# mrfstr_idx_t __mrfstr_sse2_tfindchr(
#     volatile mrfstr_idx_t *res, mrfstr_idx_t start,
#     mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# res = RES
# start = START
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_sse2_tfindchr
__mrfstr_sse2_tfindchr:
    cmp start2, (res2)
    jb RETURN6

    movd chr2, %xmm1
    punpcklbw %xmm1, %xmm1
    punpcklwd %xmm1, %xmm1
    pshufd $0, %xmm1, %xmm1

#ifdef WIN32
    mov 40(%rsp), size2
#else
    mov size2, %rax
    neg %rax
#endif

    mov _mrfstr_search_load(%rip), %r11
    cmp %r11, size2
    jae LASTLOAD6

OLHEAD6:
    mov size2, %r10
    sub %r11, %r10

ILHEAD6:
    movdqa (str2, size2), %xmm0
    pcmpeqb %xmm1, %xmm0

#ifdef WIN32
    pmovmskb %xmm0, %eax
    test %ax, %ax
#else
    pmovmskb %xmm0, %r9d
    test %r9w, %r9w
#endif
    jnz IDX6

    add $16, size2
    cmp %r10, size2
    jne ILHEAD6

    cmp start2, (res2)
    jb RETURN6

    cmp %r11, size2
    jb OLHEAD6

LASTLOAD6:
    movdqa (str2, size2), %xmm0
    pcmpeqb %xmm1, %xmm0

#ifdef WIN32
    pmovmskb %xmm0, %eax
    test %ax, %ax
#else
    pmovmskb %xmm0, %r9d
    test %r9w, %r9w
#endif
    jnz IDX6

    add $16, size2
    jnz LASTLOAD6

RETURN6:
    mov $-1, %rax
    ret

IDX6:
#ifdef WIN32
    bsf %ax, %ax
    lea (%rax, start2), %rax

    mov 40(%rsp), %rdx
    neg %rdx

    lea (%rax, %rdx), %rax
#else
    bsf %r9w, %r9w
    lea (%rax, %r9), %rax
    lea (%rax, start2), %rax
#endif

    lea (%rax, size2), %rax
    ret

# mrfstr_idx_t __mrfstr_i64_findchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_i64_findchr
__mrfstr_i64_findchr:
#ifdef WIN32
    push block
#endif

    mov $0x0101010101010101, %r10
    mov $0x8080808080808080, %r11

#ifdef WIN32
    movzx %dl, rchr
#else
    movzx %dil, rchr
#endif
    imul %r10, rchr

    mov size, %rax
    neg %rax

LHEAD7:
    mov (str, size), block
    xor rchr, block

    mov block, %r9
    sub %r10, block
    not %r9
    and %r9, block
    and %r11, block
    jnz IDX7

    add $8, size
    jnz LHEAD7

    mov $-1, %rax
#ifdef WIN32
    pop block
#endif
    ret

IDX7:
#ifdef WIN32
    push num
#endif
    mov $0x7f7f7f7f7f7f7f7f, num

    not %r9
    mov %r9, block

    and num, block
    lea (block, num), block
    or %r9, block
    or num, block
    not block

    bsf block, block
    shr $3, block

    lea (%rax, size), %rax
    lea (%rax, block), %rax

#ifdef WIN32
    pop num
    pop block
#endif
    ret

# mrfstr_idx_t __mrfstr_i64_tfindchr(
#     volatile mrfstr_idx_t *res, mrfstr_idx_t start,
#     mrfstr_data_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# res = RES
# start = START
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_i64_tfindchr
__mrfstr_i64_tfindchr:
    push %rbx
    push %rbp
#ifdef WIN32
    push %rsi
    push %rdi
#else
    push %r12
#endif

    cmp start2, (res2)
    jb RETURN8

    mov $0x0101010101010101, %r10
    mov $0x8080808080808080, %r11

#ifdef WIN32
    movzx %r9b, rchr2
#else
    movzx %dl, rchr2
#endif
    imul %r10, rchr2

#ifdef WIN32
    mov 72(%rsp), size3
#else
    mov size3, %r12
    neg %r12
#endif
    mov _mrfstr_search_load(%rip), %rbp
    cmp %rbp, size3
    jae LASTLOAD8

OLHEAD8:
    mov size3, limit2
    sub %rbp, limit2

ILHEAD8:
    mov (str2, size3), %rax
    xor rchr2, %rax

    mov %rax, bcopy2
    sub %r10, %rax
    not bcopy2
    and bcopy2, %rax
    and %r11, %rax
    jnz IDX8

    add $8, size3
    cmp limit2, size3
    jne ILHEAD8

    cmp start2, (res2)
    jb RETURN8

    cmp %rbp, size3
    jb OLHEAD8

LASTLOAD8:
    mov (str2, size3), %rax
    xor rchr2, %rax

    mov %rax, bcopy2
    sub %r10, %rax
    not bcopy2
    and bcopy2, %rax
    and %r11, %rax
    jnz IDX8

    add $8, size3
    jnz LASTLOAD8

RETURN8:
    mov $-1, %rax

#ifdef WIN32
    pop %rdi
    pop %rsi
#else
    pop %r12
#endif
    pop %rbp
    pop %rbx
    ret

IDX8:
    mov $0x7f7f7f7f7f7f7f7f, %rbp

    not bcopy2
    mov bcopy2, %rax

    and %rbp, %rax
    lea (%rax, %rbp), %rax
    or bcopy2, %rax
    or %rbp, %rax
    not %rax

    bsf %rax, %rax
    shr $3, %rax
    lea (%rax, start2), %rax

#ifdef WIN32
    mov 72(%rsp), %rdx
    neg %rdx

    lea (%rax, %rdx), %rax
#else
    lea (%rax, %r12), %rax
#endif
    lea (%rax, size3), %rax

#ifdef WIN32
    pop %rdi
    pop %rsi
#else
    pop %r12
#endif
    pop %rbp
    pop %rbx
    ret
