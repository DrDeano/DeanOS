/**
 * \file time.h
 * \brief The C Library: time.
 */
#ifndef _TIME_H
#define _TIME_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief This macro represents the number of processor clocks per second.
 * \todo Will need to be set correctly in the future.
 */
#define CLOCKS_PER_SEC 1

/**
 * \brief The structure for holding all the value in the data and time.
 */
struct tm {
	int tm_sec;		/**< seconds of minutes from 0 to 61 */
	int tm_min;		/**< minutes of hour from 0 to 59 */
	int tm_hour;	/**< hours of day from 0 to 24 */
	int tm_mday;	/**< day of month from 1 to 31 */
	int tm_mon;		/**< month of year from 0 to 11 */
	int tm_year;	/**< year since 1900 */
	int tm_wday;	/**< days since Sunday */
	int tm_yday;	/**< days since January 1st */
	int tm_isdst;	/**< hours of daylight savings time */
};

/**
 * \brief The clock_t type
 */
typedef uint32_t clock_t;

/**
 * \brief The time_t type
 */
typedef uint32_t time_t;

#ifdef __cplusplus
}
#endif

#endif /* _TIME_H */
