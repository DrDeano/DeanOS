	[bits	32]
	section .text

;
; Interrupt Requests
;

; 32: irq00
global _irq00
_irq00:
	cli
	push	byte 0
	push	byte 32
	jmp		irq_common_stub

; 33: irq01
global _irq01
_irq01:
	cli
	push	byte 0
	push	byte 33
	jmp		irq_common_stub

; 34: irq02
global _irq02
_irq02:
	cli
	push	byte 0
	push	byte 34
	jmp		irq_common_stub

; 35: irq03
global _irq03
_irq03:
	cli
	push	byte 0
	push	byte 35
	jmp		irq_common_stub
	
; 36: irq04
global _irq04
_irq04:
	cli
	push	byte 0
	push	byte 36
	jmp		irq_common_stub

; 37: irq05
global _irq05
_irq05:
	cli
	push	byte 0
	push	byte 37
	jmp		irq_common_stub
	
; 38: irq06
global _irq06
_irq06:
	cli
	push	byte 0
	push	byte 38
	jmp		irq_common_stub

; 39: irq07
global _irq07
_irq07:
	cli
	push	byte 0
	push	byte 39
	jmp		irq_common_stub
	
; 40: irq08
global _irq08
_irq08:
	cli
	push	byte 0
	push	byte 40
	jmp		irq_common_stub

; 41: irq09
global _irq09
_irq09:
	cli
	push	byte 0
	push	byte 41
	jmp		irq_common_stub
	
; 42: irq10
global _irq10
_irq10:
	cli
	push	byte 0
	push	byte 42
	jmp		irq_common_stub

; 43: irq11
global _irq11
_irq11:
	cli
	push	byte 0
	push	byte 43
	jmp		irq_common_stub
	
; 44: irq12
global _irq12
_irq12:
	cli
	push	byte 0
	push	byte 44
	jmp		irq_common_stub

; 45: irq13
global _irq13
_irq13:
	cli
	push	byte 0
	push	byte 45
	jmp		irq_common_stub
	
; 46: irq14
global _irq14
_irq14:
	cli
	push	byte 0
	push	byte 46
	jmp		irq_common_stub

; 47: irq15
global _irq15
_irq15:
	cli
	push	byte 0
	push	byte 47
	jmp		irq_common_stub

extern _irq_handler
irq_common_stub:
	pusha
    push	ds
    push	es
    push	fs
    push	gs
    mov		ax, 0x10
    mov		ds, ax
    mov		es, ax
    mov		fs, ax
    mov		gs, ax
    mov		eax, esp
    push	eax
    mov		eax, _irq_handler
    call	eax
    pop		eax
    pop		gs
    pop		fs
    pop		es
    pop		ds
    popa
    add		esp, 8
    iret
