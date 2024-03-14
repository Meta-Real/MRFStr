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

    .extern _mrfstr_cmp_load

#ifdef WIN32
.set str1, %rcx
.set str2, %rdx
.set size, %r8

.set res2, %rcx
.set str12, %rdx
.set str22, %r8
.set size2, %r9
#else
.set str1, %rsi
.set str2, %rdi
.set size, %rcx

.set res2, %rsi
.set str12, %rdi
.set str22, %rcx
.set size2, %rdx
#endif

.text

# mrfstr_bool_t __mrfstr_avx512dq_equal(
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_avx512dq_equal
__mrfstr_avx512dq_equal:
LHEAD1:
    vmovdqu64 (str1, size), %zmm16
    vpcmpq $4, (str2, size), %zmm16, %k1

    kortestb %k1, %k1
    jnz FALSE1

    add $64, size
    jnz LHEAD1

    mov $1, %al
    ret

FALSE1:
    xor %al, %al
    ret

# void __mrfstr_avx512dq_tequal(
#     volatile mrfstr_bool_t *res,
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# res = RES
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_avx512dq_tequal
__mrfstr_avx512dq_tequal:
    mov _mrfstr_cmp_load(%rip), %rax

    cmp %rax, size2
    jg LASTLOAD2

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN2

OLHEAD2:
    mov %r9, %r10
    sub %rax, %r10

ILHEAD2:
    vmovdqu64 (str12, size2), %zmm16
    vpcmpq $4, (str22, size2), %zmm16, %k1

    kortestb %k1, %k1
    jnz FALSE2

    add $64, size2
    cmp %r10, %r9
    jne ILHEAD2

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN2

    cmp %rax, %r9
    jl OLHEAD2

LASTLOAD2:
    vmovdqu64 (str12, size2), %zmm16
    vpcmpq $4, (str22, size2), %zmm16, %k1

    kortestb %k1, %k1
    jnz FALSE2

    add $64, size2
    jnz LASTLOAD2

RETURN2:
    ret

FALSE2:
    movb $0, (res2)
    ret

# mrfstr_bool_t __mrfstr_avx512f_equal(
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_avx512f_equal
__mrfstr_avx512f_equal:
LHEAD3:
    vmovdqu64 (str1, size), %zmm16
    vpcmpq $4, (str2, size), %zmm16, %k1

    kmovw %k1, %eax
    test %al, %al
    jnz FALSE3

    add $64, size
    jnz LHEAD3

    mov $1, %al
    ret

FALSE3:
    xor %al, %al
    ret

# void __mrfstr_avx512f_tequal(
#     volatile mrfstr_bool_t *res,
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# res = RES
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_avx512f_tequal
__mrfstr_avx512f_tequal:
    mov _mrfstr_cmp_load(%rip), %rax

    cmp %rax, size2
    jg LASTLOAD4

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN4

OLHEAD4:
    mov %r9, %r10
    sub %rax, %r10

ILHEAD4:
    vmovdqu64 (str12, size2), %zmm16
    vpcmpq $4, (str22, size2), %zmm16, %k1

    kmovw %k1, %r11d
    test %r11b, %r11b
    jnz FALSE4

    add $64, size2
    cmp %r10, %r9
    jne ILHEAD4

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN4

    cmp %rax, %r9
    jl OLHEAD4

LASTLOAD4:
    vmovdqu64 (str12, size2), %zmm16
    vpcmpq $4, (str22, size2), %zmm16, %k1

    kmovw %k1, %r11d
    test %r11b, %r11b
    jnz FALSE4

    add $64, size2
    jnz LASTLOAD4

RETURN4:
    ret

FALSE4:
    movb $0, (res2)
    ret

# mrfstr_bool_t __mrfstr_avx2_equal(
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_avx2_equal
__mrfstr_avx2_equal:
LHEAD5:
    vmovdqu (str1, size), %ymm0
    vpcmpeqq (str2, size), %ymm0, %ymm0

    vpmovmskb %ymm0, %eax
    cmp $0xffffffff, %eax
    jne FALSE5

    add $32, size
    jnz LHEAD5

    mov $1, %al
    vzeroupper
    ret

