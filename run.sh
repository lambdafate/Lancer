#!/bin/bash

nasm ./boot/boot.asm -o BOOT && 					# load boot to sector1
dd bs=512 if=BOOT of=Image count=1 conv=notrunc && 
rm ./BOOT &&

nasm ./boot/setup.asm -o SETUP &&					# load setup to sector2~sector5
dd bs=512 if=SETUP of=Image count=4 seek=1 conv=notrunc &&
rm ./SETUP &&


gcc -m32 -I lib/kernel/ -c ./kernel/lancer.c -o lancer.o &&				# load kernel to sector6~
nasm -f elf ./lib/kernel/print.asm -o print.o  &&
ld -Ttext 0xc0010000 -e main -m elf_i386 -o LANCER *.o  &&
dd bs=512 if=LANCER of=Image count=200 seek=5 conv=notrunc &&
rm *.o &&
bochs -q
