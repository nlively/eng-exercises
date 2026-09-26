; the intended c signature is
; uint64_t mix4(
;     uint64_t a,
;     uint64_t b,
;     uint64_t c,
;     uint64_t d
; );

; syntax
; mov dest, src

; registers used for function args on system v abi
; rdi
; rsi
; rdx
; rcx
; r8
; r9

global _start

section .text

_start:
    call caller         ; run the exercise
    mov rdi, rax        ; use its return value as the exit status
    mov rax, 60         ; linux x86-64 exit syscall
    syscall

mix4:
    mov rax, rdi        ; %rbx = `a`
    add rax, rsi        ; %rbx += `b`
    imul rax, rdx       ; %rbx *= `c` 
    xor rax, rcx        ; %rbx ^= `d`
    ret                 ; send %rax back to caller

caller:
    push rbx

    mov rbx, 0x1122334455667788 ; 64-bit (8-byte) number

    mov edi, 2          ; a = 2         0b0000 0010
    mov esi, 3          ; b = 3         0b0000 0011
    mov edx, 4          ; c = 4         0b0000 0100
    mov ecx, 5          ; d = 5         0b0000 0101
    call mix4

    ; %rax should now contain 17 (0x11, or 0b0001 0001)

    ; i think we lost the value of rbx in the mix4 code
    ; %rbx now equals 0x11

    ; note, the output we _want_ is 
    ; 0x1122334455667799, or
    ; 0b0010 0010 0011 0011 0100 0100 0101 0101 0110 0110 0111 0111 1001 1001

    xor rax, rbx        ; = 0, and that's a bug.

    pop rbx
    ret
