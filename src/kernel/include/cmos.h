/**
 *  \file cmos.h
 *  \brief Functions, definitions and structures for writing and reading to and from the CMOS chip.
 */
#ifndef INCLUDED_CMOS_H
#define INCLUDED_CMOS_H

#include <stdint.h>

/**
 *  \todo Move to NMI file
 */

/**
 *  \brief The value to unset the non-maskable interrupt. This will be sent over port 0x70.
 */
#define CMOS_DISABLE_NMI	0x00

/**
 *  \brief The value to set the non-maskable interrupt. This will be sent over port 0x70.
 */
#define CMOS_ENABLE_NMI		0x80

/**
 *  \brief The port address for the CMOS command register.
 */
#define CMOS_ADDRESS		0x70

/**
 *  \brief The port address for the CMOS data register.
 */
#define CMOS_DATA			0x71

/**
 *  \brief The list of registers that access values used in the real time clock.
 */
enum cmos_rtc_registers {
	CMOS_REG_SECOND		= 0x00,	/**< The register location for returning the seconds, (0 - 59). */
	CMOS_REG_MINUTE		= 0x02,	/**< The register location for returning the minute, (0 - 59). */
	CMOS_REG_HOUR		= 0x04,	/**< The register location for returning the hours, (0 - 23 or 0 - 12 depending if 12 or 24 hour clock). */
	CMOS_REG_WEEKDAY	= 0x06,	/**< The register location for returning the weekday, (0 - 6). */
	CMOS_REG_DAY		= 0x07,	/**< The register location for returning the day, (0 - 31). */
	CMOS_REG_MONTH		= 0x08,	/**< The register location for returning the month, (0 - 11). */
	CMOS_REG_YEAR		= 0x09,	/**< The register location for returning the year, (0 - 99). */
	CMOS_REG_CENTURY	= 0x32	/**< The register location for returning the century. */
};

/**
 *  \brief The list of registers that access the status registers.
 */
enum cmos_status_registers {
	CMOS_REG_STATUS_A	= 0x0A,	/**< The register location for return the status A register. */
	CMOS_REG_STATUS_B	= 0x0B,	/**< The register location for return the status B register. */
	CMOS_REG_STATUS_C	= 0x0C	/**< The register location for return the status C register. */
};

/**
 *  \brief Read data from a register in the CMOS chip and return its value.
 *  
 *  \param [in] reg The register to read the data from.
 *  
 *  \return The data in the register returned.
 */
uint8_t cmos_read(uint8_t reg);

/**
 *  \brief Write data to a register in the CMOS chip.
 *  
 *  \param [in] reg The register to write the data to.
 *  \param [in] val The data to write to the register.
 */
void cmos_write(uint8_t reg, uint8_t val);

#endif /* INCLUDED_CMOS_H */
