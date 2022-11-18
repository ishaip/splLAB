        segment .data
format  db      "%s",0x0a,0
        segment .text
        global  main            ; let the linker know about main
        extern  printf          ; resolve printf from libc
        extern puts
main:
        push    ebp             ; prepare stack frame for main
        mov     ebp, esp
        sub     esp, 8
        mov     edi, dword[ebp+8]    ; get argc into edi
        mov     esi, dword[ebp+12]   ; get first argv string into esi
start_loop:
        push    dword [esi]     ; must dereference esi; points to argv
        push    format
        call    printf
        add     esi, 4          ; advance to the next pointer in argv
        dec     edi             ; decrement edi from argc to 0
        cmp     edi, 0          ; when it hits 0, we're done
        jnz     start_loop      ; end with NULL pointer
end_loop:
        leave
        ret