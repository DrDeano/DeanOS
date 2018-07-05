#include <timer.h>
#include <portio.h>
#include <irq.h>
#include <regs_t.h>

#include <stdio.h>

void timer_phase(int hz) {
	int divisor = 1193180 / hz;				// Calculate our divisor
	out_port_byte(0x43, 0x36);				// Set our command byte 0x36
	out_port_byte(0x40, divisor & 0xFF);	// Set low byte of divisor
	out_port_byte(0x40, divisor >> 8);		// Set high byte of divisor
}

// This will keep track of how many ticks that the system has been running for
volatile unsigned int timer_ticks = 0;

/* Handles the timer. In this case, it's very simple: We
 * increment the 'timer_ticks' variable every time the
 * timer fires. By default, the timer fires 18.222 times
 * per second. Why 18.222Hz? Some engineer at IBM must've
 * been smoking something funky
 */
void timer_handler(regs_t * regs) {
	(void) regs;
	// Increment our 'tick count'
	timer_ticks++;

	// Every 18 clocks (approximately 1 second), we will display a message on the screen
	/* if (timer_ticks % 18 == 0) {
		kputchar('.');
	} */
}

unsigned int get_timer_ticks() {
	return timer_ticks;
}

// Sets up the system clock by installing the timer handler into IRQ0
void timer_install() {
	// Installs 'timer_handler' to IRQ0
	irq_install_handler(0, timer_handler);
}

// This will continuously loop until the given time has been reached
void timer_wait(int sec) {
	unsigned int eticks = timer_ticks + (sec * 18);
	while(timer_ticks < eticks) {
		__asm__ __volatile__ ("sti");
		__asm__ __volatile__ ("hlt");
		__asm__ __volatile__ ("cli");
	}
}
