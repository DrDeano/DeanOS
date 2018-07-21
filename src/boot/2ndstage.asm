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

	[bits	16]					; Tell the assembler that this is a 16bit program not 32bit
	[org	stage_2_location]

	jmp		stage_2_bootload_start


times (3 - ($ - $$)) db 0

boot_sector:
%include 'fat_descripter.asm'

%include 'macros.asm'
%include 'descriptors_macros.asm'
%include 'enabling_a20.asm'
%include 'memory.asm'

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
	
	; Read the size of the memory and store at 'boot_info'
	m_get_memory_size boot_info
	
	; Enable protected mode
	m_enable_protected
	
	jmp		0x08:stage_2_bootloader_32		; Set CS to the code segment of the kernel in the GDT

; If there is a floppy disk error or a boot error, the call this function
boot_error:
	m_write_line disk_error_msg		; Print the disk error message
	m_reboot						; Reboot

%include 'functions.asm'

idt_descriptor:
	dw 0x0000						; 256 entries of 8 bytes for the interrupt table
    dd 0x0000						; The location of the table, at 0x0000:0x0000
	
gdt_descriptor:
	dw 0x0017						; 3 tables of 8 bytes total (minus 1)
    dd 0x0800						; The location of the 3 tables, at 0x0000:0x0800

boot_info:
struc multiboot_info
	.flags					dd	0	; Required
	.memory_low				dd	0	; Memory size. Present if flags[0] is set
	.memory_high			dd	0
	.boot_device			dd	0	; Boot device. Present if flags[1] is set
	.cmd_line				dd	0	; Kernel command line. Present if flags[2] is set
	.mods_count				dd	0	; Number of modules loaded along with kernel. present if flags[3] is set
	.mods_addr				dd	0
	.syms0					dd	0	; Symbol table info. present if flags[4] or flags[5] is set
	.syms1					dd	0
	.syms2					dd	0
	.syms3					dd 	0
	.mem_map_length			dd	0	; Memory map. Present if flags[6] is set
	.mem_map_addr			dd	0
	.drives_length			dd	0	; Physical address of first drive structure. present if flags[7] is set
	.drives_addr			dd	0
	.config_table			dd	0	; ROM configuration table. present if flags[8] is set
	.bootloader_name 		dd	0	; Bootloader name. present if flags[9] is set
	.apm_table				dd	0	; Advanced power management (APM) table. present if flags[10] is set
	.vbe_control_info 		dd	0	; Video BIOS extension (VBE). present if flags[11] is set
	.vbe_mode_info			dd	0
	.vbe_mode				dw	0
	.vbe_interface_seg 		dw	0
	.vbe_interface_off		dw	0
	.vbe_interface_len		dw	0
	.framebuffer_addr		dq	0
	.framebuffer_pitch		dd	0
	.framebuffer_width		dd	0
	.framebuffer_height		dd	0
	.framebuffer_bpp		db	0
	.framebuffer_type		db	0
	.framebuffer_palette_addr	dd	0
	.framebuffer_palette_num_colours	dw	0	
endstruc

kernel_filename  	db "KERNEL  BIN", 0
disk_error_msg  	db "Disk error", 0
loading_msg     	db "Loading: 2nd stage bootloader", 0
reboot_msg      	db "Press any key to reboot", 0
a20_error	      	db "a20 line not initialised", 0

loading_kernel     	db "Loading: Kernel", 0x0A, 0

memory_map_error	db "Error getting memory map from BIOS INT 0x15 0xE820", 0

; Data storage
root_sectors 		db 0,0
root_start   		db 0,0
file_start			db 0,0

kernel_size			db 0,0,0,0

; Now in 32bit mode
	[bits	32]

stage_2_bootloader_32:
	m_set_up_segments
	
	lea		esi, [loading_kernel]
	call	print_string_32
	
	;mov		eax, 0x2BADB002				; Multiboot signature
	;xor		ebx, ebx					; Zero out EBX
	
	; Move kernel to target location
	mov		esi, kernel_location
	mov		edi, kernel_target_location
	mov		ecx, dword [kernel_size]
	shr		ecx, 2						; Divide by 4 as now copying 4 bytes at a time
	cld
	rep		movsd
	
	push	dword [kernel_size]			; Push the kernel size so the kernel knows how big it is
	push	dword boot_info				; Push the boot_info structure for the kernel to uses
	
	call	kernel_target_location		; Jump to the kernel. Shouldn't return, but call because of the extra parameter for multiboot_info.
	
	cli
	hlt

%include '32bit_functions.asm'

times (3 * 512) - ($ - $$) db 0
memory_map:								; The location where the memory map is stored

	[absolute	kernel_parameters]

boot_parameters:
	.signature				resw 1
	.cursor_pos_x			resb 1
	.cursor_pos_y			resb 1
	.memory_map_location	resw 1