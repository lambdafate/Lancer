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
    
    mov eax, [ss:esp+14*4]    ; cs in stack
    and eax, 0x00000003       ; check cs's CPL
    cmp eax, 3
    je user_interrupt%1
    push %1
    call [interrupt_handler_table + %1*4]
    add esp, 4
    jmp handler_finish%1

user_interrupt%1:
    mov esp, 0x80000
    push %1
    call [interrupt_handler_table + %1*4]
    add esp, 4
    mov esp, [current_task]

handler_finish%1:
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
VECTOR_NOARGS 0x20,ZERO                             ; IRQ0 clock interrupt
VECTOR_NOARGS 0x21,ZERO                             ; IRQ1 keyboard interrupt
VECTOR_NOARGS 0x22,ZERO                             ; IRQ2 no used, connected with slave.
VECTOR_NOARGS 0x23,ZERO                             ; IRQ3    
VECTOR_NOARGS 0x24,ZERO                             ; IRQ4
VECTOR_NOARGS 0x25,ZERO                             ; IRQ5
VECTOR_NOARGS 0x26,ZERO                             ; IRQ6
VECTOR_NOARGS 0x27,ZERO                             ; IRQ7

VECTOR_NOARGS 0x28,ZERO                             ; IRQ8
VECTOR_NOARGS 0x29,ZERO                             ; IRQ9
VECTOR_NOARGS 0x2a,ZERO                             ; IRQ10
VECTOR_NOARGS 0x2b,ZERO                             ; IRQ11
VECTOR_NOARGS 0x2c,ZERO                             ; IRQ12
VECTOR_NOARGS 0x2d,ZERO                             ; IRQ13
VECTOR_NOARGS 0x2e,ZERO                             ; IRQ14
VECTOR_NOARGS 0x2f,ZERO                             ; IRQ15

; user defined interrupt
; having args and return values.
; for syscall mainly
VECTOR_SYSCALL 0x80,ZERO