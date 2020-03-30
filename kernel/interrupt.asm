; file: /kernel/interrupt.asm
;   author: lambdafate
;   dat: 2020/03/30 11:25

%define ERROR_CODE  nop
%define ZERO        push 0

section .data
interrupt_message:  db "  interrupt happy!", 13, 10, 0

extern put_str
extern put_int

global interrupt_handler_table
interrupt_handler_table:

%macro VECTOR 2
section .data
    dd interrupt_handler_entry%1

section .text
interrupt_handler_entry%1:
    %2

    push %1
    call put_int
    add esp, 4
    
    push interrupt_message
    call put_str
    add esp, 4

    mov al, 0x20
    out 0xa0, al
    out 0x20, al

    add esp, 4
    iret

%endmacro




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
VECTOR 0x20,ZERO
