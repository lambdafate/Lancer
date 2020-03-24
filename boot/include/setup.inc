    ; this file will be loaded by /boot/setup.asm
    
    SetupCS         equ     0x9020      ; this file run from 0x9000:512, 0x90200
    SetupIP         equ     0x0000
    SetupProtectBase equ    0x00090200  ; setup's base addr in protect mode

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