/**
 *  \file pit.h
 *  \brief Functions, definitions and structures for setting up the Programmable Interval Timer.
 */
#ifndef INCLUDE_PIT_H
#define INCLUDE_PIT_H

#include <stdint.h>

/**
 *  \brief The port addresses of the PIT registers.
 */
enum pit_registers {
	PIT_REG_COUNTER_0	= 0x40,	/**< The port address for the PIT data register for counter 0. This is going to be used as the system clock. */
	PIT_REG_COUNTER_1	= 0x41,	/**< The port address for the PIT data register for counter 1. This was used for refreshing the DRAM chips. But now is unused and unknown use, so won't use. */
	PIT_REG_COUNTER_2	= 0x42,	/**< The port address for the PIT data register for counter 2. Connected to the PC speakers, we'll use this for the speakers. */
	PIT_REG_COMMAND		= 0x43	/**< The port address for the PIT control word register. Used to tell the PIT controller what is about to happen. Tell what data is going where, lower or upper part of it's registers. */
};

/**
 *  \brief The operational command word marks for the different modes.
 *  
 *  Bit 0: (BCP) Binary Counter.\n
 *  	0: Binary.\n
 *  	1: Binary Coded Decimal (BCD).\n
 *  Bit 1-3: (M0, M1, M2) Operating Mode. See above sections for a description of each.\n
 *  	000: Mode 0: Interrupt or Terminal Count.\n
 *  	001: Mode 1: Programmable one-shot.\n
 *  	010: Mode 2: Rate Generator.\n
 *  	011: Mode 3: Square Wave Generator.\n
 *  	100: Mode 4: Software Triggered Strobe.\n
 *  	101: Mode 5: Hardware Triggered Strobe.\n
 *  	110: Undefined; Don't use.\n
 *  	111: Undefined; Don't use.\n
 *  Bits 4-5: (RL0, RL1) Read/Load Mode. We are going to read or send data to a counter register.\n
 *  	00: Counter value is latched into an internal control register at the time of the I/O write operation.\n
 *  	01: Read or Load Least Significant Byte (LSB) only.\n
 *  	10: Read or Load Most Significant Byte (MSB) only.\n
 *  	11: Read or Load LSB first then MSB.\n
 *  Bits 6-7: (SC0-SC1) Select Counter. See above sections for a description of each.\n
 *  	00: Counter 0.\n
 *  	01: Counter 1.\n
 *  	10: Counter 2.\n
 *  	11: Illegal value.\n
 */
enum pit_ocw_counting_modes {
	PIT_OCW_MASK_BINARY_COUNT			= 0x01,	/**< xxxxxxx1 | The OCW mask for the counting mode. */
	PIT_OCW_MASK_MODE					= 0x0E,	/**< xxxx111x | The OCW mask for the operating mode. */
	PIT_OCW_MASK_READ_LOAD				= 0x30,	/**< xx11xxxx | The OCW mask for the reading mode. */
	PIT_OCW_MASK_SELECT_COUNTER			= 0xC0	/**< 11xxxxxx | The OCW mask for the selecting counter. */
};

/**
 *  \brief The operational command word for the counters. The specific control bits for setting the PIT.
 */
enum pit_ocw_counters {
	PIT_OCW_BINARY_COUNT_BINARY			= 0x00,	/**< xxxxxxx0 | Have the counter count in binary (internally?). */
	PIT_OCW_BINARY_COUNT_BCD			= 0x01,	/**< xxxxxxx1 | Have the counter count in BCD (internally?). */
	PIT_OCW_MODE_TERMINAL_COUNT			= 0x00,	/**< xxxx000x | The PIT counter will be programmed with an initial COUNT value that counts down at a
												  *  rate of the input clock frequency. When the COUNT reaches 0, and after the control word is 
												  *  written, then its OUT pit is set high (1). Count down starts then the COUNT is set. The OUT pin
												  *  remains high until the counter is reloaded with a new COUNT value or a new control work is written. */
	
