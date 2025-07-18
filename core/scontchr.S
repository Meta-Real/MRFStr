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

#define res2 %rcx
#define str2 %rdx
#define chr2 %r8d
#define size2 %r9
#else
#define str %rdi
#define chr %esi
#define size %rdx

#define res2 %rdi
#define str2 %rsi
#define chr2 %edx
#define size2 %rcx
#endif

    .extern _mrfstr_searchchr_load

.text

# mrfstr_bool_t __mrfstr_avx512bw_contchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx512bw_contchr
__mrfstr_avx512bw_contchr:
    vpbroadcastb chr, %zmm16

LHEAD1:
    vpcmpeqb (str, size), %zmm16, %k0

    kortestq %k0, %k0
    jnz TRUE1

    add $64, size
    jnz LHEAD1

    xor %al, %al
    ret

TRUE1:
    mov $1, %al
    ret

# void __mrfstr_avx512bw_tcontchr(
#     volatile mrfstr_bool_t *res, mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# res = RES
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx512bw_tcontchr
__mrfstr_avx512bw_tcontchr:
    mov (res2), %r10b
    test %r10b, %r10b
    jnz RETURN2

    vpbroadcastb chr2, %zmm16
    mov _mrfstr_searchchr_load(%rip), %rax
    cmp %rax, size2
    jae LASTLOAD2

OLHEAD2:
    mov size2, %r10
    sub %rax, %r10

ILHEAD2:
    vpcmpeqb (str2, size2), %zmm16, %k0

    kortestq %k0, %k0
    jnz TRUE2

    add $64, size2
    cmp %r10, size2
    jne ILHEAD2

    mov (res2), %r10b
    test %r10b, %r10b
    jnz RETURN2

    cmp %rax, size2
    jb OLHEAD2

LASTLOAD2:
    vpcmpeqb (str2, size2), %zmm16, %k0

    kortestq %k0, %k0
    jnz TRUE2

    add $64, size2
    jnz LASTLOAD2

RETURN2:
    ret

TRUE2:
    movb $1, (res2)
    ret

# mrfstr_bool_t __mrfstr_avx2_contchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx2_contchr
__mrfstr_avx2_contchr:
    vpxor %xmm1, %xmm1, %xmm1
    vmovd chr, %xmm0
    vpshufb %xmm1, %xmm0, %xmm0
    vinsertf128 $1, %xmm0, %ymm0, %ymm0

LHEAD3:
    vpcmpeqb (str, size), %ymm0, %ymm1

    vpmovmskb %ymm1, %eax
    test %eax, %eax
    jnz TRUE3

    add $32, size
    jnz LHEAD3

    xor %al, %al
    vzeroupper
    ret

TRUE3:
    mov $1, %al
    vzeroupper
    ret

# void __mrfstr_avx2_tcontchr(
#     volatile mrfstr_bool_t *res, mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# res = RES
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_avx2_tcontchr
__mrfstr_avx2_tcontchr:
    mov (res2), %r10b
    test %r10b, %r10b
    jnz RETURN4

    vpxor %xmm1, %xmm1, %xmm1
    vmovd chr2, %xmm0
    vpshufb %xmm1, %xmm0, %xmm0
    vinsertf128 $1, %xmm0, %ymm0, %ymm0

    mov _mrfstr_searchchr_load(%rip), %rax
    cmp %rax, size2
    jae LASTLOAD4

OLHEAD4:
    mov size2, %r10
    sub %rax, %r10

ILHEAD4:
    vpcmpeqb (str2, size2), %ymm0, %ymm1

    vpmovmskb %ymm1, %r11d
    test %r11d, %r11d
    jnz TRUE4

    add $32, size2
    cmp %r10, size2
    jne ILHEAD4

    mov (res2), %r10b
    test %r10b, %r10b
    jnz RETURN4

    cmp %rax, size2
    jb OLHEAD4

LASTLOAD4:
    vpcmpeqb (str2, size2), %ymm0, %ymm1

    vpmovmskb %ymm1, %r11d
    test %r11d, %r11d
    jnz TRUE4

    add $32, size2
    jnz LASTLOAD4

RETURN4:
    ret

TRUE4:
    movb $1, (res2)
    ret

# mrfstr_bool_t __mrfstr_sse2_contchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_sse2_contchr
__mrfstr_sse2_contchr:
    movd chr, %xmm1
    punpcklbw %xmm1, %xmm1
    punpcklwd %xmm1, %xmm1
    pshufd $0, %xmm1, %xmm1

LHEAD5:
    movdqa (str, size), %xmm0
    pcmpeqb %xmm1, %xmm0

    pmovmskb %xmm0, %eax
    test %ax, %ax
    jnz TRUE5

    add $16, size
    jnz LHEAD5

    xor %al, %al
    ret

