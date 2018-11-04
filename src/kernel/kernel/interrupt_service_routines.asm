	[bits	32]
	section	.text

;
; Interrupt service routines
;

; 0: Divide By Zero Exception.
global _isr00
_isr00:
	cli
	push	byte 0 ; No error code.
	push	byte 0 ; Exception number.
	jmp		isr_common_stub

; 1: Debug Exception.
global _isr01
_isr01:
	cli
	push	byte 0 ; No error code.
	push	byte 1 ; Exception number.
	jmp		isr_common_stub

; 2: Non Maskable Interrupt Exception.
global _isr02
_isr02:
	cli
	push	byte 0 ; No error code.
	push	byte 2 ; Exception number.
	jmp		isr_common_stub

; 3: Breakpoint Exception.
global _isr03
_isr03:
	cli
	push	byte 0 ; No error code.
	push	byte 3 ; Exception number.
	jmp		isr_common_stub

; 4: Overflow Exception.
global _isr04
_isr04:
	cli
	push	byte 0 ; No error code.
	push	byte 4 ; Exception number.
	jmp		isr_common_stub

; 5: Bound Range Exceeded Exception.
global _isr05
_isr05:
	cli
	push	byte 0 ; No error code.
	push	byte 5 ; Exception number.
	jmp		isr_common_stub

; 6: Invalid Opcode Exception.
global _isr06
_isr06:
	cli
	push	byte 0 ; No error code.
	push	byte 6 ; Exception number.
	jmp		isr_common_stub

; 7: Device Not Available Exception.
global _isr07
_isr07:
	cli
	push	byte 0 ; No error code.
	push	byte 7 ; Exception number.
	jmp		isr_common_stub

; 8: Double Fault Exception.
global _isr08
_isr08:
	cli
	push	byte 8 ; Exception number. Error code is pushed by system.
	jmp		isr_common_stub

;  9: Coprocessor Segment Overrun (unused in modern processors).
global _isr09
_isr09:
	cli
	push	byte 0 ; No error code.
	push	byte 9 ; Exception number.
	jmp		isr_common_stub

; 10: Invalid TSS Exception.
global _isr10
_isr10:
	cli
	push	byte 10 ; Exception number. Error code is pushed by system.
	jmp		isr_common_stub

; 11: Segment Not Present.
global _isr11
_isr11:
	cli
	push	byte 11 ; Exception number. Error code is pushed by system.
	jmp		isr_common_stub

; 12: Stack Fault Exception.
global _isr12
_isr12:
	cli
	push	byte 12 ; Exception number. Error code is pushed by system.
	jmp		isr_common_stub

; 13: General Protection Exception.
global _isr13
_isr13:
	cli
	push	byte 13 ; Exception number. Error code is pushed by system.
	jmp		isr_common_stub

; 14: Page Fault Exception.
global _isr14
_isr14:
	cli
	push	byte 14 ; Exception number. Error code is pushed by system.
	jmp		isr_common_stub

; 15: Unknown Interrupt Exception
global _isr15
_isr15:
	cli
	push	byte 0
	push	byte 15
	jmp		isr_common_stub

; 16: x87 FPU Floating Point Error.
global _isr16
_isr16:
	cli
	push	byte 0 ; No error code.
	push	byte 16 ; Exception number.
	jmp		isr_common_stub

; 17: Alignment Check Exception.
global _isr17
_isr17:
	cli
	push	byte 0 ; No error code.
	push	byte 17 ; Exception number.
	jmp		isr_common_stub

; 18: Machine Check Exception.
global _isr18
_isr18:
	cli
	push	byte 0 ; No error code.
	push	byte 18 ; Exception number.
	jmp		isr_common_stub

; 19: SIMD Floating Point Exception.
global _isr19
_isr19:
	cli
	push	byte 0 ; No error code.
	push	byte 19 ; Exception number.
	jmp		isr_common_stub

; 20: Virtualization Exception.
global _isr20
_isr20:
	cli
	push	byte 0 ; No error code.
	push	byte 20 ; Exception number.
	jmp		isr_common_stub

global _isr21
_isr21:
	cli
	push	byte 0 ; No error code.
	push	byte 21 ; Exception number.
	jmp		isr_common_stub
	
global _isr22
_isr22:
	cli
	push	byte 0 ; No error code.
	push	byte 22 ; Exception number.
	jmp		isr_common_stub
	
global _isr23
_isr23:
	cli
	push	byte 0 ; No error code.
	push	byte 23 ; Exception number.
	jmp		isr_common_stub
	
global _isr24
_isr24:
	cli
	push	byte 0 ; No error code.
	push	byte 24 ; Exception number.
	jmp		isr_common_stub
	
global _isr25
_isr25:
	cli
	push	byte 0 ; No error code.
	push	byte 25 ; Exception number.
	jmp		isr_common_stub
	
global _isr26
_isr26:
	cli
	push	byte 0 ; No error code.
	push	byte 26 ; Exception number.
	jmp		isr_common_stub
	
global _isr27
_isr27:
	cli
	push	byte 0 ; No error code.
	push	byte 27 ; Exception number.
	jmp		isr_common_stub
	
global _isr28
_isr28:
	cli
	push	byte 0 ; No error code.
	push	byte 28 ; Exception number.
	jmp		isr_common_stub
	
global _isr29
_isr29:
	cli
	push	byte 0 ; No error code.
	push	byte 29 ; Exception number.
	jmp		isr_common_stub
	
global _isr30
_isr30:
	cli
	push	byte 0 ; No error code.
	push	byte 30 ; Exception number.
	jmp		isr_common_stub
	
global _isr31
_isr31:
	cli
	push	byte 0 ; No error code.
	push	byte 31 ; Exception number.
	jmp		isr_common_stub

extern _fault_handler
isr_common_stub:
	pusha
	push	ds
	push	es
	push	fs
	push	gs
	
	mov		ax, 0x10			; Load the Kernel Data Segment descriptor
	mov		ds, ax
	mov		es, ax
	mov		fs, ax
	mov		gs, ax
	
	mov		eax, esp			; Push the stack
	push	eax
	
	call	_fault_handler		; Call C exception handler
	pop		eax
	
	pop		gs
	pop		fs
	pop		es
	pop		ds
	popa
	
	add		esp, 8				; Cleans up the pushed error code and pushed ISR number
	iret						; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!