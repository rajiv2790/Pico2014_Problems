#include "protect.h"

.intel_syntax noprefix
.globl add_protection

add_protection:
    push eax
    push ebx
    mov eax, protector  #pointer to stored ret address
    mov ebx, [ebp+4]    #return address
    mov [eax], ebx      #store retrun address in arg1
    pop ebx
    pop eax
    ret

.globl remove_protection

remove_protection:
    push ecx
    push ebx
    mov ecx, [ebp+4]     #ret address
    mov ebx, protector   #pointer to stored ret address
    mov ebx, [ebx]       #stored ret address
    cmp ebx, ecx
    je noproblem
    xor eax, eax
    inc eax
    jmp end
noproblem:
    xor eax, eax
end:
    pop ebx
    pop ecx
    ret

.att_syntax noprefix
        

