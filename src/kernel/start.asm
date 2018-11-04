	[bits	32]

	[section	.text]
	
	[global start_kernel]
	
extern	kernel_main

start_kernel:
	jmp		long kernel_main
halt:
	cli
	hlt
	jmp		halt