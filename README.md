# DeanOS
Creating an operating system from scratch. This operating system is build for i386+ 32bit Intel x86. Any other platforms may not work and haven't been tested on.

## How to run
Just type `make run` which will create all the files needed and run in the qemu emulator.
The makefile is a shambles, so any suggestions on better makefiles are appreciated.

## Prerequisite
* `nasm` - For assembling the bootloader
* `gcc` - For compiling the Kernel and included C Library
* `ld` - For linking all the kernel files together
* `objcopy` - For creating the kernel binary from the elf file
* `ar` - For creating the C Library library
* `make` - To run the makefile
* `qemu` - To run the OS
* `Windows Linux subsystem`:
* `cmd` - For using Windows commands in the Linux subsystem terminal
* `partcopy` - Windows cmd for creating the boot sector
* `imdisk` - Windows cmd for creating and mounting the floppy image
* `copy` - Windows cms for copying files onto the floppy image

I'm sorry that this will hurt people to see as using Windows and Linux together, so if anyone has suggestions and/or better makefile that works that I can use, please let me know. Makefiles are new to me so any changes made, can you comment them so I can understand what is going on. Especially a makefile that just uses Linux commands would be best. I have tried using `dd` for creating the boot sector but couldn't get it working correctly, so any way of getting ride of some Windows commands will be good.

## Testing
You can run `make debug` which will create all the files needed and run in the qemu emulator. You can then attach a GDB debugger by adding the lines into .gdbinit or type the commands directly:
```
set architecture i386
target remote localhost:1234
br *0x7c00
c
```
This will set the architecture to 16 bit x86 as this is what the bootloader is coded in to start with. Then connects to the emulator on port 1234 (default for qemu). Then sets a break point at the beginning of the bootloader. The starts the OS and stops at the beginning of the bootloader.

If want to break at the beginning of the kernel, the address it is loaded at is 0x100000. So replace `br *0x7c00` with `br *0x100000`.

## WARNING
You run my operating system at you own risk on real hardware. There is no guarantee that hardware won't be damaged by running the OS. So run in a virtual machine or emulator for safety.
