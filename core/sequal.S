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
#define str1 %rcx
#define str2 %rdx
#define size %r8

#define res2 %rcx
#define str12 %rdx
#define str22 %r8
#define size2 %r9
#else
#define str1 %rsi
#define str2 %rdi
#define size %rcx

#define res2 %rsi
#define str12 %rdi
#define str22 %rcx
#define size2 %rdx
#endif

    .extern _mrfstr_cmp_load

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
    vpcmpq $4, (str2, size), %zmm16, %k0

    kortestb %k0, %k0
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
    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN2

    mov _mrfstr_cmp_load(%rip), %rax
    cmp %rax, size2
    jbe LASTLOAD2

OLHEAD2:
    mov %r9, %r10
    sub %rax, %r10

ILHEAD2:
    vmovdqu64 (str12, size2), %zmm16
    vpcmpq $4, (str22, size2), %zmm16, %k0

    kortestb %k0, %k0
    jnz FALSE2

    add $64, size2
    cmp %r10, %r9
    jne ILHEAD2

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN2

    cmp %rax, %r9
    ja OLHEAD2

LASTLOAD2:
    vmovdqu64 (str12, size2), %zmm16
    vpcmpq $4, (str22, size2), %zmm16, %k0

    kortestb %k0, %k0
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
    vpcmpq $4, (str2, size), %zmm16, %k0

    kmovw %k0, %eax
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
    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN4

    mov _mrfstr_cmp_load(%rip), %rax
    cmp %rax, size2
    jbe LASTLOAD4

OLHEAD4:
    mov %r9, %r10
    sub %rax, %r10

ILHEAD4:
    vmovdqu64 (str12, size2), %zmm16
    vpcmpq $4, (str22, size2), %zmm16, %k0

    kmovw %k0, %r11d
    test %r11b, %r11b
    jnz FALSE4

    add $64, size2
    cmp %r10, %r9
    jne ILHEAD4

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN4

    cmp %rax, %r9
    ja OLHEAD4

LASTLOAD4:
    vmovdqu64 (str12, size2), %zmm16
    vpcmpq $4, (str22, size2), %zmm16, %k0

    kmovw %k0, %r11d
    test %r11b, %r11b
    jnz FALSE4

    add $64, size2
    jnz LASTLOAD4

RETURN4:
    ret

FALSE4:
    movb $0, (res2)
    ret

# mrfstr_bool_t __mrfstr_avx2_avx512vl_equal(
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_avx2_avx512vl_equal
__mrfstr_avx2_avx512vl_equal:
LHEAD5:
    vmovdqu64 (str1, size), %ymm16
    vpcmpq $4, (str2, size), %ymm16, %k0

    kortestb %k0, %k0
    jnz FALSE5

    add $32, size
    jnz LHEAD5

    mov $1, %al
    ret

FALSE5:
    xor %al, %al
    ret

# void __mrfstr_avx2_avx512vl_tequal(
#     volatile mrfstr_bool_t *res,
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# res = RES
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_avx2_avx512vl_tequal
__mrfstr_avx2_avx512vl_tequal:
    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN6

    mov _mrfstr_cmp_load(%rip), %rax
    cmp %rax, size2
    jbe LASTLOAD6

OLHEAD6:
    mov size2, %r10
    sub %rax, %r10

ILHEAD6:
    vmovdqu64 (str12, size2), %ymm16
    vpcmpq $4, (str22, size2), %ymm16, %k0

    kortestb %k0, %k0
    jnz FALSE6

    add $64, size2
    cmp %r10, size2
    jne ILHEAD6

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN6

    cmp %rax, size2
    ja OLHEAD6

LASTLOAD6:
    vmovdqu64 (str12, size2), %ymm16
    vpcmpq $4, (str22, size2), %ymm16, %k0

    kortestb %k0, %k0
    jnz FALSE6

    add $32, size2
    jnz LASTLOAD6

RETURN6:
    ret

FALSE6:
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
LHEAD7:
    vmovdqu (str1, size), %ymm0
    vpcmpeqq (str2, size), %ymm0, %ymm0

    vpmovmskb %ymm0, %eax
    cmp $0xffffffff, %eax
    jne FALSE7

    add $32, size
    jnz LHEAD7

    mov $1, %al
    vzeroupper
    ret

FALSE7:
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
    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN8

    mov _mrfstr_cmp_load(%rip), %rax
    cmp %rax, size2
    jbe LASTLOAD8

OLHEAD8:
    mov size2, %r10
    sub %rax, %r10

ILHEAD8:
    vmovdqu (str12, size2), %ymm0
    vpcmpeqq (str22, size2), %ymm0, %ymm0

    vpmovmskb %ymm0, %r11d
    cmp $0xffffffff, %r11d
    jne FALSE8

    add $32, size2
    cmp %r10, size2
    jne ILHEAD8

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN8

    cmp %rax, size2
    ja OLHEAD8

