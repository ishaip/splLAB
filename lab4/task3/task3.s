section .data
    str1: db "%x",10,0
    str2: db "%d",10,0
    x_struct: dd 5
    x_num: db 0xaa, 1,2,0x44,0x4f
    y_struct: dd 6
    y_num: db 0xaa, 1,2,3,0x44,0x4f
section .text
    global multi_add
    global main
    global get_maxmin
    global x_struct
    global y_struct
    global loop
    global done_loop
    global done_min_arr
    extern printf
    extern malloc

main: 
    push ebp
    mov ebp, esp
    push dword x_struct
    push dword y_struct
    call get_maxmin
    mov eax, 0
    mov esp, ebp
    pop ebp
    ret
get_maxmin:
    push ebp
    mov ebp, esp
    mov edi, [ebp+8] ;y_struct pointer
    mov eax, [edi] ; y_size

    mov esi, [ebp+12] ;x_struct pointer
    mov ebx, [esi] ; y_size

    add esi, 4 ; esi is x_struct values
    add edi, 4 ; edi is x_struct values

    cmp     ebx, eax
    jna     multi_add                                                                                                                                        
    mov     ecx, eax ; save the bigger size in eax
    mov     eax, ebx
    mov     ebx, ecx

    mov     ecx, edi ; save the bigger struct in edi
    mov     edi, esi
    mov     esi, ecx
    
multi_add:
   mov edx, eax ; duplicate max_size to edx
    push edx 
    
    mov ecx, eax 
    add ecx, ecx
    add ecx, ecx ; ecx = 4*max_size
    push ecx 
    call malloc ; allocate new array in eax
    add esp, 4
    pop edx

loop:
    mov ecx, 0
    cmp edx, 0
    jz done_loop ; the end of the bigger array
    dec edx
    cmp ebx, 0
    jz done_min_arr ; the end of the smaller array
    dec ebx
    mov ecx, 0 ; add esi[i]
    mov cl, [esi] 
    
done_min_arr:
    push edx 
    mov edx, 0
    mov dl, [edi] ; add edi[i]
    add ecx, edx
    pop edx
    mov [eax], ecx ; move the result to eax[i]
    
    pushad ;print the eax array
    push dword ecx
    push dword str1
    call printf
    add esp, 8
    popad

    inc esi 
    inc edi
    inc eax
    jmp loop
done_loop:
    mov eax, 0
    mov esp, ebp
    pop ebp
    ret