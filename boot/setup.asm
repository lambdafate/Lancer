;  file: /boot/setup.asm
;   

    CurrCS  equ 0x07e0
    
    mov ax, 0x07c0
    mov ds, ax

    ; display stepinfo
    call readpoint
    mov ax, CurrCS
    mov es, ax
    mov bp, stepinfo
    mov cx, 25
    mov bl, 7
    call print

    ; read memory size, it support most 4G
    call read_memory_size

    ; check if reading memory is successful
    cmp eax, 0
    jne display_memoryinfo
    ; reading memory info failed
    call readpoint
    mov bp, memoryerror
    mov cx, 28
    mov bl, 4
    call print
    jmp $

display_memoryinfo:
    mov [ds:0x00], eax            ; store memory size to 0x7c00
    ; convert 'eax' to Decimal, and store it in 'memoryinfo'
    mov si, memoryinfo+20+3       ; es:esi -> momoryinfo: 0000 mb
    mov edx, 0
    mov ebx, 0x100000
    div ebx
    mov edx, 0
    mov ebx, 10
    mov ecx, 4
_loopwrite:
    div ebx
    add dl, 0x30
    mov [es:si], dl
    mov edx, 0
    dec si
    loop _loopwrite

    ; display memoryinfo
    call readpoint
    mov bp, memoryinfo
    mov cx, 30
    mov bl, 7
    call print
    jmp $


; return: eax-> memory size
read_memory_size:
    push es

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

_readsucces:   
    mov eax, ebp
    jmp short _readfinish
_readerror:
    mov eax, 0
_readfinish:
    pop es
    ret


; es:bp -> string
; cx    -> string size
; bl    -> color
print:
    ; read lightpoint position, then display message
    mov bh, 0x00
    mov al, 0x01
    mov ah, 0x13
    int 0x10
    ret

readpoint:
    mov bh, 0x00
    mov ah, 0x03
    int 0x10
    ret




stepinfo:
    db "[/boot/setup          ]: "                      ; length 25

memoryinfo:
    db "reading memory size "                           ; length 30
    db "0000 mb!", 13, 10

memoryerror:
    db "reading memory info error!", 13, 10              ; length 28
message:
    db "Hello, OS!"
