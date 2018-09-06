#ifndef INCLUDED_CMOS_H
#define INCLUDED_CMOS_H

#include <stdint.h>

/**
 *  \todo Move to NMI file
 */
#define CMOS_ENABLE_NMI		0x80
#define CMOS_DISABLE_NMI	0x00

/**
 *  \brief The port address for the CMOS command register.
 */
#define CMOS_ADDRESS		0x70

/**
 *  \brief The port address for the CMOS data register.
 */
#define CMOS_DATA			0x71

/**
 *  \brief The register location for returning the seconds, (0 - 59).
 */
#define CMOS_REG_SECOND		0x00

/**
 *  \brief The register location for returning the minute, (0 - 59).
 */
#define CMOS_REG_MINUTE		0x02

/**
 *  \brief The register location for returning the hours, (0 - 23 or 0 - 12 depending if a 12hr or 24hr
 clock).
 */
#define CMOS_REG_HOUR		0x04

/**
 *  \brief The register location for returning the weekday, (0 - 6).
 */
#define CMOS_REG_WEEKDAY	0x06

/**
 *  \brief The register location for returning the day, (0 - 31).
 */
#define CMOS_REG_DAY		0x07

/**
 *  \brief The register location for returning the month, (0 - 11).
 */
#define CMOS_REG_MONTH		0x08

/**
 *  \brief The register location for returning the year, (0 - 99).
 */
#define CMOS_REG_YEAR		0x09

/**
 *  \brief The register location for returning the century.
 */
#define CMOS_REG_CENTURY	0x32

/**
 *  \brief The register location for return the status A register.
 */
#define CMOS_REG_STATUS_A	0x0A

/**
 *  \brief The register location for return the status B register.
 */
#define CMOS_REG_STATUS_B	0x0B

/**
 *  \brief The register location for return the status Cs register.
 */
#define CMOS_REG_STATUS_C	0x0C

/**
 *  \brief Read data from a register in the CMOS chip.
 *
 *  \parma [in] reg The register to read the data from.
 *
 *  \return The data in the register returned.
 */
uint8_t cmos_read(uint8_t reg);

/**
 *  \brief Write data to a register in the CMOS chip.
 *
 *  \parma [in] reg The register to write the data to.
 *  \parma [in] val The data to write to the register.
 */
void cmos_write(uint8_t reg, uint8_t val);

#endif /* INCLUDED_CMOS_H */
