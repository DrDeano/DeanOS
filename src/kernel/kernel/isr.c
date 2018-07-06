#include <stdint.h>
#include <stdio.h>

#include <idt.h>
#include <isr.h>
#include <main.h>

extern void _isr00();
extern void _isr01();
extern void _isr02();
extern void _isr03();
extern void _isr04();
extern void _isr05();
extern void _isr06();
extern void _isr07();
extern void _isr08();
extern void _isr09();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

static char * exception_msg[] = {
	"Divide By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"x87 FPU Floating Point Error",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating Point",
	"Virtualization",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Security",
	"Reserved"
};

static isr_handler isr_handlers[ISR_TOTAL] = {
	0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

void _fault_handler(regs_t * regs) {
	// Get the handler
	isr_handler handler = isr_handlers[regs->int_num];
	
	// Is there a handler defined for the exception
	if (handler) {
		handler(regs);
	} else { // Else panic
		panic("CPU: Exception: %s (error code: %p)\n", exception_msg[regs->int_num], regs->error_code);
	}
}

void isr_install_handler(uint8_t isr_num, isr_handler handler) {
	isr_handlers[isr_num] = handler;
	kprintf("ISR Handle for %u installed\n", isr_num);
}

void isr_uninstall_handler(uint8_t isr_num) {
	isr_handlers[isr_num] = 0;
	kprintf("ISR Handle for %u removed\n", isr_num);
}

void isr_init() {
    kprintf("Initialising iterrupt service routines\n");
	idt_open_interrupt_gate(0, (uintptr_t) &_isr00);
	idt_open_interrupt_gate(1, (uintptr_t) &_isr01);
	idt_open_interrupt_gate(2, (uintptr_t) &_isr02);
	idt_open_interrupt_gate(3, (uintptr_t) &_isr03);
	idt_open_interrupt_gate(4, (uintptr_t) &_isr04);
	idt_open_interrupt_gate(5, (uintptr_t) &_isr05);
	idt_open_interrupt_gate(6, (uintptr_t) &_isr06);
	idt_open_interrupt_gate(7, (uintptr_t) &_isr07);
	idt_open_interrupt_gate(8, (uintptr_t) &_isr08);
	idt_open_interrupt_gate(9, (uintptr_t) &_isr09);
	idt_open_interrupt_gate(10, (uintptr_t) &_isr10);
	idt_open_interrupt_gate(11, (uintptr_t) &_isr11);
	idt_open_interrupt_gate(12, (uintptr_t) &_isr12);
	idt_open_interrupt_gate(13, (uintptr_t) &_isr13);
	idt_open_interrupt_gate(14, (uintptr_t) &_isr14);
	idt_open_interrupt_gate(15, (uintptr_t) &_isr15);
	idt_open_interrupt_gate(16, (uintptr_t) &_isr16);
	idt_open_interrupt_gate(17, (uintptr_t) &_isr17);
	idt_open_interrupt_gate(18, (uintptr_t) &_isr18);
	idt_open_interrupt_gate(19, (uintptr_t) &_isr19);
	idt_open_interrupt_gate(20, (uintptr_t) &_isr20);
	idt_open_interrupt_gate(21, (uintptr_t) &_isr21);
	idt_open_interrupt_gate(22, (uintptr_t) &_isr22);
	idt_open_interrupt_gate(23, (uintptr_t) &_isr23);
	idt_open_interrupt_gate(24, (uintptr_t) &_isr24);
	idt_open_interrupt_gate(25, (uintptr_t) &_isr25);
	idt_open_interrupt_gate(26, (uintptr_t) &_isr26);
	idt_open_interrupt_gate(27, (uintptr_t) &_isr27);
	idt_open_interrupt_gate(28, (uintptr_t) &_isr28);
	idt_open_interrupt_gate(29, (uintptr_t) &_isr29);
	idt_open_interrupt_gate(20, (uintptr_t) &_isr30);
	idt_open_interrupt_gate(31, (uintptr_t) &_isr31);
}