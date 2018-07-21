#include <rtc.h>
#include <regs_t.h>
#include <irq.h>
#include <interrupt.h>
#include <portio.h>
#include <tty.h>
#include <pic.h>

#include <stdint.h>
#include <stdbool.h>

/**
 * \brief The register location for returning the century. As some CMOS chips don't support the
 * century register, and accessing it could lead to undefined results. The CMOS will set this if
 * there is a century register, else it will stay zero. So check this value, and if not zero, then
 * the CMOS supports the century register and can get the value from he register, else will just
 * have to add on the century to the year as the year is only the lower 2 numbers.
 */
static uint8_t century_reg = 0;

/**
 *  \brief Get whether the update flag is set or not. The update flag is set when the date and time
 *  in the CMOS chip is being updated. If the date and time is being updated, then will have to
 *  wait until the update flag is unset. If trying to get the current date and time when the update
 *  flag is set, this could lead to inconsistent values.
 *  
 *  \return If return a non zero value, then the update flag is set, and shouldn't get the current
 *  date and time. If return zero, then the flag isn't set and can get the current date and time.
 *  This function will either return 0x00 or 0x80.
 */
static uint8_t get_update_in_progress_flag(void) {
	out_port_byte(CMOS_ADDRESS, CMOS_REG_STATUS_A);
	return (in_port_byte(CMOS_DATA) & 0x80);
}

/**
 *  \brief Get data from a register from the CMOS chip.
 *  
 *  \parma [in] reg The register to get the data from.
 *  
 *  \return The data in the register returned.
 */
static uint8_t get_data_from_rtc_reg(const uint8_t reg) {
	out_port_byte(CMOS_ADDRESS, reg);
	return in_port_byte(CMOS_DATA);
}

/**
 *  \brief Calculate the day of the week from the supplied date.
 *  
 *  \parma [in] date The date to calculate the day of the week from.
 *  
 *  \return The pointer the date give as the parameter.
 */
static rtc_date_time_t * day_of_week(rtc_date_time_t * date) {
	static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	date->year -= date->month < 3;
	date->day_of_week = (date->year + date->year/4 - date->year/100 + date->year/400 + t[date->month-1] + date->day) % 7;
	return date;
}

rtc_date_time_t * read_rtc(rtc_date_time_t * date, const bool hour_24h, const bool day_light_savings) {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint16_t year;
	uint8_t century;
	
	uint8_t last_second;
	uint8_t last_minute;
	uint8_t last_hour;
	uint8_t last_day;
	uint8_t last_month;
	uint16_t last_year;
	uint8_t last_century;
	
	uint8_t reg_B;
	
	if(!date) {
		return NULL;
	}
	
	while(get_update_in_progress_flag());
	
	second = get_data_from_rtc_reg(CMOS_REG_SECOND);
	minute = get_data_from_rtc_reg(CMOS_REG_MINUTE);
	hour = get_data_from_rtc_reg(CMOS_REG_HOUR);
	day = get_data_from_rtc_reg(CMOS_REG_DAY);
	month = get_data_from_rtc_reg(CMOS_REG_MONTH);
	year = get_data_from_rtc_reg(CMOS_REG_YEAR);
	
	if(century_reg) {
		century = get_data_from_rtc_reg(century_reg);
	} else {
		century = -1;
	}
	
	// Use the method:
	// Read the registers twice and check if they are the same so to avoid inconsistent values due to RTC updates
	do {
		last_second = second;
		last_minute = minute;
		last_hour = hour;
		last_day = day;
		last_month = month;
		last_year = year;
		last_century = century;
		
		while(get_update_in_progress_flag());
		
		second = get_data_from_rtc_reg(CMOS_REG_SECOND);
		minute = get_data_from_rtc_reg(CMOS_REG_MINUTE);
		hour = get_data_from_rtc_reg(CMOS_REG_HOUR);
		day = get_data_from_rtc_reg(CMOS_REG_DAY);
		month = get_data_from_rtc_reg(CMOS_REG_MONTH);
		year = get_data_from_rtc_reg(CMOS_REG_YEAR);
		
		if(century_reg) {
			century = get_data_from_rtc_reg(century_reg);
		} else {
			century = -1;
		}
	} while ((last_second != second) || (last_minute != minute) || (last_hour != hour) ||
            (last_day != day) || (last_month != month) || (last_year != year) ||
            (last_century != century));
	
	reg_B = get_data_from_rtc_reg(CMOS_REG_STATUS_B);
	
	// Convert BCD to binary if necessary
	if(!(reg_B & 0x04)) {
		second = (second & 0x0F) + ((second / 16) * 10);
		minute = (minute & 0x0F) + ((minute / 16) * 10);
		hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
		day = (day & 0x0F) + ((day / 16) * 10);
		month = (month & 0x0F) + ((month / 16) * 10);
		year = (year & 0x0F) + ((year / 16) * 10);
		if(century_reg) {
			  century = (century & 0x0F) + ((century / 16) * 10);
		}
	}
	
	// Convert to 24 hour clock
	if(hour_24h && !(reg_B & 0x02) && (hour & 0x80)) {
		hour = ((hour & 0x7F) + 12) % 24;
	}  // Else 12 hour
	
	if(day_light_savings && hour_24h) {
		if(hour == 23) {
			hour = 0;
		} else {
			hour++;
		}
	} // Handle 12 hr format
	
	
	// Calculate the full (4-digit) year
	if(century_reg) {
		year += century * 100;
	} else {
		year += CURRENT_CENTURY;
	}
	
	date->second = second;
	date->minute = minute;
	date->hour = hour;
	date->day = day;
	date->month = month;
	date->year = year;
	date->century = century;
	
	return day_of_week(date);
}

