segment .data
str_infection: db "Hello, Infected File"
format: db "%X",10,0
str1: db 10
i_str: dd "-i"
e_str: dd "+e"
o_str: dd "-o"
len: db 0
encoder: db 0 ;add value always
encoder_flag: db 0 ;the curr_index in encoder
input_file: db 0 ;check
output_file: db 1 ;check
sys_read equ 3
sys_write equ 4
sys_open equ 5
buffer: db 0xff


segment .text
WRITE EQU 4
STDOUT EQU 1
global  main            ; let the linker know about main
extern strlen
extern atoi
extern printf
main:
        push    ebp             ; prepare stack frame for main
        mov     ebp, esp
        mov     edi, dword[ebp+8]    ; get argc into edi       
        mov     esi, [ebp+12]   ; get first argv string into esi
;        jmp do_action
set_flags:
;        mov eax, 0
;        mov eax, [esi]
;        mov ebx, dword[esi+8]
;        push dword edi
        
        pushad
        push dword[esi]
        call strlen
        mov [len], eax
        add esp, 4
        popad

        pushad  
        mov eax, WRITE		;sys_write
	mov ebx, STDOUT		;fd of stdout which is unbuffered
	mov ecx, dword [esi]		;pointer to buffer with data
	mov edx, dword [len]		;length of data to be written (4 bytes)
	int 0x80
        popad

        pushad  
        mov eax, WRITE		;sys_write
	mov ebx, STDOUT		;fd of stdout which is unbuffered
        mov ecx, str1
        mov edx, 1
        int 0x80
        popad

        mov     ebx, [esi]
        mov     eax, 0
        mov     al, [ebx] 
        cmp     al, 43           ; compare to +
        jne     no_e_flag
        inc     ebx
        mov     al, [ebx]
        cmp     al, 101         ; compare to e
        jne     no_e_flag
        inc ebx

        mov eax, 0
        push ebx
        call atoi      ;ebx is a pointer to the rest of the string 
        mov [encoder], eax      ; encoder now holds the int value of the string
        mov [encoder_flag], dword 1   ; rise encoder flag
        
no_e_flag:

        mov     ebx, [esi]
        mov     eax, 0
        mov     al, [ebx] 
        cmp     al, 45           ; compare to +
        jne     no_i_flag
        inc     ebx
        mov     al, [ebx]
        cmp     al, 105         ; compare to e
        jne     no_i_flag
        inc ebx   ; i flag case to be done, ebx is a pointer to the rest of the input adress

        pushad
        mov eax, sys_open
        mov ebx, ebx
        mov ecx, 1
        int 0x80
        mov [input_file], eax ;the returned value is in eax 
        popad
no_i_flag:

        mov     ebx, [esi]
        mov     eax, 0
        mov     al, [ebx] 
        cmp     al, 45           ; compare to +
        jne     no_o_flag
        inc     ebx
        mov     al, [ebx]
        cmp     al, 111         ; compare to e
        jne     no_o_flag
        inc ebx   ; i flag case to be done, ebx is a pointer to the rest of the output adress

        pushad
        mov eax, sys_open
        mov ebx, ebx
        mov ecx, 2
        int 0x80
        mov [output_file], eax ;the returned value is in eax 
        popad 
no_o_flag:

        add     esi, 4          ; advance to the next pointer in argv
        dec     edi             ; decrement edi from argc to 0
        cmp     edi, 0          ; when it hits 0, we're done
        jnz     set_flags      ; end with NULL pointer
        ;use sys_open output

do_action:
        ;if no_flags: encoder = 0, else encoder = value
        ;use sys_read
        pushad
        mov eax, sys_read
        mov ebx, 0 ; file descriptor (stdin) = 0
        mov ecx, buffer
        mov edx, 1 ;wrong- need to change
        int 0x80
        popad

        ;adding encoder value
        cmp [encoder_flag], dword 0
        jz  no_encoder_value
        pushad
        mov eax, [encoder]
        add [buffer],eax 
        popad
no_encoder_value:

        pushad
        mov	eax,sys_write	; system call number (sys_write) = 4
        mov	ebx,1		; file descriptor (stdout) = 1
        mov	ecx,buffer	; message to write
        mov	edx,1		; message length
        int	0x80		; call kernel
        popad

       jmp do_action
        
        mov eax, 0
        mov esp, ebp
        pop ebp
        ret