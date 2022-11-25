segment .data    
str1: db 10
len: db 0
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
  mov     esi, dword[ebp+12]   ; get first argv string into esi
start_loop:
  pushad
  push esi
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
  popad			;restore fd in ebx from stack

  mov [len], dword 0
  add     esi, 4          ; advance to the next pointer in argv
  dec     edi             ; decrement edi from argc to 0
  cmp     edi, 0          ; when it hits 0, we're done
  jnz     start_loop      ; end with NULL pointer
end_loop:
        mov eax,0
        leave
        ret