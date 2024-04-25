global _verificate_next_arr  ; int (*)(size_t * next_arr, size_t list_capacity)
global _verificate_prev_arr  ; int (*)(int * prev_arr, size_t list_capacity)

section .text

_verificate_next_arr:
    xor eax, eax
    jmp .check
.next_iteration:
    add rax, 1
    cmp rsi, rax
    je succ_exit
.check:
    cmp qword [rdi+rax*8], rsi
    jb .next_iteration
    mov eax, 1
    ret
succ_exit:
    xor eax, eax
    ret

_verificate_prev_arr:
    xor edx, edx
    jmp .check
.next_iteration:
    cmp rax, rsi
    jnb .fail_exit
    add rdx, 1
    cmp rsi, rdx
    je .succ_exit
.check:
    movsx rax, dword [rdi+rdx*4]
    cmp eax, -1
    jge .next_iteration
.fail_exit:
    mov eax, 1
    ret
.succ_exit:
    xor eax, eax
    ret