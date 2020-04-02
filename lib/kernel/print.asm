;   file: /lib/kernel/print.asm
;   mainly use to print a char on screen
; 
;       author: lambdafate
;       data: 2020/03/27, 18:00

    CodeSelector    equ     0x08
    ScreenSelector  equ     0x10
    DataSelector    equ     0x18
    StackSelector   equ     0x20

[bits 32]
section .text

global put_char         ; it exports put_char
global put_str 
global put_hex
global put_int        

put_char:
    pushad          ; it will push 8 registers(32 byte) to stack
                    ; so our char's location: esp+32+4 -> esp+36

    mov ax, ScreenSelector
    mov gs, ax

    call readcursor         ; read cursor location, where i put the fucking character!
    xor ebx, ebx
    mov bx, ax

    mov ecx, [ss:esp+36]    ; let me see what the fuck are you?
    
    ; 13-0x0d --- CR (Carriage Return) --- \r ---- it will turn cursor to begin of this line
    ; 10-0x0a --- LF/NL(Line Feed/New Line) --- \n ----- it will turn cursor to begin of next line
    ; 8-0x08 ---- BS (Backspace) --- delete a character
    cmp cl, 0x0d
    je _carriage_return
    cmp cl, 0x0a
    je _carriage_return
    cmp cl, 0x08
    je _backspace

_put_general_char:
    shl bx, 1
    mov byte [gs:ebx], cl
    mov byte [gs:ebx+1], 0x07       ; 0x07 is the fucking white
    inc ax                          ; cursor's new location
    cmp ax, 2000
    jb _setcursor                   ; ax < 2000, ax is a currect location, just setcursor
    call rollscreen                 ; ax >= 2000, just rollscreen
    mov ax, 2000-80                 ; begin index of last line 
    ; call setcursor
    jmp _setcursor

_backspace:
    cmp bx, 0
    je _putchar_finish
    dec bx
    shl bx, 1
    mov byte [gs:ebx], ' '
    mov byte [gs:ebx+1], 0x00
    dec ax
    jmp _setcursor

; \r: change the cursor location to begin of current line
_carriage_return:
    xor dx, dx
    mov di, 80
    div di                          ; dx:ax / di --> to get current line
    sub bx, dx
    mov ax, bx

    cmp cl, 0x0d                    ; just reset cursor if cl == '\r'                    
    je _setcursor

; '\n': change the cursor location to begin of next line
_line_feed:
    add ax, 80
    cmp ax, 2000
    jb _setcursor
    call rollscreen
    mov ax, 2000-80

_setcursor:
    call setcursor

_putchar_finish:
    popad

    ret

; print a str
put_str:
    ; push ds
    push eax
    push ebx

    ; mov ax, DataSelector
    ; mov ds, ax
    mov ebx, [ss:esp+12]
_repchar:
    xor eax, eax
    mov al, [ds:ebx]
    cmp al, 0x00
    je _printstr_finish
    
    push eax                        ; note, note, note !!!
    call put_char                   ; we must follow 'cdecl', if you call a fuction, for example 
    add esp, 4                      ; call put_char here, you must deal with args on stack
              
    inc ebx
    jmp _repchar                    ; oh my God, fuck this line three thousand times
                                    ; 'jmp _repchar' is currect, but i write 'loop _repchar' before
                                    ; i found this bug(so that it will depend on 'ecx' for how many 
                                    ; chars will be printed.)
                                    ; fix this bug(or fault?): 2020/04/02 12:34

_printstr_finish:
    pop ebx
    pop eax
    ; pop ds
    ret


; put_hex
put_hex:
    pushad
    
    mov edx, [ss:esp+36]
    mov eax, edx
    mov ecx, 8
_eachfourbits:
    and eax, 0x0000000f
    cmp al, 9
    ja _turnae           ; a~f
    add al, 48
    jmp _dealnext
_turnae:
    add al, 55
_dealnext:
    push eax
    shr edx, 4
    mov eax,edx 
    loop _eachfourbits

    mov ecx, 8
_printeachhex:
    call put_char                   ; you must clean stack after you call put_char
    add esp, 4                      ; alse it would't work well

    loop _printeachhex
    popad
    ret


; put_int
put_int:
    pushad
    mov eax, [ss:esp+36]
    xor edx, edx
    mov edi, 10
    mov ecx, 0
_eachint:
    div edi
    add edx, 48
    push edx
    inc ecx
    cmp eax, 0
    je _printeachint
    xor edx, edx
    jmp _eachint    

_printeachint:
    call put_char
    add esp, 4                      ; note clean stack arg
    loop _printeachint    

    popad
    ret





; rolling screen
rollscreen:
    pushad

    mov ax, DataSelector
    mov ds, ax
    mov es, ax

    mov esi, 0xb8000+160
    mov edi, 0xb8000
    mov ecx, 4000-160
    cld
    rep movsb

    mov al, ' '
    mov ah, 0x00
    mov ebx, 0xb8000
    add ebx, (2000-80)<<1
    mov ecx, 80
_clearlast:
    mov [ds:ebx], ax
    add ebx, 2
    loop _clearlast

    popad
    ret





; read cursor location
; return: ax->cursor location(0~1999)
readcursor:
    push dx

    xor eax, eax
    mov al, 0x0e
    mov dx, 0x3d4
    out dx, al
    mov dx, 0x3d5
    in al, dx
    mov ah, al

    mov al, 0x0f
    mov dx, 0x3d4
    out dx, al
    mov dx, 0x3d5
    in al, dx

    pop dx
    ret

; set cursor location
; arg: ax--> cursor's new location
setcursor:
    push bx
    push dx
    
    mov bx, ax
    mov al, 0x0e
    mov dx, 0x3d4
    out dx, al
    mov ax, bx
    shr ax, 8
    mov dx, 0x3d5
    out dx, al

    mov al, 0x0f
    mov dx, 0x3d4
    out dx, al
    mov al, bl
    mov dx, 0x3d5
    out dx, al
    
    pop dx
    pop bx 
    ret


