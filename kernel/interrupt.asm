; file: /kernel/interrupt.asm
;   author: lambdafate
;   dat: 2020/03/30 11:25

%define ERROR_CODE  nop
%define ZERO        push 0

extern current_task
extern interrupt_handler_table
global interrupt_handler_entrys

section .data               ; you can't delete this line, or it will not find handler entry
                            ; when you call int xx

interrupt_handler_entrys:


; this is for cpu inside interrupt(0~31 verctor num)
%macro VECTOR 2
section .text
interrupt_handler_entry%1:
    %2

    push ds
    push es
    push gs
    push fs
    pushad
    
    mov esp, 0x80000
    push %1
    call [interrupt_handler_table + %1*4]
    add esp, 4
    mov esp, [current_task]

    mov al, 0x20
    out 0xa0, al
    out 0x20, al

    ; sti;

    popad
    pop fs
    pop gs
    pop es
    pop ds

    add esp, 4
    iret
section .data
    dd interrupt_handler_entry%1

%endmacro


; this is for general interrupt, but it didn't have 'push %1' and 'add esp, 4' below
; this means their interrupt handle function no args
%macro VECTOR_NOARGS 2
section .text
interrupt_handler_entry%1:
    %2

    push ds
    push es
    push gs
    push fs
    pushad
    
    mov esp, 0x80000
    ; push %1
    call [interrupt_handler_table + %1*4]
    ; add esp, 4
    mov esp, [current_task]

    mov al, 0x20
    out 0xa0, al
    out 0x20, al

    ; sti;

    popad

    pop fs
    pop gs
    pop es
    pop ds

    add esp, 4
    iret
section .data
    dd interrupt_handler_entry%1

%endmacro

; this is for syscall mainly, their interrupt handler no args.
; alse their 'eax' as return value.
%macro VECTOR_SYSCALL 2
section .text
interrupt_handler_entry%1:
    %2

    push ds
    push es
    push gs
    push fs
    pushad
    
    mov esp, 0x80000
    ; push %1
    call [interrupt_handler_table + %1*4]    ; this maybe use 'eax' as return value.
    ; add esp, 4

    push eax                                 ; don't change eax
    mov al, 0x20
    out 0xa0, al
    out 0x20, al
    pop eax

    mov esp, [current_task]

    pop edi
    pop esi
    pop ebp
    ; pop esp
    add esp, 4

    pop ebx
    pop edx
    pop ecx
    ; pop eax                                
    add esp, 4                  ; there maybe will be changed, because
                                ; ebx, ecx, edx maybe as args

    pop fs
    pop gs
    pop es
    pop ds

    add esp, 4
    iret
section .data
    dd interrupt_handler_entry%1

%endmacro

; CPU inside interrupt
VECTOR 0x00,ZERO
VECTOR 0x01,ZERO
VECTOR 0x02,ZERO
VECTOR 0x03,ZERO 
VECTOR 0x04,ZERO
VECTOR 0x05,ZERO
VECTOR 0x06,ZERO
VECTOR 0x07,ZERO 
VECTOR 0x08,ERROR_CODE
VECTOR 0x09,ZERO
VECTOR 0x0a,ERROR_CODE
VECTOR 0x0b,ERROR_CODE 
VECTOR 0x0c,ZERO
VECTOR 0x0d,ERROR_CODE
VECTOR 0x0e,ERROR_CODE
VECTOR 0x0f,ZERO 
VECTOR 0x10,ZERO
VECTOR 0x11,ERROR_CODE
VECTOR 0x12,ZERO
VECTOR 0x13,ZERO 
VECTOR 0x14,ZERO
VECTOR 0x15,ZERO
VECTOR 0x16,ZERO
VECTOR 0x17,ZERO 
VECTOR 0x18,ERROR_CODE
VECTOR 0x19,ZERO
VECTOR 0x1a,ERROR_CODE
VECTOR 0x1b,ERROR_CODE 
VECTOR 0x1c,ZERO
VECTOR 0x1d,ERROR_CODE
VECTOR 0x1e,ERROR_CODE
VECTOR 0x1f,ZERO 

; user defined interrupt
; no args, no return. just general interrupt
VECTOR_NOARGS 0x20,ZERO                             ; clock interrupt


; user defined interrupt
; having args and return values.
; for syscall mainly
VECTOR_SYSCALL 0x21,ZERO