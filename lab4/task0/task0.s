
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
        push    dword [esi]     ; must dereference esi; points to argv
        push    format
        call    printf
        add     esi, 4          ; esi now points to the next argument
        push    dword [esi]     ; must dereference esi; points to argv
        push    format
        call    printf
        leave
        ret