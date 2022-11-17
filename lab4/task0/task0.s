global main ; This is a commet
extern printf


section .text
main:
push ebp        ; save the previus frames pointer
mov ebp, esp    ; move the epb to the next segment in the stack
pushad          ; save all register information
pop ecx, esp    ; pointer to ARGC on stack