rtc_date_time_t * zero_date_time(rtc_date_time_t * d1) {
	if (d1 == NULL) {
		return NULL;
	}
	d1->second = 0;
	d1->minute = 0;
	d1->hour = 0;
	d1->day = 0;
	d1->month = 0;
	d1->year = 0;
	d1->century = 0;
	d1->day_of_week = 0;
	
	return d1;
}

rtc_date_time_t * copy_date_time(rtc_date_time_t * d1, rtc_date_time_t * d2) {
	if (d1 == NULL || d2 == NULL) {
		return NULL;
	}
	d1->second = d2->second;
	d1->minute = d2->minute;
	d1->hour = d2->hour;
	d1->day = d2->day;
	d1->month = d2->month;
	d1->year = d2->year;
	d1->century = d2->century;
	d1->day_of_week = d2->day_of_week;
	
	return d1;
}

void set_rate(uint8_t rate) {
	rate &= 0x0F;			// rate must be above 2 and not over 15
	
	interrupt_disable();
	out_port_byte(CMOS_ADDRESS, 0x8A);		// set index to register A, disable NMI
	uint8_t prev = in_port_byte(CMOS_DATA);	// get initial value of register A
	out_port_byte(CMOS_ADDRESS, 0x8A);		// reset index to A
	out_port_byte(CMOS_DATA, (prev & 0xF0) | rate); //write only our rate to A. Note, rate is the bottom 4 bits.
	interrupt_enable();
}

static void rtc_handler(regs_t * regs) {
	(void) regs;
	set_display_time();					// May change to update internal time and have get time function and other function poll this.
	
	// Why need this?
	out_port_byte(CMOS_ADDRESS, 0x0C);	// select register C
	in_port_byte(CMOS_DATA);			// just throw away contents
}

void rtc_init(void) {
	// Install the handler for the real time clock
	irq_install_handler(PIC_IRQ_CMOS_REALT_TIME_CLOCK, rtc_handler);
	
	// Set the rate of interrupts to every half a second.
	set_rate(15);
	
	// 
	
	// Disable interrupts
	interrupt_disable();
	out_port_byte(CMOS_ADDRESS, 0x8B);			// select register B, and disable NMI
	uint8_t prev = in_port_byte(CMOS_DATA);	// read the current value of register B
	out_port_byte(CMOS_ADDRESS, 0x8B);			// set the index again (a read will reset the index to register D)
	out_port_byte(CMOS_DATA, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
	
	// Re-enable interrupts
	interrupt_enable();
}
