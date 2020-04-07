    ; this file will be loaded by /boot/setup.asm
    
    SetupCS         equ     0x9020      ; this file run from 0x9000:512, 0x90200
    SetupIP         equ     0x0000
    SetupProtectBase equ    0x00090200  ; setup's base addr in protect mode

    HardwareBase    equ     0x9000      ; load the hardware info to there
    HardwareOffset  equ     0x0000

    GDTlIMIT        equ     79
    GDTBaseR        equ     0x8000      ; it's base addr in real mode
    GDTBaseP        equ     0x00080000  ; gdtr need the physical addr

    KernelLoadAddr      equ     0x50000   ; boot.asm will load kernel here
    KernelVirtualAddr   equ     0xc0010000      ; setup.asm will extend kernel follow elf
    ; KernelSector    equ     64
    ; KernelSize      equ     KernelSector*512

    ; selectors
    CodeSelector    equ     0x08
    ScreenSelector  equ     0x10
    DataSelector    equ     0x18
    StackSelector   equ     0x20

    TSSSelector     equ     0x28
    LDTSelector     equ     0x30