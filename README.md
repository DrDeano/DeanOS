# DeanOS
Creating an operating system from scratch

## How to run
Just type `make run` which will create all the files needed and run in the qemu emulator.
The makefile is a shambles, so any suggestions on better makefiles are appreciated.


## Prerequisite
* `nams` - For assembling the bootloader
* `gcc` - For compiling the Kernel and inluded C Library
* `ld` - For linking all the kernel files together
* `objcopy` - For creating the kernel binary from the elf file
* `ar` - For creating the C Libray library
* `make` - To run the makefile
* `qemu` - To run the OS
* `Windows Linux subsystem`:
* `cmd` - For using Windows commands in the Linux subsystem terminal
* `partcopy` - Windows cmd for creating the boot sector
* `imdisk` - Windows cmd for creating and mounting the floppy image
* `copy` - Windows cms for copying files onto the floppy image

I'm sorry that this will hurt people to see as using Windows and Linux together, so if anyone has suggestions and/or better makefile that works that I can use, please let me know. Makefiles are new to me so any changes made, can you comment them so I can understand what is going on. Especially a makefile that just uses Linux commands would be best. I have tried using `dd` for creating the boot sector but couldn't get it working correctly, so any way of getting ride of some Windows commands will be good.
