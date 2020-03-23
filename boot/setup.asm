;  file: /boot/setup.asm
;   

    SetupCS         equ     0x9020      ; this file run from 0x9000:512, 0x90200
    SetupIP         equ     0x0000

    HardwareBase    equ     0x9000      ; load the hardware info to there
    HardwareOffset  equ     0x0000

    GDTlIMIT        equ     39
    GDTBaseR        equ     0x8000      ; it's base addr in real mode
    GDTBaseP        equ     0x00080000  ; gdtr need the physical addr

    KernelOldBase   equ     0x1000
    KernelOldOffset equ     0x0000
    KernelNewBase   equ     0x0000
    KernelNewOffset equ     0x0000
    KernelSector    equ     64
    KernelSize      equ     KernelSector*512

    ; selectors
    CodeSelector    equ     0x08
    ScreenSelector  equ     0x10
    DataSelector    equ     0x18
    StackSelector   equ     0x20

    jmp begin_setup

; some api, don't place those after '[bits 32]'
; return: eax-> memory size
read_memory_size:
    push es

    mov ax, HardwareBase
    mov es, ax
    mov di, HardwareOffset      ; info will be loaded to HardwareBase:HardwareOffset
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

; move kernel's position
movkernel:
    push ds
    push es
    push si
    push di

    mov ax, KernelOldBase
    mov ds, ax
    mov si, KernelOldOffset
    mov ax, KernelNewBase
    mov es, ax
    mov di, KernelNewOffset
    mov cx, KernelSize/32       ; use movsd
    cld
    rep movsd

    pop di
    pop si
    pop es
    pop ds
    ret


    ; setup begin run from there
begin_setup:
    mov ax, HardwareBase    ; Hardware info will be stored to ds:0x00
    mov ds, ax

    ; display stepinfo
    call readpoint
    mov ax, SetupCS
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
    mov [ds:HardwareOffset], eax            ; store memory size
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

    ; mov kernel to KernelNewBase:KernelNewOffset
    call movkernel

    ; prepare for into protected mode
    mov ax, GDTBaseR
    mov ds, ax
    ; 装载GDT
    mov dword [ds:0x00], 0x00000000
    mov dword [ds:0x04], 0x00000000

    mov dword [ds:0x08], 0x020003ff         ; 代码段
    mov dword [ds:0x0c], 0x00409809         

    mov dword [ds:0x10], 0x80007fff         ; screen display
    mov dword [ds:0x14], 0x0040920b

    mov dword [ds:0x18], 0x0000ffff         ; 数据段
    mov dword [ds:0x1c], 0x00cf9200

    mov dword [ds:0x20], 0x00000000         ; 堆栈段
    mov dword [ds:0x24], 0x00409600

    ; 填写GDTR
    lgdt [cs:gdtinfo]   

    ; 打开A20
    in al, 0x92
    or al, 000000_1_0B
    out 0x92, al

    ; 关闭中断
    cli

    ; 修改PE位
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword 0x08:flush

    [bits 32]
flush:
    mov ax, DataSelector
    mov ds, ax

    mov ax, StackSelector
    mov ss, ax
    mov esp, 0x7c00

    mov ax, ScreenSelector
    mov es, ax

    push esp

    mov ebx, (20*80+40)*2
    mov byte [es:ebx], 'Y'
    mov byte [es:ebx+1], 4 
    jmp $


; gdt data
gdtinfo:
    dw  GDTlIMIT
    dd  GDTBaseP

stepinfo:
    db "[/boot/setup          ]: "                      ; length 25

memoryinfo:
    db "reading memory size "                           ; length 30
    db "0000 mb!", 13, 10

memoryerror:
    db "reading memory info error!", 13, 10              ; length 28
message:
    db "Hello, OS!"
