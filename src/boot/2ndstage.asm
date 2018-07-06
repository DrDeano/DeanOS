; ------------------------------------------------------------
; Stage 2 of the bootloader
; ------------------------------------------------------------
%define boot_sector_location    (0x7c03)	; The location of the boot sector
%define fat_segment				(0x0ee0)	; The memory location to load the FAT into memory
%define stage_2_location        (0x1000)	; The location of the second stage bootloader
%define kernel_target_segment   (0x2000)	; The target location for the kernel to be loaded. Above 1MB.

%define kernel_location			(0x20000)
%define kernel_target_location	(0x100000)
;%define kernel_size				(4668)

%define kernel_parameters		(0x7000)
%define SIGNATURE				(0x8a3c)

	[bits	16]             ; Tell the assembler that this is a 16bit program not 32bit
	[org	stage_2_location]

	jmp		stage_2_bootload_start


times (3 - ($ - $$)) db 0

boot_sector:
%include 'fat_descripter.asm'

%include 'macros.asm'
%include 'descriptors_macros.asm'
%include 'enabling_a20.asm'

stage_2_bootload_start:
	cli
	xor		ax, ax
	mov		ds, ax
	mov		es, ax
	sti

	; Copy the boot sector
	m_copy_boot_sector
	
	m_find_file kernel_filename, kernel_target_segment
	
	m_read_file kernel_target_segment, fat_segment
	
	; Save The size read, stored in BX
	mov		word [kernel_size], bx
	
	m_reset_disk
	
	; Write the loading message for the second stage
	m_write_line loading_msg
	
	; Save the the boot parameters so the kernel can access them
	m_save_parameters
	
	; Enable the a20 line
	m_enable_a20
	
	; Set up and write into memory the interrupt descriptor table
	m_setup_idt
	
	; Set up and write into memory the global descriptor table
	m_setup_gdt
	
	; Load the tables
	m_load_gdt_and_idt
	
	; Enable protected mode
	m_enable_protected
	
	jmp		0x08:stage_2_bootloader_32

; If there is a floppy disk error or a boot error, the call this function
boot_error:
	m_write_line disk_error_msg		; Print the disk error message
	m_reboot						; Reboot

%include 'functions.asm'

idt_descriptor:
	dw 0x0000						; 256 entries of 8 bytes for the interrupt table
    dd 0x0000						; The location of the table, at 0x0000:0x0000
	
gdt_descriptor:
	dw 0x0017						; 3 tables of 8 bytes total
    dd 0x0800						; The location of the 3 tables, at 0x0000:0x0800

kernel_filename  	db "KERNEL  BIN", 0
disk_error_msg  	db "Disk error", 0
loading_msg     	db "Loading: 2nd stage bootloader", 0
reboot_msg      	db "Press any key to reboot", 0
a20_error	      	db "a20 line not initialised", 0

loading_kernel     	db "Loading: Kernel", 0xA, 0


; Data storage
root_sectors 		db 0,0
root_start   		db 0,0
file_start			db 0,0

kernel_size			db 0,0

; Now in 32bit mode
	[bits	32]

stage_2_bootloader_32:
	m_set_up_segments
	
	lea		esi, [loading_kernel]
	call print_string_32
	; Move kernel to target location
	cld
	mov		esi, kernel_location
	mov		edi, kernel_target_location
	mov		cx, word [kernel_size]
	rep		movsb
	
	jmp		kernel_target_location

%include '32bit_functions.asm'

times (3 * 512) - ($ - $$) db 0

	[absolute	kernel_parameters]

boot_parameters:
.signature		resw 1
.cursor_pos_x	resb 1
.cursor_pos_y	resb 1