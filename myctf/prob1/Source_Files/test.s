.intel_syntax noprefix
.globl test


test:
    mov dword ptr [eax], ebx
    pop eax
    ret
    xor ebx, ebx
    ret
    xor eax, eax
    ret
    inc eax
    ret
    pop ecx
    ret
    pop edx
    ret
    int 0x80

.att_syntax noprefix
