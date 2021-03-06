    ; this file will be loaded by /boot/boot.asm
    MovePosition    equ     0x9000

    SetupCS         equ     0x9020            ; load setup to 0x90000+512
    SetupIP         equ     0x0000            ; setup: 0x9000:0x200 == 0x9020:0x0000
    SetupBeginSector    equ   2
    SetupSector     equ     0x04              ; setup's size (512byte*SetupSector)

    KernelBase        equ     0x5000          ; load kernel to 0x50000
    KernelOffset        equ     0x0000
    KernelBeginSector   equ     6
    KernelSector    equ     64