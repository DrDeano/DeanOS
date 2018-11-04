#include <stdint.h>

#include <idt.h>
#include <isr.h>
#include <panic.h>

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

/**
 * \brief A list of string that say what exception has been raised so can be printed to the user so
 * that they know what happened.
 */
static char * exception_msg[] = {
	"Divide By Zero",
	"Single Step (Debugger)",
	"Non Maskable Interrupt",
	"Breakpoint (Debugger)",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"No Coprocessor, Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid Task State Segment (TSS)",
	"Segment Not Present",
	"Stack Segment Overrun",
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

/**
 *  \brief The list of handlers for each exception.
 */
static isr_handler isr_handlers[ISR_TOTAL] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * \brief The main handler that is called when a exception is raises. It then calls the appropriate
 * handler for the exception if one is present.
 * 
 * \param [in] regs The registers
 */
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
}

void isr_uninstall_handler(uint8_t isr_num) {
	isr_handlers[isr_num] = 0;
}

void isr_init(void) {
	idt_open_interrupt_gate(0, (uint32_t) &_isr00);
	idt_open_interrupt_gate(1, (uint32_t) &_isr01);
	idt_open_interrupt_gate(2, (uint32_t) &_isr02);
	idt_open_interrupt_gate(3, (uint32_t) &_isr03);
	idt_open_interrupt_gate(4, (uint32_t) &_isr04);
	idt_open_interrupt_gate(5, (uint32_t) &_isr05);
	idt_open_interrupt_gate(6, (uint32_t) &_isr06);
	idt_open_interrupt_gate(7, (uint32_t) &_isr07);
	idt_open_interrupt_gate(8, (uint32_t) &_isr08);
	idt_open_interrupt_gate(9, (uint32_t) &_isr09);
	idt_open_interrupt_gate(10, (uint32_t) &_isr10);
	idt_open_interrupt_gate(11, (uint32_t) &_isr11);
	idt_open_interrupt_gate(12, (uint32_t) &_isr12);
	idt_open_interrupt_gate(13, (uint32_t) &_isr13);
	idt_open_interrupt_gate(14, (uint32_t) &_isr14);
	idt_open_interrupt_gate(15, (uint32_t) &_isr15);
	idt_open_interrupt_gate(16, (uint32_t) &_isr16);
	idt_open_interrupt_gate(17, (uint32_t) &_isr17);
	idt_open_interrupt_gate(18, (uint32_t) &_isr18);
	idt_open_interrupt_gate(19, (uint32_t) &_isr19);
	idt_open_interrupt_gate(20, (uint32_t) &_isr20);
	idt_open_interrupt_gate(21, (uint32_t) &_isr21);
	idt_open_interrupt_gate(22, (uint32_t) &_isr22);
	idt_open_interrupt_gate(23, (uint32_t) &_isr23);
	idt_open_interrupt_gate(24, (uint32_t) &_isr24);
	idt_open_interrupt_gate(25, (uint32_t) &_isr25);
	idt_open_interrupt_gate(26, (uint32_t) &_isr26);
	idt_open_interrupt_gate(27, (uint32_t) &_isr27);
	idt_open_interrupt_gate(28, (uint32_t) &_isr28);
	idt_open_interrupt_gate(29, (uint32_t) &_isr29);
	idt_open_interrupt_gate(20, (uint32_t) &_isr30);
	idt_open_interrupt_gate(31, (uint32_t) &_isr31);
}
