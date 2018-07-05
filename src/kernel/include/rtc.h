#ifndef INCLUDE_RTC_H
#define INCLUDE_RTC_H

#include <stdint.h>
#include <stdbool.h>

#define CURRENT_YEAR 2018

enum {
	CMOS_ADDRESS = 0x70,
	CMOS_DATA	 = 0x71
};

enum {
	CMOS_REG_SECOND		= 0x00,
	CMOS_REG_MINUTE		= 0x02,
	CMOS_REG_HOUR		= 0x04,
	CMOS_REG_WEEKDAY	= 0x06,
	CMOS_REG_DAY		= 0x07,
	CMOS_REG_MONTH		= 0x08,
	CMOS_REG_YEAR		= 0x09,
	CMOS_REG_CENTURY	= 0x32,
};

enum {
	CMOS_REG_STATUS_A	= 0x0A,
	CMOS_REG_STATUS_B	= 0x0B,
	CMOS_REG_STATUS_C	= 0x0C
};

typedef struct {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint16_t year;
	uint8_t century;
	uint8_t day_of_week; // 0 = Sunday, 1 = Monday
} rtc_date_time_t;

rtc_date_time_t * zero_date_time(rtc_date_time_t * d1);
rtc_date_time_t * copy_date_time(rtc_date_time_t * d1, rtc_date_time_t * d2);
void set_rate(uint8_t rate);
rtc_date_time_t * read_rtc(rtc_date_time_t * date, bool hour_24h, bool day_light_savings);
void rtc_init();

#endif /* INCLUDE_RTC_H */