FALSE5:
    xor %al, %al
    vzeroupper
    ret

# void __mrfstr_avx2_tequal(
#     volatile mrfstr_bool_t *res,
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# res = RES
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_avx2_tequal
__mrfstr_avx2_tequal:
    mov _mrfstr_cmp_load(%rip), %rax

    cmp %rax, size2
    jg LASTLOAD6

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN6

OLHEAD6:
    mov size2, %r10
    sub %rax, %r10

ILHEAD6:
    vmovdqu (str12, size2), %ymm0
    vpcmpeqq (str22, size2), %ymm0, %ymm0

    vpmovmskb %ymm0, %r11d
    cmp $0xffffffff, %r11d
    jne FALSE6

    add $32, size2
    cmp %r10, size2
    jne ILHEAD6

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN6

    cmp %rax, size2
    jl OLHEAD6

LASTLOAD6:
    vmovdqu (str12, size2), %ymm0
    vpcmpeqq (str22, size2), %ymm0, %ymm0

    vpmovmskb %ymm0, %r11d
    cmp $0xffffffff, %r11d
    jne FALSE6

    add $32, size2
    jnz LASTLOAD6

RETURN6:
    vzeroupper
    ret

FALSE6:
    movb $0, (res2)
    vzeroupper
    ret

# mrfstr_bool_t __mrfstr_sse2_equal(
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_sse2_equal
__mrfstr_sse2_equal:
LHEAD7:
    movdqu (str1, size), %xmm0
    pcmpeqd (str2, size), %xmm0

    pmovmskb %xmm0, %eax
    cmp $0xffff, %ax
    jne FALSE7

    add $16, size
    jnz LHEAD7

    mov $1, %al
    ret

FALSE7:
    xor %al, %al
    ret

# void __mrfstr_sse2_tequal(
#     volatile mrfstr_bool_t *res,
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# res = RES
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_sse2_tequal
__mrfstr_sse2_tequal:
    mov _mrfstr_cmp_load(%rip), %rax

    cmp %rax, size2
    jg LASTLOAD8

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN8

OLHEAD8:
    mov size2, %r10
    sub %rax, %r10

ILHEAD8:
    movdqu (str12, size2), %xmm0
    pcmpeqd (str22, size2), %xmm0

    pmovmskb %xmm0, %r11d
    cmp $0xffff, %r11w
    jne FALSE8

    add $16, size2
    cmp %r10, size2
    jne ILHEAD8

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN8

    cmp %rax, size2
    jl OLHEAD8

LASTLOAD8:
    movdqu (str12, size2), %xmm0
    pcmpeqd (str22, size2), %xmm0

    pmovmskb %xmm0, %r11d
    cmp $0xffff, %r11w
    jne FALSE8

    add $16, size2
    jnz LASTLOAD8

RETURN8:
    ret

FALSE8:
    movb $0, (res2)
    ret

# mrfstr_bool_t __mrfstr_i64_equal(
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_i64_equal
__mrfstr_i64_equal:
LHEAD9:
    mov (str1, size), %rax
    cmp (str2, size), %rax
    jne FALSE9

    add $8, size
    jnz LHEAD9

    mov $1, %al
    ret

FALSE9:
    xor %al, %al
    ret

# void __mrfstr_i64_tequal(
#     volatile mrfstr_bool_t *res,
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# res = RES
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_i64_tequal
__mrfstr_i64_tequal:
    mov _mrfstr_cmp_load(%rip), %rax

    cmp %rax, size2
    jg LASTLOAD10

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN10

OLHEAD10:
    mov size2, %r10
    sub %rax, %r10

ILHEAD10:
    mov (str12, size2), %r11
    cmp (str22, size2), %r11
    jne FALSE10

    add $8, size2
    cmp %r10, size2
    jne ILHEAD10

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN10

    cmp %rax, size2
    jl OLHEAD10

LASTLOAD10:
    mov (str12, size2), %r11
    cmp (str22, size2), %r11
    jne FALSE10

    add $8, size2
    jnz LASTLOAD10

RETURN10:
    ret

FALSE10:
    movb $0, (res2)
    ret