LASTLOAD8:
    vmovdqu (str12, size2), %ymm0
    vpcmpeqq (str22, size2), %ymm0, %ymm0

    vpmovmskb %ymm0, %r11d
    cmp $0xffffffff, %r11d
    jne FALSE8

    add $32, size2
    jnz LASTLOAD8

RETURN8:
    vzeroupper
    ret

FALSE8:
    movb $0, (res2)
    vzeroupper
    ret

# mrfstr_bool_t __mrfstr_sse2_avx512vl_equal(
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_sse2_avx512vl_equal
__mrfstr_sse2_avx512vl_equal:
LHEAD9:
    vmovdqu64 (str1, size), %xmm0
    vpcmpq $4, (str2, size), %xmm0, %k0

    kortestb %k0, %k0
    jnz FALSE9

    add $16, size
    jnz LHEAD9

    mov $1, %al
    ret

FALSE9:
    xor %al, %al
    ret

# void __mrfstr_sse2_avx512vl_tequal(
#     volatile mrfstr_bool_t *res,
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# res = RES
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_sse2_avx512vl_tequal
__mrfstr_sse2_avx512vl_tequal:
    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN10

    mov _mrfstr_cmp_load(%rip), %rax
    cmp %rax, size2
    jbe LASTLOAD10

OLHEAD10:
    mov size2, %r10
    sub %rax, %r10

ILHEAD10:
    vmovdqu64 (str12, size2), %xmm0
    vpcmpq $4, (str22, size2), %xmm0, %k0

    kortestb %k0, %k0
    jnz FALSE10

    add $16, size2
    cmp %r10, size2
    jne ILHEAD10

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN10

    cmp %rax, size2
    ja OLHEAD10

LASTLOAD10:
    vmovdqu64 (str12, size2), %xmm0
    vpcmpq $4, (str22, size2), %xmm0, %k0

    kortestb %k0, %k0
    jnz FALSE10

    add $16, size2
    jnz LASTLOAD10

RETURN10:
    ret

FALSE10:
    movb $0, (res2)
    ret

# mrfstr_bool_t __mrfstr_sse2_equal(
#     mrfstr_ptr_ct str1, mrfstr_ptr_ct str2, mrfstr_size_t size)
#
# str1 = STR1+SIZE
# str2 = STR2+SIZE
# size = -SIZE

    .globl __mrfstr_sse2_equal
__mrfstr_sse2_equal:
LHEAD11:
    movdqu (str1, size), %xmm0
    pcmpeqd (str2, size), %xmm0

    pmovmskb %xmm0, %eax
    cmp $0xffff, %ax
    jne FALSE11

    add $16, size
    jnz LHEAD11

    mov $1, %al
    ret

FALSE11:
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
    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN12

    mov _mrfstr_cmp_load(%rip), %rax
    cmp %rax, size2
    jbe LASTLOAD12

OLHEAD12:
    mov size2, %r10
    sub %rax, %r10

ILHEAD12:
    movdqu (str12, size2), %xmm0
    pcmpeqd (str22, size2), %xmm0

    pmovmskb %xmm0, %r11d
    cmp $0xffff, %r11w
    jne FALSE12

    add $16, size2
    cmp %r10, size2
    jne ILHEAD12

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN12

    cmp %rax, size2
    ja OLHEAD12

LASTLOAD12:
    movdqu (str12, size2), %xmm0
    pcmpeqd (str22, size2), %xmm0

    pmovmskb %xmm0, %r11d
    cmp $0xffff, %r11w
    jne FALSE12

    add $16, size2
    jnz LASTLOAD12

RETURN12:
    ret

FALSE12:
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
LHEAD13:
    mov (str1, size), %rax
    cmp (str2, size), %rax
    jne FALSE13

    add $8, size
    jnz LHEAD13

    mov $1, %al
    ret

FALSE13:
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
    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN14

    mov _mrfstr_cmp_load(%rip), %rax
    cmp %rax, size2
    jbe LASTLOAD14

OLHEAD14:
    mov size2, %r10
    sub %rax, %r10

ILHEAD14:
    mov (str12, size2), %r11
    cmp (str22, size2), %r11
    jne FALSE14

    add $8, size2
    cmp %r10, size2
    jne ILHEAD14

    mov (res2), %r10b
    test %r10b, %r10b
    jz RETURN14

    cmp %rax, size2
    ja OLHEAD14

LASTLOAD14:
    mov (str12, size2), %r11
    cmp (str22, size2), %r11
    jne FALSE14

    add $8, size2
    jnz LASTLOAD14

RETURN14:
    ret

FALSE14:
    movb $0, (res2)
    ret
