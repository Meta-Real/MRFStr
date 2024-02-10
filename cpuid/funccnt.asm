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

; mrfstr_byte_t mrfstr_cpuid_funccnt(void)

.data
    extern _mrfstr_is_intel : db
    _mrfstr_funccnt db ?
    public _mrfstr_funccnt

.code
mrfstr_cpuid_funccnt proc
    push rbx

    bt word ptr [_mrfstr_is_intel], 0
    jnc AMD

    xor eax, eax
    cpuid

    mov byte ptr [_mrfstr_funccnt], al  ; function count (on Intel)
    pop rbx
    ret

AMD:
    mov eax, 80000000h
    cpuid

    mov byte ptr [_mrfstr_funccnt], al  ; function count (on AMD)
    pop rbx
    ret
mrfstr_cpuid_funccnt endp
end
