#include <pit.h>
#include <portio.h>
#include <irq.h>
#include <regs_t.h>

#include <stdio.h>

// This will keep track of how many ticks that the system has been running for
static volatile uint32_t pit_ticks;

/**
 *  \brief Inline function to send a command to the PIT command register
 *  
 *  \param [in] cmd The command to send to the PIT
 */
static inline void pit_send_command(uint8_t cmd) {
	out_port_byte(PIT_REG_COMMAND, cmd);
}

/**
 *  \brief Inline function for sending data to counter 0 of the PIT
 *  
 *  \param [in] data The data to send
 */
static inline void pit_send_data_counter0(uint8_t data) {
	out_port_byte(PIT_REG_COUNTER0, data);
}

/**
 *  \brief Inline function for sending data to counter 1 of the PIT
 *  
 *  \param [in] data The data to send
 */
static inline void pit_send_data_counter1(uint8_t data) {
	out_port_byte(PIT_REG_COUNTER1, data);
}

/**
 *  \brief Inline function for sending data to counter 2 of the PIT
 *  
 *  \param [in] data The data to send
 */
static inline void pit_send_data_counter2(uint8_t data) {
	out_port_byte(PIT_REG_COUNTER2, data);
}

// Receive

static inline uint8_t pit_receive_data_counter0() {
	return in_port_byte(PIT_REG_COUNTER0);
}

static inline uint8_t pit_receive_data_counter1() {
	return in_port_byte(PIT_REG_COUNTER1);
}

static inline uint8_t pit_receive_data_counter2() {
	return in_port_byte(PIT_REG_COUNTER2);
}

void pit_set_tick_speed(uint16_t hz) {
	if(hz == 0) {
		return;
	}
	
	/* if(hz > 1193180) {
		return;
	} */
	
	uint16_t divisor = 1193180 / hz;
	
	uint8_t cmd = 0;
	cmd |= PIT_OCW_BINARY_COUNT_BINARY;				// Going to count in binary
	cmd |= PIT_OCW_MODE_SQUARE_WAVE_GENERATOR;		// With a square wave
	cmd |= PIT_OCW_READ_LOAD_DATA;					// Going to load LSB first then MSB
	cmd |= PIT_OCW_SELECT_COUNTER_0;				// This is for counter 0
	
	pit_send_command(cmd);							// Send the command
	
	// Send the divisor to the PIT
	pit_send_data_counter0(divisor & 0xFF);			// Set the lower half
	pit_send_data_counter0((divisor >> 8) & 0xff);	// Set the upper half
}

/* Handles the PIT. In this case, it's very simple: We
 * increment the 'pit_ticks' variable every time the
 * pit fires. By default, the pit fires 18.222 times
 * per second.
 */
void pit_handler(regs_t * regs) {
	(void) regs;
	// Increment tick count
	pit_ticks++;
}

uint32_t get_pit_ticks() {
	return pit_ticks;
}

// Sets up the system clock by installing the timer handler into IRQ0
void pit_install() {
	// Initialise the ticks to zero
	pit_ticks = 0;
	
	// Installs 'pit_handler' to IRQ0
	irq_install_handler(0, pit_handler);
}

// This will continuously loop until the given time has been reached
void pit_wait(int tick) {
	uint32_t eticks = pit_ticks + tick; //(sec * 18);
	while(pit_ticks < eticks) {
		__asm__ __volatile__ ("sti");
		__asm__ __volatile__ ("hlt");
		__asm__ __volatile__ ("cli");
	}
}
