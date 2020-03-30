; file: /kernel/interrupt.asm
;   author: lambdafate
;   dat: 2020/03/30 11:25

%define ERROR_CODE  nop
%define ZERO        push 0

interrupt_message:  db "interrupt happy!", 0

extern put_str
global interrupt_handler_table
interrupt_handler_table:

%macro Vector 2
section .data
    dd interrupt_handler_entry%1

section .text
interrupt_handler_entry%1:
    %2
    
    push interrupt_message
    call put_str
    add esp, 4

    mov al, 0x20
    out 0xa0, al
    out 0x20, al

    add esp, 4
    iret
%endmacro




Vector 0, ZERO
Vector 1, ZERO
Vector 2, ZERO
Vector 3, ZERO
Vector 4, ZERO
Vector 5, ZERO
Vector 6, ZERO
Vector 7, ZERO
Vector 8, ERROR_CODE
Vector 9, ZERO
Vector 10, ERROR_CODE
Vector 11, ERROR_CODE
Vector 12, ERROR_CODE
Vector 13, ERROR_CODE
Vector 14, ERROR_CODE
Vector 15, ZERO
Vector 16, ZERO
Vector 17, ERROR_CODE
Vector 18, ZERO
Vector 19, ZERO
Vector 20, ZERO
Vector 21, ZERO
Vector 22, ZERO
Vector 23, ZERO
Vector 24, ZERO
Vector 25, ZERO
Vector 26, ZERO
Vector 27, ZERO
Vector 28, ZERO
Vector 29, ZERO
Vector 30, ZERO
Vector 31, ZERO


Vector 32, ZERO
