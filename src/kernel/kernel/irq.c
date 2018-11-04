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
 * \brief The list of handlers for each IRQ.
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
 * \brief Each of the IRQ ISR's point to this function, rather than the 'fault_handler' in 'isr.c'.
 * The IRQ Controllers need to be told when you are done servicing them, so you need to send them
 * an "End of Interrupt" command (0x20).
 * 
 * \param [in] regs The registers when this is called.
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

void irq_set_mask(uint8_t irq_num) {
	uint16_t port;
	uint8_t value;
	
	if(irq_num < 8) {
		port = PIC_INTERRUPT_MASK_REG_MASTER;
	} else {
		port = PIC_INTERRUPT_MASK_REG_SLAVE;
		irq_num -= 8;
	}
	
	value = in_port_byte(port) | (1 << irq_num);
	out_port_byte(port, value);
}

void irq_clear_mask(uint8_t irq_num) {
	uint16_t port;
	uint8_t value;
	
	if(irq_num < 8) {
		port = PIC_INTERRUPT_MASK_REG_MASTER;
	} else {
		port = PIC_INTERRUPT_MASK_REG_SLAVE;
		irq_num -= 8;
	}
	
	value = in_port_byte(port) & ~(1 << irq_num);
	out_port_byte(port, value);
}

void irq_init(void) {
	// Remap the PIC IRQ so not to overlap with other exceptions
	pic_remap_irq();
	
	// Open all the IRQ's
	idt_open_interrupt_gate(32, (uint32_t) &_irq00);
	idt_open_interrupt_gate(33, (uint32_t) &_irq01);
	idt_open_interrupt_gate(34, (uint32_t) &_irq02);
	idt_open_interrupt_gate(35, (uint32_t) &_irq03);
	idt_open_interrupt_gate(36, (uint32_t) &_irq04);
	idt_open_interrupt_gate(37, (uint32_t) &_irq05);
	idt_open_interrupt_gate(38, (uint32_t) &_irq06);
	idt_open_interrupt_gate(39, (uint32_t) &_irq07);
	idt_open_interrupt_gate(40, (uint32_t) &_irq08);
	idt_open_interrupt_gate(41, (uint32_t) &_irq09);
	idt_open_interrupt_gate(42, (uint32_t) &_irq10);
	idt_open_interrupt_gate(43, (uint32_t) &_irq11);
	idt_open_interrupt_gate(44, (uint32_t) &_irq12);
	idt_open_interrupt_gate(45, (uint32_t) &_irq13);
	idt_open_interrupt_gate(46, (uint32_t) &_irq14);
	idt_open_interrupt_gate(47, (uint32_t) &_irq15);
}