TRUE5:
    mov $1, %al
    ret

# void __mrfstr_sse2_tcontchr(
#     volatile mrfstr_bool_t *res, mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# res = RES
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_sse2_tcontchr
__mrfstr_sse2_tcontchr:
    mov (res2), %r10b
    test %r10b, %r10b
    jnz RETURN6

    movd chr2, %xmm1
    punpcklbw %xmm1, %xmm1
    punpcklwd %xmm1, %xmm1
    pshufd $0, %xmm1, %xmm1

    mov _mrfstr_searchchr_load(%rip), %rax
    cmp %rax, size2
    jae LASTLOAD6

OLHEAD6:
    mov size2, %r10
    sub %rax, %r10

ILHEAD6:
    movdqa (str2, size2), %xmm0
    pcmpeqb %xmm1, %xmm0

    pmovmskb %xmm0, %r11d
    test %r11w, %r11w
    jnz TRUE6

    add $16, size2
    cmp %r10, size2
    jne ILHEAD6

    mov (res2), %r10b
    test %r10b, %r10b
    jnz RETURN6

    cmp %rax, size2
    jb OLHEAD6

LASTLOAD6:
    movdqa (str2, size2), %xmm0
    pcmpeqb %xmm1, %xmm0

    pmovmskb %xmm0, %r11d
    test %r11w, %r11w
    jnz TRUE6

    add $16, size2
    jnz LASTLOAD6

RETURN6:
    ret

TRUE6:
    movb $1, (res2)
    ret

# mrfstr_bool_t __mrfstr_i64_contchr(
#     mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_i64_contchr
__mrfstr_i64_contchr:
    mov $0x0101010101010101, %r10
    mov $0x8080808080808080, %r11

    movzx %dl, %rdx
    imul %r10, %rdx

LHEAD7:
    mov (str, size), %rax
    xor %rdx, %rax

    mov %rax, %r9
    sub %r10, %rax
    not %r9
    and %r9, %rax
    and %r11, %rax
    jnz TRUE7

    add $8, size
    jnz LHEAD7

    xor %al, %al
    ret

TRUE7:
    mov $1, %al
    ret

# void __mrfstr_i64_tcontchr(
#    volatile mrfstr_bool_t *res, mrfstr_ptr_ct str, mrfstr_chr_t chr, mrfstr_size_t size)
#
# res = RES
# str = STR+SIZE
# chr = CHR
# size = -SIZE

    .globl __mrfstr_i64_tcontchr
__mrfstr_i64_tcontchr:
    push %rbx
    push %rbp

#ifdef WIN32
    push %rsi
    push %rdi
#endif

    mov (res2), %bpl
    test %bpl, %bpl
    jnz RETURN8

    mov $0x0101010101010101, %r10
    mov $0x8080808080808080, %r11

#ifdef WIN32
    movzx %r8b, %r8
    imul %r10, %r8
#else
    movzx %dl, %rdx
    imul %r10, %rdx
#endif

    mov _mrfstr_searchchr_load(%rip), %rax
    cmp %rax, size2
    jae LASTLOAD8

OLHEAD8:
    mov size2, %rbp
    sub %rax, %rbp

ILHEAD8:
#ifdef WIN32
    mov (str2, size2), %rsi
    xor %r8, %rsi

    mov %rsi, %rdi
    sub %r10, %rsi
    not %rdi
    and %rdi, %rsi
    and %r11, %rsi
#else
    mov (str2, size2), %r8
    xor %rdx, %r8

    mov %r8, %r9
    sub %r10, %r8
    not %r9
    and %r9, %r8
    and %r11, %r8
#endif
    jnz TRUE8

    add $8, size2
    cmp %rbp, size2
    jne ILHEAD8

    mov (res2), %bpl
    test %bpl, %bpl
    jnz RETURN8

    cmp %rax, size2
    jb OLHEAD8

LASTLOAD8:
#ifdef WIN32
    mov (str2, size2), %rsi
    xor %r8, %rsi

    mov %rsi, %rdi
    sub %r10, %rsi
    not %rdi
    and %rdi, %rsi
    and %r11, %rsi
#else
    mov (str2, size2), %r8
    xor %rdx, %r8

    mov %r8, %r9
    sub %r10, %r8
    not %r9
    and %r9, %r8
    and %r11, %r8
#endif
    jnz TRUE8

    add $8, size2
    jnz LASTLOAD8

RETURN8:
#ifdef WIN32
    pop %rdi
    pop %rsi
#endif
    pop %rbp
    pop %rbx
    ret

TRUE8:
    movb $1, (res2)
#ifdef WIN32
    pop %rdi
    pop %rsi
#endif
    pop %rbp
    pop %rbx
    ret
