ENTRY(start_kernel)
OUTPUT_FORMAT(elf32-i386)
SECTIONS
{
    . = 1M;
	
    .text : {
        *(.text)
    }
    .data : {
        *(.data)
    }
    .rodata : {
        *(.rodata)
    }
    .bss : {
        *(.bss)
    }
}