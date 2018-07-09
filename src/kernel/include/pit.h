/**
 *  \file pit.h
 *  \brief The Programmable Interval Timer
 */
#ifndef INCLUDE_PIT_H
#define INCLUDE_PIT_H

#include <stdint.h>

/**
 * \def PIT_REG_COUNTER0
 * \brief The port address for the PIT data register for counter 0. This is going to be used as the
 * system clock.
 */
#define PIT_REG_COUNTER0	0x40

/**
 * \def PIT_REG_COUNTER1
 * \brief The port address for the PIT data register for counter 1. This was used for refreshing
 * the DRAM chips. But now is unused and unknown use, so won't use.
 */
#define PIT_REG_COUNTER1	0x41

/**
 * \def PIT_REG_COUNTER2
 * \brief The port address for the PIT data register for counter 2. Connected to the PC speakers,
 * we'll use this for the speakers.
 */
#define PIT_REG_COUNTER2	0x42

/**
 * \def PIT_REG_COMMAND
 * \brief The port address for the PIT control word register. Used to tell the PIT controller what
 * is about to happen. Tell what data is going where, lower or upper part of it's registers.
 */
#define PIT_REG_COMMAND		0x43


// Operational command word Bit masks

/*
Bit 0: (BCP) Binary Counter
	0: Binary
	1: Binary Coded Decimal (BCD)
Bit 1-3: (M0, M1, M2) Operating Mode. See above sections for a description of each.
	000: Mode 0: Interrupt or Terminal Count
	001: Mode 1: Programmable one-shot
	010: Mode 2: Rate Generator
	011: Mode 3: Square Wave Generator
	100: Mode 4: Software Triggered Strobe
	101: Mode 5: Hardware Triggered Strobe
	110: Undefined; Don't use
	111: Undefined; Don't use
Bits 4-5: (RL0, RL1) Read/Load Mode. We are going to read or send data to a counter register
	00: Counter value is latched into an internal control register at the time of the I/O write operation.
	01: Read or Load Least Significant Byte (LSB) only
	10: Read or Load Most Significant Byte (MSB) only
	11: Read or Load LSB first then MSB
Bits 6-7: (SC0-SC1) Select Counter. See above sections for a description of each.
	00: Counter 0
	01: Counter 1
	10: Counter 2
	11: Illegal value
*/


#define PIT_OCW_MASK_BINARY_COUNT			1		//00000001
#define PIT_OCW_MASK_MODE					0xE		//00001110
#define PIT_OCW_MASK_READ_LOAD				0x30	//00110000
#define PIT_OCW_MASK_SELECT_COUNTER			0xC0	//11000000

// The specific control bits for setting the PIT

#define PIT_OCW_BINARY_COUNT_BINARY			0		//xxxxxxx0
#define PIT_OCW_BINARY_COUNT_BCD			1		//xxxxxxx1

#define PIT_OCW_MODE_TERMINAL_COUNT			0		//xxxx000x
#define PIT_OCW_MODE_PROGRAMMABLE_ONE_SHOT	0x2		//xxxx001x
#define PIT_OCW_MODE_RATE_GENERATOR			0x4		//xxxx010x
#define PIT_OCW_MODE_SQUARE_WAVE_GENERATOR	0x6		//xxxx011x
#define PIT_OCW_MODE_SOFTWARE_TRIGGER		0x8		//xxxx100x
#define PIT_OCW_MODE_HARDWARE_TRIGGER		0xA		//xxxx101x

#define PIT_OCW_READ_LOAD_LATCH				0		//xx00xxxx
#define PIT_OCW_READ_LOAD_LSBONLY			0x10	//xx01xxxx
#define PIT_OCW_READ_LOAD_MSBONLY			0x20	//xx10xxxx
#define PIT_OCW_READ_LOAD_DATA				0x30	//xx11xxxx

#define PIT_OCW_SELECT_COUNTER_0			0		//00xxxxxx
#define PIT_OCW_SELECT_COUNTER_1			0x40	//01xxxxxx
#define PIT_OCW_SELECT_COUNTER_2			0x80	//10xxxxxx

/**
 *  \brief Set the tick rate of the PIT in hertz.
 *  
 *  \param [in] hz The number of hertz the PIT is to be run at
 *  
 *  \todo Add ability to change the mode of the PIT
 */
void pit_set_tick_speed(uint16_t hz);

/**
 *  \brief A simple wait that used the PIT to wait a number of ticks
 *  
 *  \param [in] tick The number of ticks to wait
 *  
 *  \details More details
 */
void pit_wait(int tick);

/**
 *  \brief Initialise the PIT with a handler to a IRQ.
 *  
 *  \details More details
 */
void pit_install();

/**
 *  \brief Get the number of ticks that have passed when the PIT was initiated.
 *  
 *  \return Number of ticks passed
 *  
 *  \details More details
 */
uint32_t get_pit_ticks();

#endif /* INCLUDE_PIT_H */
