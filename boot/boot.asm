;   file: /boot/boot.asm 
;   boot.asm, it is a little boot sector. it's size is 512byte
;   BIOS will load this file to 0x0000:0x7c00(cs:ip), and run in real mode
;   the main function of this file:
;        1. load the setup.asm to memory(0x7e00)
;        2. jmp to the setup.asm(cs=0x07e0, ip=0x0000)
;    
;   author: lambdafate
;   time  : 2020/03/10 
;

    ; declare some constant
    JmpTargetCS     equ     0x07e0          ; setup's cp
    JmpTargetIP     equ     0x0000          ; setup's ip
    LoadTargetSize  equ     0x02            ; setup's size (512byte*LoadTargetSize)

    ; let's begin the fucking boot sector!
    mov ax, JmpTargetCS
    mov es, ax
    mov bx, JmpTargetIP             
    mov ah, 0x02
    mov al, LoadTargetSize
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    mov dl, 0x00
    int 0x13                                ; read 'setup.asm' in boot disk

    ; check if reading is successful (cf=0 if success)
    jnc loadsuccess              
    mov bh, 0x00
    mov ah, 0x03
    int 0x10
    mov ax, 0x07c0                      ; there should be 0x07c0, not 0x7c00
    mov es, ax                          ; es:bp is message' display-position
    mov bp, errorinfo
    mov cx, 29                          ; error message's size
    mov al, 0x01
    mov bl, 0x04                        ; message color
    mov ah, 0x13
    int 0x10                            ; display error message if failed, then stop runing
    jmp $

loadsuccess:
    ; next, i will jmp to setup.
    ; jmp $
    jmp JmpTargetCS:JmpTargetIP


errorinfo:
    ; give some messages if reading error
    db  13, 10 
    db "bootsector reading error!"
    db  13, 10 

    times 510-($-$$) db 0
    db 0x55, 0xaa














