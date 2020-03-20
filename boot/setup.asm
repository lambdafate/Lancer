;   file: /boot/setup.asm
;   
    
    mov ax, 0xb800
    mov es, ax
    mov di, 0x0000

    mov cx, 25*80
clear:
    mov word [es:di], 0x0000
    add di, 2
    loop clear

    call read_memory_size

    mov ax, 0xb800
    mov es, ax
    mov di, 0x0000

    mov cx, 25*80

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




read_memory_size:
    mov ax, 0x07c0
    mov es, ax
    mov di, 0x0000
    mov ecx, 20
    mov ebx, 0
    mov ebp, 0
_loopread:
    mov eax, 0xe820             ; function number
    mov edx, 0x534d4150
    int 0x15
    jc _readerror
    mov eax, [es:di]
    add eax, [es:di+8]
    cmp ebp, eax
    ja _isreadfinished
    mov ebp, eax
_isreadfinished:
    cmp ebx, 0
    je _readsucces
    jmp _loopread

; display memory size
_readsucces:   
    mov ax, 0xb800
    mov es, ax
    mov di, (4*80+60)*2
    mov byte [es:di], 'm'
    mov byte [es:di+1], 4
    sub di, 2

    mov eax, ebp
    mov edx, 0
    mov ebx, 0x100000
    div ebx
    mov edx, 0
    mov ebx, 10
    mov ecx, 4
_loopdisplay:
    div ebx
    add dl, 0x30
    mov [es:di], dl
    mov byte [es:di+1], 4
    mov edx, 0
    sub di, 2
    loop _loopdisplay
    jmp short _readfinish

_readerror:
    jmp $

_readfinish:
    ret

message:
    db "Hello, OS!"
