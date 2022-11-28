segment .data
str_infection: db "Hello, Infected File"
format: db "%X",10,0
str1: db 10
i_str: dd "-i"
e_str: dd "+e"
o_str: dd "-o"
len: dd 0
encoder: dd 0 ;add value always
encoder_flag: dd 0 ;the curr_index in encoder
input_file: dd 0 ;check
output_file: dd 1 ;check
sys_read equ 3
sys_write equ 4
sys_open equ 5
buffer: db 0xff
better_encoder: dd 0
offset: dd 0
encoder_len: dd 0


segment .text
WRITE EQU 4
STDOUT EQU 1
global  main            ; let the linker know about main
extern strlen
extern atoi
extern printf
extern system
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
        jnz     no_e_flag
        inc     ebx
        mov     al, [ebx]
        cmp     al, 101         ; compare to e
        jnz     no_e_flag
        inc ebx





        mov [encoder], ebx      ; encoder now holds the string without
        mov eax, [len]
        sub eax, 2
        mov [encoder_len], eax
        mov [encoder_flag], dword 1   ; rise encoder flag


        mov eax, 0
        mov ecx, [encoder]
        mov ebx, [offset]
        mov al, [ecx + ebx]
        cmp al, 51
        jnz no_e_flag
        pushad
        mov	ecx, [encoder]	; message to write

        mov	eax,sys_write	; system call number (sys_write) = 4
        mov	ebx,1		; file descriptor (stdout) = 1
        mov	edx,[encoder_len]		; message length
        ;int	0x80		; call kernel
        popad
        

no_e_flag:

        ;testing the e flag
        ;cmp [better_encoder], dword 2
        ;jnz testing
       ; pushad
      ;  mov	eax,sys_write	; system call number (sys_write) = 4
     ;   mov	ebx,1		; file descriptor (stdout) = 1
    ;    mov	ecx, str_infection	; message to write
   ;     mov	edx,21		; message length
  ;      int	0x80		; call kernel
 ;       popad
;testing:


        mov     ebx, [esi]
        mov     eax, 0
        mov     al, [ebx] 
        cmp     al, 45           ; compare to -
        jnz     no_i_flag
        inc     ebx
        mov     al, [ebx]
        cmp     al, 105         ; compare to i
        jnz     no_i_flag
        inc ebx   ; i flag case to be done, ebx is a pointer to the rest of the input adress

        pushad
        mov eax, sys_open
        mov ebx, ebx
        mov ecx, 0
        mov edx, 0777
        int 0x80
        mov [input_file], eax ;the returned value is in eax 
        popad

no_i_flag:

        mov     ebx, [esi]
        mov     eax, 0
        mov     al, [ebx] 
        cmp     al, 45           ; compare to -
        jne     no_o_flag
        inc     ebx
        mov     al, [ebx]
        cmp     al, 111         ; compare to o
        jne     no_o_flag
        inc ebx   ; i flag case to be done, ebx is a pointer to the rest of the output adress

        pushad
        mov eax, sys_open
        mov ebx, ebx
        mov ecx, 0x42
        mov edx, 0777
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
        mov ebx, [input_file] ; file descriptor (stdin) = 0
        mov ecx, buffer
        mov edx, 1 ;read 1 byte
        int 0x80
        cmp eax, 0
        jz end_of_file  ;making sure were not done
        popad

        cmp [buffer], dword 0   ;removing the enter symbol
        jz do_action 

        ;adding encoder value
        pushad
        cmp [encoder_flag], dword 0
        jz  no_encoder_value

        mov eax, 0
        mov ecx, [encoder]      ; encoder is the pinter to the string
        mov ebx, [offset]       ; what index are we going for
        mov al, [ecx + ebx]     ; al is the value to be added
        sub al, 48              ; normalize al
        add [buffer], al  
      
        add [offset],dword 1    
        ;making sure we didnt finish the encoder length
        mov eax, [encoder_len]
        cmp [offset], eax
        jne no_encoder_value
        mov [offset], dword 0
        popad
no_encoder_value:

        pushad
        mov	eax,sys_write	; system call number (sys_write) = 4
        mov	ebx,[output_file]		; file descriptor (stdout) = 1
        mov	ecx,buffer	; message to write
        mov	edx,1		; message length
        int	0x80		; call kernel
        popad

       jmp do_action

end_of_file:
        mov eax, 0
        mov esp, ebp
        pop ebp
        ret