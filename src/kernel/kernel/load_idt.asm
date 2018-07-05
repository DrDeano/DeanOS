	[bits	32]
	
	section	.text
	
global _idt_load
_idt_load:
	mov		eax, [esp + 4]
	
	lidt	[eax]
	ret