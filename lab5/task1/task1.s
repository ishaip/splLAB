segment .data
format: db "%X",10,0
str1: db 10
i_str: db "-i"
e_str: db "+e"
o_str: db "-o"

len: db 0
encoder: db 0 ;add value always
input_flag: db 0
output_flag: db 0

segment .text
WRITE EQU 4
STDOUT EQU 1
global  main            ; let the linker know about main
extern strlen
extern printf
main:
        push    ebp             ; prepare stack frame for main
        mov     ebp, esp
        mov     edi, dword[ebp+8]    ; get argc into edi       
        mov     esi, [ebp+12]   ; get first argv string into esi
set_flags:
        mov eax, 0
        mov eax, [esi]
        mov ebx, dword[eax+8]        
        push dword ebx
        push dword format
        call printf
        add esp, 8

        cmp e_str, [esi]
        jnz no_e_flag
        ;hold the rest of esi
        ;add to the global encoder 

no_e_flag:

        cmp i_str, [esi]
        jnz no_i_flag
        ;init the -i case

no_i_flag:

        cmp o_str, [esi]
        jnz no_o_flag
        ;init o case

no_o_flag:

        add     esi, 4          ; advance to the next pointer in argv
        dec     edi             ; decrement edi from argc to 0
        cmp     edi, 0          ; when it hits 0, we're done
        jnz     set_flags      ; end with NULL pointer

end_loop:

        mov eax,0
        leave
        ret