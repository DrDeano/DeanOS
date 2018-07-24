#include <irq.h>
#include <portio.h>
#include <idt.h>
#include <pic.h>

extern void _irq00();
extern void _irq01();
extern void _irq02();
extern void _irq03();
extern void _irq04();
extern void _irq05();
extern void _irq06();
extern void _irq07();
extern void _irq08();
extern void _irq09();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

/**
 *  The list of handlers for each IRQ
 */
static irq_handler irq_handlers[IRQ_TOTAL] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq_num, irq_handler handler) {
	irq_handlers[irq_num] = handler;
}

void irq_uninstall_handler(int irq_num) {
	irq_handlers[irq_num] = 0;
}
 
/**
 *  \brief Each of the IRQ ISR's point to this function, rather than
 *  the 'fault_handler' in 'isr.c'. The IRQ Controllers need
 *  to be told when you are done servicing them, so you need
 *  to send them an "End of Interrupt" command (0x20).
 *  
 *  \param [in] regs The registers
 */
void _irq_handler(regs_t * regs) {
	uint8_t irq_num = regs->int_num - 32;
	
	// Get the handler
	irq_handler handler = irq_handlers[irq_num];

	// Run the handler if got one
	if (handler) {
		handler(regs);
	}
	
	// Send the end of interrupt command
	pic_send_end_of_interrupt(irq_num);
}

void irq_init(void) {
	// Remap the PIC IRQ so not to overlap with other exceptions
	pic_remap_irq();

	idt_open_interrupt_gate(32, (uintptr_t) &_irq00);
	idt_open_interrupt_gate(33, (uintptr_t) &_irq01);
	idt_open_interrupt_gate(34, (uintptr_t) &_irq02);
	idt_open_interrupt_gate(35, (uintptr_t) &_irq03);
	idt_open_interrupt_gate(36, (uintptr_t) &_irq04);
	idt_open_interrupt_gate(37, (uintptr_t) &_irq05);
	idt_open_interrupt_gate(38, (uintptr_t) &_irq06);
	idt_open_interrupt_gate(39, (uintptr_t) &_irq07);
	idt_open_interrupt_gate(40, (uintptr_t) &_irq08);
	idt_open_interrupt_gate(41, (uintptr_t) &_irq09);
	idt_open_interrupt_gate(42, (uintptr_t) &_irq10);
	idt_open_interrupt_gate(43, (uintptr_t) &_irq11);
	idt_open_interrupt_gate(44, (uintptr_t) &_irq12);
	idt_open_interrupt_gate(45, (uintptr_t) &_irq13);
	idt_open_interrupt_gate(46, (uintptr_t) &_irq14);
	idt_open_interrupt_gate(47, (uintptr_t) &_irq15);
}
