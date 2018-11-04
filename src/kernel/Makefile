CC = gcc
CFLAGS = -Wall -Wextra -Wno-packed-bitfield-compat -std=c11 -O2 -c -ffreestanding -m32 -masm=intel -I../libc/include -Iinclude

LDMAP = kernel.map
LDSCRIPT = kernel.ld
LD = ld
LDFLAGS = -M -T $(LDSCRIPT) -nostdlib -m elf_i386

ASM = nasm
ASMFLAGS = -f elf32

BIN = ../../bin/kernel
SRC = kernel

OBJS = $(BIN)/start.o \
	$(BIN)/interrupt_service_routines.o \
	$(BIN)/interrupt_request.o \
	$(BIN)/boot.o \
	$(BIN)/gdt.o \
	$(BIN)/idt.o \
	$(BIN)/isr.o \
	$(BIN)/irq.o \
	$(BIN)/pic.o \
	$(BIN)/vga.o \
	$(BIN)/tty.o \
	$(BIN)/pit.o \
	$(BIN)/dma.o \
	$(BIN)/keyboard.o \
	$(BIN)/pmm.o \
	$(BIN)/paging.o \
	$(BIN)/cmos.o \
	$(BIN)/rtc.o \
	$(BIN)/speaker.o \
	$(BIN)/floppy.o \
	$(BIN)/panic.o \
	$(BIN)/kernel_task.o \
	$(BIN)/kernel_main.o

all: clean $(BIN)/kernel.bin | $(BIN)

$(BIN):
	mkdir -p $(BIN)

$(BIN)/%.o: $(SRC)/%.asm | $(BIN)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BIN)/%.o: $(SRC)/%.c | $(BIN)
	$(CC) $< -o $@ $(CFLAGS)

$(BIN)/kernel.elf: $(OBJS) | $(BIN)
	$(LD) $^ ../libc/libk.a -o $@ $(LDFLAGS) 1> ../../$(LDMAP)

$(BIN)/kernel.bin: $(BIN)/kernel.elf | $(BIN)
	objcopy -O binary -S $< $@

clean:
	rm -f $(BIN)/*.o
	rm -f $(BIN)/*.elf