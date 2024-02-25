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

; void __mrfstr_avx512_copy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)

__mrfstr_avx512_copy proc
LHEAD:
    vmovdqu32 zmm0, [rcx+r8]
    vmovdqa32 [rdx+r8], zmm0

    add r8, 64
    jnz LHEAD

    ret
__mrfstr_avx512_copy endp

; void __mrfstr_avx512_ntcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)

__mrfstr_avx512_ntcopy proc
LHEAD:
    vmovdqu64 zmm0, [rcx+r8]
    vmovntdq zmmword ptr [rdx+r8], zmm0

    add r8, 64
    jnz LHEAD

    sfence
    ret
__mrfstr_avx512_ntcopy endp

; void __mrfstr_avx_copy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)

__mrfstr_avx_copy proc
LHEAD:
    vmovdqu ymm0, ymmword ptr [rcx+r8]
    vmovdqa ymmword ptr [rdx+r8], ymm0

    add r8, 32
    jnz LHEAD

    ret
__mrfstr_avx_copy endp

; void __mrfstr_avx_ntcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)

__mrfstr_avx_ntcopy proc
LHEAD:
    vmovdqu ymm0, ymmword ptr [rcx+r8]
    vmovntdq ymmword ptr [rdx+r8], ymm0

    add r8, 32
    jnz LHEAD

    sfence
    ret
__mrfstr_avx_ntcopy endp

; void __mrfstr_sse_copy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)

__mrfstr_sse_copy proc
LHEAD:
    vmovdqu ymm0, ymmword ptr [rcx+r8]
    vmovdqa ymmword ptr [rdx+r8], ymm0

    add r8, 32
    jnz LHEAD

    ret
__mrfstr_sse_copy endp

; void __mrfstr_sse_ntcopy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)

__mrfstr_sse_ntcopy proc
LHEAD:
    vmovdqu ymm0, ymmword ptr [rcx+r8]
    vmovntdq ymmword ptr [rdx+r8], ymm0

    add r8, 32
    jnz LHEAD

    sfence
    ret
__mrfstr_sse_ntcopy endp

; void __mrfstr_base_copy(
;     mrfstr_ptr_t dst, mrfstr_ptr_ct src, mrfstr_size_t size)

__mrfstr_base_copy proc
LHEAD:
    mov rax, [rcx+r8]
    mov [rdx+r8], rax

    add r8, 32
    jnz LHEAD

    ret
__mrfstr_base_copy endp

end
