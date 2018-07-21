/**
 *  \file rtc.h
 *  \brief The interface for the Real Time Clock using the CMOS.
 */
#ifndef INCLUDE_RTC_H
#define INCLUDE_RTC_H

#include <stdint.h>
#include <stdbool.h>

/**
 *  \brief The current year to be used for calculating the 4 digit year, as the CMOS return the
 *  last two digits of the year.
 */
#define CURRENT_CENTURY		2000

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
 *  \struct rtc_date_time_t
 * 
 *  \brief The structure for storing the date and time.
 */
typedef struct {
	uint8_t second;			/**< The number of seconds that have passed with a minute. (0 - 59) */
	uint8_t minute;			/**< The number of minute that have passed with a hour. (0 - 59) */
	uint8_t hour;			/**< The number of hours that have passed with a day. (0 - 12 or 0 - 24 depending on if it is a 12hr or 24hr clock). */
	uint8_t day;			/**< The number of days that have passed with a month. (0 - 31) */
	uint8_t month;			/**< The number of months that have passed with a year. (0 - 11) */
	uint16_t year;			/**< The number of years that have passed (e.g. 2018). */
	uint8_t century;		/**< The current century (e.g. 2000). */
	uint8_t day_of_week;	/**< The day of the week represented as a number starting from 0 = Sunday, 1 = Monday... */
} rtc_date_time_t;

/**
 *  \brief Given a \ref rtc_date_time_t structure, set all values to zero.
 *  
 *  \parma [in] d1 The date and time structure to be initialised to zero.
 *  
 *  \return The same pointer as the one given as the parameter.
 */
rtc_date_time_t * zero_date_time(rtc_date_time_t * d1);

/**
 *  \brief Copy the date and time values from d2 into d1.
 *  
 *  \parma [in] d1 The date and time structure to copy into.
 *  \parma [in] d2 The date and time structure to copy from.
 *  
 *  \return The pointer to d1. If d1 or d2 are NULL, then the return value will also be NULL.
 */
rtc_date_time_t * copy_date_time(rtc_date_time_t * d1, rtc_date_time_t * d2);

/**
 *  \brief Set the rate at which the CMOS will generate interrupts. Where rate must be between 0
 *  and 15. The rate is calculated by the formula: ?? 2^(rate - 1) ??.
 *  \todo Correct formula.
 *  \parma [in] rate 
 */
void set_rate(uint8_t rate);

/**
 *  \brief Read the current date and time from the CMOS chip and store then in the given structure.
 *  
 *  \parma [in] date              The date and time structure which the values will be placed in.
 *  \parma [in] hour_24           Whether the time is to be in 24hr format or 12hr format.
 *  \parma [in] day_light_savings Whether it is currently daylight saving. Adding an hour is it is.
 *  
 *  \return The pointer to the date structure.
 */
rtc_date_time_t * read_rtc(rtc_date_time_t * date, bool hour_24h, bool day_light_savings);

/**
 *  \brief Initialise the real time clock to update every half a second.
 */
void rtc_init(void);

#endif /* INCLUDE_RTC_H */
