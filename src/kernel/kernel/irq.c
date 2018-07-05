#include <irq.h>
#include <portio.h>
#include <idt.h>

#include <stdio.h>

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

static irq_handler irq_handlers[IRQ_TOTAL] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

// This installs a custom IRQ handler for the given IRQ
void irq_install_handler(int irq, irq_handler handler) {
	irq_handlers[irq] = handler;
	kprintf("IRQ Handle for %u installed\n", irq);
}

// This clears the handler for a given IRQ
void irq_uninstall_handler(int irq) {
	irq_handlers[irq] = 0;
	kprintf("IRQ Handle for %u uninstalled\n", irq);
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
 * is a problem in protected mode, because IDT entry 8 is a
 * Double Fault! Without remapping, every time IRQ0 fires,
 * you get a Double Fault Exception, which is NOT actually
 * what's happening. We send commands to the Programmable
 * Interrupt Controller (PIC) in order to make IRQ0 to 15
 * be remapped to IDT entries 32 to 47
 */
static void irq_remap(void) {
	out_port_byte(0x20, 0x11);
	out_port_byte(0xA0, 0x11);
	out_port_byte(0x21, 0x20);
	out_port_byte(0xA1, 0x28);
	out_port_byte(0x21, 0x04);
	out_port_byte(0xA1, 0x02);
	out_port_byte(0x21, 0x01);
	out_port_byte(0xA1, 0x01);
	out_port_byte(0x21, 0x0);
	out_port_byte(0xA1, 0x0);
}

/* Each of the IRQ ISRs point to this function, rather than
 * the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
 * to be told when you are done servicing them, so you need
 * to send them an "End of Interrupt" command (0x20). There
 * are two 8259 chips: The first exists at 0x20, the second
 * exists at 0xA0. If the second controller (an IRQ from 8 to
 * 15) gets an interrupt, you need to acknowledge the
 * interrupt at BOTH controllers, otherwise, you only send
 * an EOI command to the first controller. If you don't send
 * an EOI, you won't raise any more IRQs
 */
void _irq_handler(regs_t * regs) {
	// Get the handler
	irq_handler handler = irq_handlers[regs->int_num - 32];

	// Run the handler if got one
	if (handler) {
		handler(regs);
	}

	// If the IDT entry that was invoked was greater than 40 (meaning IRQ8 - 15), then we need to send an EOI to the slave controller
	if (regs->int_num >= 40) {
		out_port_byte(0xA0, 0x20);
	}

	// In either case, we need to send an EOI to the master interrupt controller too
	out_port_byte(0x20, 0x20);
}

/* We first remap the interrupt controllers, and then we install
 * the appropriate ISRs to the correct entries in the IDT. This
 * is just like installing the exception handlers
 */
void irq_init() {
	kprintf("Initialising iterrupt requests\n");

	irq_remap();

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
