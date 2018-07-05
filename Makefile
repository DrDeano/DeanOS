SRC = src
BIN = bin

BOOT = $(SRC)/boot
KERNEL = $(SRC)/kernel
LIBC = $(SRC)/libc

1stSTAGE_BINARY = bootloader.bin
FLOPPY = bootloader.flp

2ndSTAGE_BINARY = 2ndstage.bin

CMD = cmd.exe /c

EMULATOR = qemu-system-i386
EMULATOR_FLAGS = -fda $(BIN)/$(FLOPPY) -curses
#EMULATOR_FLAGS = -drive media=disk,format=raw,file=$(OUTPUT_IMAGE) -curses

EMULATOR_FLAGS_DEBUG = -s -S -fda $(BIN)/$(FLOPPY) -curses
#EMULATOR_FLAGS_DEBUG = -s -S -m 16M -drive media=disk,format=raw,file=$(OUTPUT_IMAGE) -curses

all: clean $(BIN)/$(FLOPPY)

libc:
	@echo "\nCompiling the C Library\n"
	$(MAKE) -C $(LIBC) all

boot:
	@echo "\nCompiling the bootloader\n"
	$(MAKE) -C $(BOOT) all

kernel:
	@echo "\nCompiling the kernel\n"
	$(MAKE) -C $(KERNEL) all

$(BIN)/$(FLOPPY): libc boot kernel | $(BIN)
	@echo "\nCreating $@ floppy image\n"
	$(CMD) PartCopy.exe "$(BIN)\boot\$(1stSTAGE_BINARY)" ".\$@" 0h 511d
	$(CMD) imdisk -a -f "$@" -s 1440K -m A:
	$(CMD) copy "$(BIN)\boot\$(2ndSTAGE_BINARY)" A:
	$(CMD) copy "$(BIN)\kernel\kernel.bin" A:
	$(CMD) imdisk -D -m A:
	
run: | $(BIN)/$(FLOPPY)
	echo "\nEmulating with flags: $(EMULATOR) $(EMULATOR_FLAGS)\n"
	$(EMULATOR) $(EMULATOR_FLAGS)
	
debug: | $(BIN)/$(FLOPPY)
	echo "\nEmulating with flags: $(EMULATOR) $(EMULATOR_FLAGS_DEBUG)\n"
	$(EMULATOR) $(EMULATOR_FLAGS_DEBUG)

clean:
	@echo "\nCleanning the workspace\n"
	rm -f $(BIN)/$(FLOPPY)
	$(MAKE) -C $(KERNEL) clean
	$(MAKE) -C $(BOOT) clean
	$(MAKE) -C $(LIBC) clean