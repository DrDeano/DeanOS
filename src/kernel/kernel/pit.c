#include <pit.h>
#include <pic.h>
#include <portio.h>
#include <irq.h>
#include <regs_t.h>

#include <stdio.h>

/**
 *  \brief The number of tick that has passed when the PIT was initially set up.
 */
static volatile uint32_t pit_ticks;

/**
 *  \brief The frequency the PIT runs at.
 */
static uint16_t frequency;

/**
 *  \brief Inline function to send a command to the PIT command register.
 *
 *  \param [in] cmd The command to send to the PIT.
 */
static inline void pit_send_command(uint8_t cmd) {
	out_port_byte(PIT_REG_COMMAND, cmd);
}

/**
 *  \brief Inline function for sending data to counter of the PIT. Will be only one of the 3
 *  counters as is an internal function.
 *
 *  \param [in] data The data to send.
 */
static inline void pit_send_data(uint8_t counter, uint8_t data) {
	out_port_byte(counter, data);
}

/**
 *  \brief Inline function for receiving data from a counter of the PIT. Will be only one of the 3
 *  counters as is an internal function.
 *
 *  \param [in] counter The counter number to get the data from.
 *  \return The data from the counter register.
 */
static inline uint8_t pit_receive_data_counter(uint8_t counter) {
	return in_port_byte(counter);
}

void pit_setup_counter(uint16_t freq, uint8_t counter, uint8_t mode) {
	if(freq == 0) {
		return;
	}

	uint8_t port;

	if(counter == PIT_OCW_SELECT_COUNTER_0) {
		port = PIT_REG_COUNTER_0;
	} else if (counter == PIT_OCW_SELECT_COUNTER_1) {
		port = PIT_REG_COUNTER_1;
	} else if (counter == PIT_OCW_SELECT_COUNTER_2) {
		port = PIT_REG_COUNTER_2;
	} else {
		return;
	}

	uint16_t divisor = 1193180 / freq;
	frequency = freq;

	uint8_t cmd = 0;
	cmd |= mode;
	cmd |= PIT_OCW_READ_LOAD_DATA;		// Going to load LSB first then MSB
	cmd |= counter;

	pit_send_command(cmd);							// Send the command

	// Send the divisor to the PIT
	pit_send_data(port, divisor & 0xFF);			// Set the lower half
	pit_send_data(port, (divisor >> 8) & 0xff);		// Set the upper half

	/**
	 *  \todo Add support of more counter variables
	 */
	pit_ticks = 0;		// Reset the tick counter
}

/**
 *  \brief The PIT handler that is called when the PIT creates an interrupt.
 *
 *  \param [in] regs The register of the CPU when the interrupt was called
 */
static void pit_handler(regs_t * regs) {
	(void) regs;		// Not using the registers
	pit_ticks++;		// Increment tick count
}

uint32_t get_pit_ticks(void) {
	return pit_ticks;
}

uint16_t get_pit_frequency(void) {
	return frequency;
}

void pit_install(void) {
	// Set up counter 0 at 1000hz in a square wave mode counting in binary
	frequency = 1000;
	pit_setup_counter(frequency, PIT_OCW_SELECT_COUNTER_0, PIT_OCW_MODE_SQUARE_WAVE_GENERATOR | PIT_OCW_BINARY_COUNT_BINARY);

	// Installs 'pit_handler' to IRQ0 (PIC_IRQ_TIMER)
	irq_install_handler(PIC_IRQ_TIMER, pit_handler);
}

void pit_wait(int milliseconds) {
	/**
	 *  \todo Have a check so not to over flow
	 */
	uint32_t eticks = pit_ticks + milliseconds;
	while(pit_ticks < eticks) {
		__asm__ __volatile__ ("sti");
		__asm__ __volatile__ ("hlt");
		__asm__ __volatile__ ("cli");
	}
}
