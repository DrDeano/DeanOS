/**
 *  \file rtc.h
 *  \brief Functions, definitions and structures for setting up the Real Time Clock using the CMOS.
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
 *  \param [in] d1 The date and time structure to be initialised to zero.
 *  
 *  \return The same pointer as the one given as the parameter.
 */
rtc_date_time_t * zero_date_time(rtc_date_time_t * d1);

/**
 *  \brief Copy the date and time values from d2 into d1.
 *  
 *  \param [in] d1 The date and time structure to copy into.
 *  \param [in] d2 The date and time structure to copy from.
 *  
 *  \return The pointer to d1. If d1 or d2 are NULL, then the return value will also be NULL.
 */
rtc_date_time_t * copy_date_time(rtc_date_time_t * d1, rtc_date_time_t * d2);

/**
 *  \brief Set the rate at which the CMOS will generate interrupts. Where rate must be between 0
 *  and 15. The rate is calculated by the formula: 2^(rate - 1).
 *  
 *  \param [in] rate The rate which the RTC timer operates at using the formula: 2^(rate - 1).
 */
void set_rate(uint8_t rate);

/**
 *  \brief Read the current date and time from the CMOS chip and store then in the given structure.
 *  
 *  \param [in] date              The date and time structure which the values will be placed in.
 *  \param [in] hour_24h          Whether the time is to be in 24hr format or 12hr format.
 *  \param [in] day_light_savings Whether it is currently daylight saving. Adding an hour is it is.
 *  
 *  \return The pointer to the date structure.
 */
rtc_date_time_t * read_rtc(rtc_date_time_t * date, bool hour_24h, bool day_light_savings);

/**
 *  \brief Initialise the real time clock to update every half a second.
 */
void rtc_init(void);

#endif /* INCLUDE_RTC_H */
