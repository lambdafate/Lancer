#!/bin/bash
# note: 'ld -Ttext 0x10000' will load kernel begin 0xc0010000 actually!!!


nasm ./boot/boot.asm -o BOOT && 					# load boot to sector1
dd bs=512 if=BOOT of=Image count=1 conv=notrunc && 
rm ./BOOT &&
nasm ./boot/setup.asm -o SETUP &&					# load setup to sector2~sector5
dd bs=512 if=SETUP of=Image count=4 seek=1 conv=notrunc &&
rm ./SETUP &&
gcc -m32 -c ./kernel/lancer.c -o lancer.o &&				# load kernel to sector6~
ld -Ttext 0x10000 -e main -m elf_i386 -o LANCER lancer.o &&
dd bs=512 if=LANCER of=Image count=200 seek=5 conv=notrunc &&
rm lancer.o LANCER &&
bochs -q