	PIT_OCW_MODE_ONE_SHOT				= 0x02,	/**< xxxx001x | The counter is programmed to output a pulse every curtain number of clock pulses. The
												  *  OUT pin remains high as soon as a control word is written. When COUNT is written, the counter
												  *  waits until the rising edge of the GATE pin to start.  One clock pulse after the GATE pin, the
												  *  OUT pin will remain low until COUNT reaches 0. */
	
	PIT_OCW_MODE_RATE_GENERATOR			= 0x04,	/**< xxxx010x | The counter is initiated with a COUNT value. Counting starts next clock pulse. OUT pin
												  *  remains high until COUNT reaches 1, then is set low for one clock pulse. Then COUNT is reset
												  *  back to initial value and OUT pin is set high again. */
	
	PIT_OCW_MODE_SQUARE_WAVE_GENERATOR	= 0x06,	/**< xxxx011x | Similar to PIT_OCW_MODE_RATE_GENERATOR, but OUT pin will be high for half the time and low for
												  *  half the time. Good for the speaker when setting a tone. */
	
	PIT_OCW_MODE_SOFTWARE_TRIGGER		= 0x08,	/**< xxxx100x | The counter is initiated with a COUNT value. Counting starts on next clock pulse. OUT pin remains high
												  *  until count is 0. Then OUT pin is low for one clock pulse. Then resets to high again. */
	
	PIT_OCW_MODE_HARDWARE_TRIGGER		= 0x0A,	/**< xxxx101x | The counter is initiated with a COUNT value. OUT pin remains high until the rising edge
												  *  of the GATE pin. Then the counting begins. When COUNT reaches 0, OUT pin goes low for one clock
												  *  pulse. Then COUNT is reset and OUT pin goes high. This cycles for each rising edge of the GATE pin. */
	
	PIT_OCW_READ_LOAD_LATCH				= 0x00,	/**< xx00xxxx | The counter value is latched into an internal control register at the time of the I/O write operations. */
	PIT_OCW_READ_LOAD_LSB_ONLY			= 0x10,	/**< xx01xxxx | Read or load the most significant bit only. */
	PIT_OCW_READ_LOAD_MSB_ONLY			= 0x20,	/**< xx10xxxx | Read or load the least significant bit only. */
	PIT_OCW_READ_LOAD_DATA				= 0x30,	/**< xx11xxxx | Read or load the least significant bit first then the most significant bit. */
	PIT_OCW_SELECT_COUNTER_0			= 0x00,	/**< 00xxxxxx | The OCW bits for selecting counter 0. Used for the system clock. */
	PIT_OCW_SELECT_COUNTER_1			= 0x40,	/**< 01xxxxxx | The OCW bits for selecting counter 1. Was for the memory refreshing. */
	PIT_OCW_SELECT_COUNTER_2			= 0x80	/**< 10xxxxxx | The OCW bits for selecting counter 2. Channel for the speaker. */
};

/**
 *  \brief Set up a counter with a tick rate and mode of operation
 *  
 *  \param [in] freq The frequency that the counter operates at.
 *  \param [in] counter Which counter is to be set up, either \ref PIT_OCW_SELECT_COUNTER_0, \ref PIT_OCW_SELECT_COUNTER_1 or \ref PIT_OCW_SELECT_COUNTER_2 only.
 *  \param [in] mode The mode of operation that the counter will operate in. See The modes
 *  definition above to chose which mode the counter is to run at.
 */
void pit_setup_counter(uint16_t freq, uint8_t counter, uint8_t mode);

/**
 *  \brief A simple wait that used the PIT to wait a number of ticks
 *  
 *  \param [in] milliseconds The number of ticks to wait
 */
void pit_wait(uint32_t milliseconds);

/**
 *  \brief Get the number of ticks that have passed when the PIT was initiated.
 *  
 *  \return Number of ticks passed
 */
uint32_t pit_get_ticks(void);

/**
 *  \brief Get the divisor used by the PIT when initiated.
 *  
 *  \return The divisor
 */
uint16_t pit_get_frequency(void);

/**
 *  \brief Initialise the PIT with a handler to a IRQ.
 */
void pit_init(void);

#endif /* INCLUDE_PIT_H */
