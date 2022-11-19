        segment  .data
format1   db    "%s",0x0a,0
format2   db    "%d",10,0
        segment  .text
        global   main            ; let the linker know about main
x_struct dd 5
x_num   db      0xaa,1,2,0x44,0x4f
        extern   printf          ; resolve printf from libc
        extern   puts

main:
        push    ebp             ; prepare stack frame for main
        mov     ebp, esp
        pushad
      ;  sub     esp, 8
     ;   mov     edi, dword[ebp+8]    ; get argc into edi
     ;   mov     esi, dword[ebp+12]   ; get first argv string into esi
start_loop:
        mov     ebx, dword[x_num ]
        ;mov     eax, dword[ebx]
        push    dword [ebx]    ; must dereference esi; points to argv
        push    format2
        call    printf
        ;add     esi, 4          ; advance to the next pointer in argv
        ;dec     edi             ; decrement edi from argc to 0
        ;cmp     edi, 0          ; when it hits 0, we're done
        ;jnz     start_loop      ; end with NULL pointer
end_loop:
        leave
        ret