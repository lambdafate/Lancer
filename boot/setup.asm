;   file: boot/setup.asm
    
    mov ax, 0xb800
    mov es, ax
    mov di, 0x0000


    mov cx, 25*80
clear:
    mov word [es:di], 0x0000
    add di, 2
    loop clear

    mov di, (12*80+32)*2
    mov ax, cs
    mov ds, ax
    mov si, message
    mov ah, 2
    mov cx, 10
display:
    mov al, [ds:si]
    mov [es:di], ax
    inc si
    add di, 2
    loop display    

    jmp $

message:
    db "Hello, OS!"