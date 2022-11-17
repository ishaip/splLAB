
        segment .data
format  db      "%s",0x0a,0
        segment .text
        global  main            ; let the linker know about main
        extern  printf          ; resolve printf from libc
        extern puts
main:
        push    ebp             ; prepare stack frame for main
        mov     ebp, esp
        pushad
        mov     esi, dword[ebp + 8]   ; get first argc string into esi
        push    dword [esi]          ; must dereference esi ; points to argc
        push    format
        call    printf
        add     esi, 4               ; points to argv[0]
        push    dword [esi]     ; must dereference esi;
        push    format
        call    puts          ; advance to the next pointer in argv[0]
        leave
        ret