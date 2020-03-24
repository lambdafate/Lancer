#!/bin/bash

nasm ./boot/boot.asm -o BOOT && dd bs=512 if=BOOT of=Image count=1 conv=notrunc && bochs -q
