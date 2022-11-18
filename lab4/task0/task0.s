
        segment .data
format1      db      "%s",0x0a,0
format2      db     "%d",10,0
        segment .text
        global  main            ; let the linker know about main
        extern  printf          ; resolve printf from libc
        extern puts
main:
        push    ebp             ; prepare stack frame for main
        mov     ebp, esp
        push    dword [ebp+8]     ; must dereference esi; points to argv
        push    format2
        call    printf

        mov     esi, [ebp + 12]          ; esi now points to the next argument
        push    dword [esi]     ; must dereference esi; points to argv
        call    puts
        leave
        ret