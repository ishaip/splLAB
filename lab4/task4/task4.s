        segment .data
format1   db    "%s",0x0a,0
format2   db    "%d",10,0
        segment .text
        global  main
        global end_loop
        global loop
        global rand_num            ; let the linker know about main
        extern  printf          ; resolve printf from libc
        extern puts
        state dw 16
        mask db 255
main:
    push ebp
    mov ebp, esp
    push state
    call rand_num
    mov eax, 0
    mov esp, ebp
    pop ebp
    ret
rand_num:
    push ebp
    mov edi, [esp+8] ;p to state value
    mov eax , 20    ;loop counter
    mov ebx, 0
    mov bl , [edi]  ;ebx will hold the state value
loop:
    cmp eax, 0
    jz end_loop
    dec eax

    mov ecx, 0
    xor ecx , ebx       ;get the parity
    shl word[ecx], 15   ;get the last bit
    Shr word[ebx], 1    ;shr
    add ebx, ecx        ;add the MSB bit

    pushad         ;print 
    push dword ebx
    push dword format2
    call printf
    add esp, 8
    popad
    jmp loop

end_loop:
    mov eax, 0
    mov esp, ebp
    pop ebp
    ret