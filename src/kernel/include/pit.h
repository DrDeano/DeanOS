/**
 *  \file pit.h
 *  \brief The Programmable Interval Timer
 */
#ifndef INCLUDE_PIT_H
#define INCLUDE_PIT_H

#include <stdint.h>

/**
 * \brief The port address for the PIT data register for counter 0. This is going to be used as the
 * system clock.
 */
#define PIT_REG_COUNTER_0	0x40

/**
 * \brief The port address for the PIT data register for counter 1. This was used for refreshing
 * the DRAM chips. But now is unused and unknown use, so won't use.
 */
#define PIT_REG_COUNTER_1	0x41

/**
 * \brief The port address for the PIT data register for counter 2. Connected to the PC speakers,
 * we'll use this for the speakers.
 */
#define PIT_REG_COUNTER_2	0x42

/**
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

/**
 *  \brief The OCW mask for the counting mode.
 */
#define PIT_OCW_MASK_BINARY_COUNT			0x01	//00000001

/**
 *  \brief The OCW mask for the operating mode.
 */
#define PIT_OCW_MASK_MODE					0x0E	//00001110

/**
 *  \brief The OCW mask for the reading mode.
 */
#define PIT_OCW_MASK_READ_LOAD				0x30	//00110000

/**
 *  \brief The OCW mask for the selecting counter.
 */
#define PIT_OCW_MASK_SELECT_COUNTER			0xC0	//11000000

// The specific control bits for setting the PIT

/**
 *  \brief Have the counter count in binary (internally?).
 */
#define PIT_OCW_BINARY_COUNT_BINARY			0x00	//xxxxxxx0

/**
 *  \brief Have the counter count in BCD (internally?).
 */
#define PIT_OCW_BINARY_COUNT_BCD			0x01	//xxxxxxx1

/**
 *  \brief The PIT counter will be programmed with an initial COUNT value that counts down at a
 *  rate of the input clock frequency. When the COUNT reaches 0, and after the control word is 
 *  written, then its OUT pit is set high (1). Count down starts then the COUNT is set. The OUT pin
 *  remains high until the counter is reloaded with a new COUNT value or a new control work is
 *  written.
 */
#define PIT_OCW_MODE_TERMINAL_COUNT			0x00	//xxxx000x

/**
 *  \brief The counter is programmed to output a pulse every curtain number of clock pulses. The
 *  OUT pin remains high as soon as a control word is written. When COUNT is written, the counter
 *  waits until the rising edge of the GATE pin to start.  One clock pulse after the GATE pin, the
 *  OUT pin will remain low until COUNT reaches 0.
 */
#define PIT_OCW_MODE_ONE_SHOT				0x02	//xxxx001x

/**
 *  \brief The counter is initiated with a COUNT value. Counting starts next clock pulse. OUT pin
 *  remains high until COUNT reaches 1, then is set low for one clock pulse. Then COUNT is reset
 *  back to initial value and OUT pin is set high again.
 */
#define PIT_OCW_MODE_RATE_GENERATOR			0x04	//xxxx010x

/**
 *  \brief Similar to PIT_OCW_MODE_RATE_GENERATOR, but OUT pin will be high for half the time and
 *  low for half the time. Good for the speaker when setting a tone.
 */
#define PIT_OCW_MODE_SQUARE_WAVE_GENERATOR	0x06	//xxxx011x

/**
 *  \brief The counter is initialed with a COUNT value. Counting starts on next clock pulse. OUT
 *  pin remains high until count is 0. Then OUT pin is low for one clock pulse. Then resets to high
 *  again.
 */
#define PIT_OCW_MODE_SOFTWARE_TRIGGER		0x08	//xxxx100x

/**
 *  \brief The counter is initialed with a COUNT value. OUT pin remains high until the rising edge
 *  of the GATE pin. Then the counting begins. When COUNT reaches 0, OUT pin goes low for one clock
 *  pulse. Then COUNT is reset and OUT pin goes high. This cycles for each rising edge of the GATE
 *  pin.
 */
#define PIT_OCW_MODE_HARDWARE_TRIGGER		0x0A	//xxxx101x

/**
 *  \brief The counter value is latched into an internal control register at the time of the I/O
 *  write operations.
 */
#define PIT_OCW_READ_LOAD_LATCH				0x00	//xx00xxxx

/**
 *  \brief Read or load the most significant bit only.
 */
#define PIT_OCW_READ_LOAD_LSB_ONLY			0x10	//xx01xxxx

/**
 *  \brief Read or load the least significant bit only.
 */
#define PIT_OCW_READ_LOAD_MSB_ONLY			0x20	//xx10xxxx

/**
 *  \brief Read or load the least significant bit first then the most significant bit.
 */
#define PIT_OCW_READ_LOAD_DATA				0x30	//xx11xxxx

/**
 *  \brief The OCW bits for selecting counter 0. Used for the system clock.
 */
#define PIT_OCW_SELECT_COUNTER_0			0x00	//00xxxxxx

/**
 *  \brief The OCW bits for selecting counter 1. Was for the memory refreshing.
 */
#define PIT_OCW_SELECT_COUNTER_1			0x40	//01xxxxxx

/**
 *  \brief The OCW bits for selecting counter 2. Channel for the speaker.
 */
#define PIT_OCW_SELECT_COUNTER_2			0x80	//10xxxxxx
 
/**
 *  \brief Setup a counter with a tick rate and mode of operation
 *  
 *  \param [in] freq The frequency that the counter operates at.
 *  \param [in] counter Which counter is to be set up, either PIT_OCW_SELECT_COUNTER_0, 
 *  PIT_OCW_SELECT_COUNTER_1 or PIT_OCW_SELECT_COUNTER_2 only.
 *  \param [in] mode The mode of operation that the counter will operate in. See The modes
 *  definition above to chose which mode the counter is to run at.
 */
void pit_setup_counter(uint16_t freq, uint8_t counter, uint8_t mode);

/**
 *  \brief A simple wait that used the PIT to wait a number of ticks
 *  
 *  \param [in] tick The number of ticks to wait
 */
void pit_wait(int tick);

/**
 *  \brief Initialise the PIT with a handler to a IRQ.
 */
void pit_install(void);

/**
 *  \brief Get the number of ticks that have passed when the PIT was initiated.
 *  
 *  \return Number of ticks passed
 */
uint32_t get_pit_ticks(void);

/**
 *  \brief Get the divisor used by the PIT when initiated.
 *  
 *  \return The divisor
 */
uint16_t get_pit_divisor(void);

#endif /* INCLUDE_PIT_H */
