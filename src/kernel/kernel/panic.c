#include <panic.h>
#include <interrupt.h>

#include <stdnoreturn.h>
#include <stdio.h>

noreturn void panic(const char * format, ...) {
	// Disable interrupts.
	interrupt_disable();
	
    // Get args.
    va_list args;
	va_start(args, format);
	
	// Show panic.
	kprintf("\nPANIC:\n");
	kvprintf(format, args);
	kprintf("\nHalted");
	
	va_end(args);
	
	// Halt forever.
	while(1) {
		__asm__ __volatile__ ("sti");
		__asm__ __volatile__ ("hlt");
		__asm__ __volatile__ ("cli");
	}
	__builtin_unreachable();
}
