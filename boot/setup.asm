;   file: /boot/setup.asm
;   setup.asm, it is a kernel loader. it's size is most 4 sectors
;   first it will run in read mode(cs:ip->0x9020:0x0000), and then it resets kernel,
;   and finally it jmp to kernel
;   the main function of this file:
;        0. read memory size(most support 4G) and print in screen.   -- run in real mode
;        1. write gdt and load gdtr, into protected mode             -- run in real mode
;        2. set kernel pages, open paging, reset gdt                 -- run in protected mode
;        3. load kernel according to elf, jmp into kernel            -- run in protected mode(paging)
;    
;   author: lambdafate
;   time  : 2020/03/28, 13:36.          ps: i just eat lunch and write the fucking comments at the fucking time 
;

    %include "boot/include/setup.inc"
    %include "boot/include/page.inc"


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

    ; prepare for into protected mode
    mov ax, GDTBaseR
    mov ds, ax
    ; load GDT
    mov dword [ds:0x00], 0x00000000
    mov dword [ds:0x04], 0x00000000

    mov dword [ds:0x08], 0x020003ff         ; code seg, only exec, no read, no write
    mov dword [ds:0x0c], 0x00409809         

    mov dword [ds:0x10], 0x80007fff         ; screen display
    mov dword [ds:0x14], 0x0040920b

    mov dword [ds:0x18], 0x0000ffff         ; data seg
    mov dword [ds:0x1c], 0x00cf9200

    mov dword [ds:0x20], 0x00000000         ; stack seg
    mov dword [ds:0x24], 0x00409600


    ; add below seg descriptor, main for task switch and user task
    ; data: 2020/04/07 21:47
    mov dword [ds:0x28], 0x00000000         ; TSS
    mov dword [ds:0x2c], 0x00000000         
    
    mov dword [ds:0x30], 0x0000ffff         ; user code seg 
    mov dword [ds:0x34], 0x00cff800

    mov dword [ds:0x38], 0x0000ffff         ; user data seg 
    mov dword [ds:0x3c], 0x00cff200

    mov dword [ds:0x40], 0x00000000         ; user stack seg 
    mov dword [ds:0x44], 0x0040f600


    ; load GDTR
    lgdt [cs:gdtinfo]   

    ; open A20
    in al, 0x92
    or al, 000000_1_0B
    out 0x92, al

    ; close interrupt
    cli

    ; change PE
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword CodeSelector:flush

    [bits 32]
flush:
    mov ax, DataSelector
    mov ds, ax

    ; open paging
    ; 1. create pages
    call create_page
    
    ; note, note, note!!!
    ; there can't use [cs:gdtinfo], because cs segment can't be written, can't be read
    sgdt [ds:SetupProtectBase + gdtinfo]
    add dword [ds:SetupProtectBase + gdtinfo + 2], 0xc0000000

    call reset_gdt

    ; 2. change cr3 -> pde
    mov eax, PAGE_DIR_TABLE_POS
    mov cr3, eax

    ; 3. change cr0 -> PG=1, PG is last bit in cr0
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax                ; success open paging 

    ; note, note, note  there can't use [cs:gdtinfo], because cs segmeng can't be read!!!
    ; fuck, this fault spent my a whole afternoon!!!
    lgdt [ds:SetupProtectBase + gdtinfo]
    ; using new selector
    jmp dword CodeSelector:SetupProtectBase+flushpage

flushpage:
    mov ax, StackSelector
    mov ss, ax
    mov esp, 0x80000
                                ; init segment register before into kernel
    mov ax, DataSelector
    mov ds, ax  
    mov es, ax

    mov ax, ScreenSelector
    mov gs, ax
    mov fs, ax

    call kernel_init            ; init kernel, prepare for leaving setup and jmping to kernel
    
    jmp eax     ; jmp to kernel, eax->e_entry





reset_gdt:
    push ebx

    mov ebx, GDTBaseP
    mov dword [ds:ebx+0x08], 0x0000ffff     ; code seg, base->0x00000000 limit->4G
    mov dword [ds:ebx+0x0c], 0x00cf9800

    add dword [ds:ebx+0x14], 0xc0000000     ; screen seg, base+0xc0000000

    add dword [ds:ebx+0x24], 0xc0000000     ; stack seg, base+0xc0000000
    pop ebx
    ret


; init kernel, extend kernel to virtual address according to kernel's elf header
kernel_init:
    mov eax, 0
    mov ebx, 0
    mov ecx, 0
    mov edx, 0

    mov dx, [ds:KernelLoadAddr+42]   ; e_pdentsize, program header entry's size
    mov ebx, [ds:KernelLoadAddr+28]  ; e_phoff(it should be 0x34), first program header's offset(from file begin)
    mov cx, [ds:KernelLoadAddr+44]   ; e_phnum, program header entry's num
    add ebx, KernelLoadAddr

_deal_each_segment:
    cmp dword [ds:ebx+0], 0x00       ; if p_type==PT_NULL(00), we should not load it
    je _dealnext

    cld
    push ecx
    mov esi, KernelLoadAddr          ; note, move data: ds:esi-->es:edi
    add esi, [ds:ebx+4]              ; you must think about what the fuck are es and ds
    mov edi, [ds:ebx+8]              ; if (ds and es)'s base addr is 0x00, it works well
    mov ecx, [ds:ebx+16]             ; if not, for example their base addr is 0xc0000000, and
    rep movsb                        ; meantime the edi(vritual addr) is 0xc0010000 
    mov eax, 1                       ; it will make you fuck yourself. don't say anything, i first
    pop ecx

_dealnext:
    add ebx, edx
    loop _deal_each_segment
    mov eax, [ds:KernelLoadAddr+24]     ; e_entry

    ret





; create virtual pages, prepare for opening paging
create_page:
    mov ecx, 4096
    mov esi, 0
_clearpde:                                          ; clear pde
    mov byte [ds:PAGE_DIR_TABLE_POS+esi], 0
    inc esi
    loop _clearpde;
_createpde:
    mov eax, PAGE_DIR_TABLE_POS
    add eax, 0x1000                                 ; eax->first page
    mov ebx, eax

    or eax, PAGE_P | PAGE_RW_W | PAGE_US_U

    mov [ds:PAGE_DIR_TABLE_POS+0x000], eax          ; pde0 -> eax
    mov [ds:PAGE_DIR_TABLE_POS+0xc00], eax          ; pde0x300 -> eax

    sub eax, 0x1000
    mov [ds:PAGE_DIR_TABLE_POS+4092], eax           

    mov ecx, 256        ; 2mb/4kb
    mov esi, 0
    mov edx, PAGE_P | PAGE_RW_W | PAGE_US_U
_createpte:
    mov [ds:ebx+esi], edx
    add esi, 4
    add edx, 4096
    loop _createpte

    mov eax, PAGE_DIR_TABLE_POS
    add eax, 0x2000
    or eax, PAGE_P | PAGE_RW_W | PAGE_US_U
    mov ebx, PAGE_DIR_TABLE_POS
    mov esi, 769
    mov ecx, 254                ; kernel pde index: 768~1022, 768->first page, 769~1022->second page
_createother:
    mov [ds:ebx+esi*4], eax
    inc esi
    add eax, 0x1000
    loop _createother
    ret



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

