# makefile: use to build Lancer mainly
# 		author: lambdafate
# 		date: 2020-03-31 20:47
#   

BUILDDIR	= ./build
LIB			= -I lib/kernel/ -I lib/ -I kernel/ -I device/
GCCARGS		= -m32 -fno-stack-protector
GCCCompile  = gcc $(LIB) $(GCCARGS)

NASMARGS	= -f elf
NASMCompile	= nasm $(NASMARGS)

LANCER_OBJECT	=  $(BUILDDIR)/lancer.o $(BUILDDIR)/interrupt_c.o \
	$(BUILDDIR)/init.o $(BUILDDIR)/timer.o $(BUILDDIR)/debug.o $(BUILDDIR)/print.o \
	$(BUILDDIR)/interrupt_asm.o


# BOOT and SETUP
$(BUILDDIR)/BOOT: ./boot/boot.asm
	nasm $^ -o $@ && dd bs=512 if=$@ of=Image count=1 conv=notrunc
$(BUILDDIR)/SETUP: ./boot/setup.asm
	nasm $^ -o $@ && dd bs=512 if=$@ of=Image count=4 seek=1 conv=notrunc


# lancer kernel
$(BUILDDIR)/lancer.o: ./kernel/lancer.c
	$(GCCCompile) -c $^ -o $@
$(BUILDDIR)/interrupt_c.o: ./kernel/interrupt.c
	$(GCCCompile) -c $^ -o $@
$(BUILDDIR)/init.o: ./kernel/init.c
	$(GCCCompile) -c $^ -o $@
$(BUILDDIR)/timer.o: ./device/timer.c
	$(GCCCompile) -c $^ -o $@
$(BUILDDIR)/debug.o: ./kernel/debug.c
	$(GCCCompile) -c $^ -o $@


$(BUILDDIR)/print.o: ./lib/kernel/print.asm
	$(NASMCompile) $^ -o $@

$(BUILDDIR)/interrupt_asm.o: ./kernel/interrupt.asm
	$(NASMCompile) $^ -o $@


$(BUILDDIR)/LANCER: $(LANCER_OBJECT)
	ld -Ttext 0xc0010000 -e main -m elf_i386 -o $@ $^ && \
	dd bs=512 if=$@ of=Image count=200 seek=5 conv=notrunc

# Image
Image: $(BUILDDIR)/BOOT $(BUILDDIR)/SETUP $(BUILDDIR)/LANCER
	echo "build Image!\n"



.PHONY: clean build all run

clean:
	cd $(BUILDDIR) && rm -rf ./*

build:
	make Image
all:
	make build
run:
	make build && bochs -q