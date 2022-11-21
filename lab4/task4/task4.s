        segment .data
str1: db "%x",10,0
        segment .text
        global  main
        global end_loop
        global loop
        global rand_num            ; let the linker know about main
        extern  printf          ; resolve printf from libc
        extern puts
        state dd 0x1010
        mask dd 0x1100
main:
    push ebp
    mov ebp, esp
rand_num:
    mov edi, [state] ;state value
    mov eax , 20    ;loop counter
loop:
    cmp eax, 0
    jz end_loop
    dec eax

    mov esi ,[mask]  ;esi is the mask
    and esi ,edi     
    mov ebx, 0       ;odd flag

    one_counter:    ;we will check leftmose bit
        cmp esi, 0
        jz one_counter_end
        xor ebx, esi    
        and ebx, 1
        shr esi, 1
        jmp one_counter

    one_counter_end:
    shr edi, 1
    shl ebx, 15
    add edi, ebx

    pushad         ;print 
    push dword edi
    push dword str1
    call printf
    add esp, 8
    popad

    jmp loop
end_loop:       ;taken form stackoverflow
    mov eax, 0
    mov esp, ebp
    pop ebp
    ret