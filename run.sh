#!/bin/bash

nasm ./boot/boot.asm -o BOOT && 					# load boot to sector1
dd bs=512 if=BOOT of=Image count=1 conv=notrunc && 
rm ./BOOT &&

nasm ./boot/setup.asm -o SETUP &&					# load setup to sector2~sector5
dd bs=512 if=SETUP of=Image count=4 seek=1 conv=notrunc &&
rm ./SETUP &&

# load kernel to sector6~
gcc -m32 -I lib/kernel/ -I lib/ -I kernel/ -I device/  -fno-stack-protector  -c ./kernel/lancer.c -o build/lancer.o &&				
gcc -m32 -I lib/kernel/ -I lib/ -I kernel/ -I device/  -fno-stack-protector  -c ./kernel/interrupt.c -o build/interrupt_c.o &&
gcc -m32 -I lib/kernel/ -I lib/ -I kernel/ -I device/  -fno-stack-protector  -c ./kernel/init.c -o build/init.o  &&
gcc -m32 -I lib/kernel/ -I lib/ -I kernel/ -I device/  -fno-stack-protector  -c ./device/timer.c -o build/timer.o  &&
gcc -m32 -I lib/kernel/ -I lib/ -I kernel/ -I device/  -fno-stack-protector  -c ./kernel/debug.c -o build/debug.o  &&
nasm -f elf ./lib/kernel/print.asm -o build/print.o  &&
nasm -f elf ./kernel/interrupt.asm -o build/interrupt_asm.o &&
ld -Ttext 0xc0010000 -e main -m elf_i386 -o LANCER build/*.o  &&
dd bs=512 if=LANCER of=Image count=200 seek=5 conv=notrunc &&
bochs -q
