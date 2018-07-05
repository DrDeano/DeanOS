	[bits	32]
	
	section	.text

; Sets up the new GDT into the processor while flushing out the old one.
global _gdt_load
_gdt_load:
	; Get GDT pointer into EAX, and desired segment offset into EBX.
	mov		eax, [esp + 4]
	mov		ebx, [esp + 8]

	; Load the GDT.
	lgdt	[eax]

	; Load segments.
	mov		ds, bx
	mov		es, bx
	mov		fs, bx
	mov		gs, bx
	mov		ss, bx
	
	jmp		0x08:.flush
.flush:
	ret

; Loads the TSS into the processor.
global _tss_load
_tss_load:
	; Load specified TSS segment into the processor.
	mov		eax, [esp + 4]
	ltr		ax
	ret