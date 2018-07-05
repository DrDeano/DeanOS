#ifndef _TIME_H
#define _TIME_H 1

#include <stdint.h>

#define CLOCKS_PER_SEC 1
#define TIME_UTC 1

struct tm {
   int tm_sec;   // seconds of minutes from 0 to 61
   int tm_min;   // minutes of hour from 0 to 59
   int tm_hour;  // hours of day from 0 to 24
   int tm_mday;  // day of month from 1 to 31
   int tm_mon;   // month of year from 0 to 11
   int tm_year;  // year since 1900
   int tm_wday;  // days since sunday
   int tm_yday;  // days since January 1st
   int tm_isdst; // hours of daylight savings time
};

typedef uint32_t clock_t;

typedef uint32_t time_t;


#endif /* _TIME_H */