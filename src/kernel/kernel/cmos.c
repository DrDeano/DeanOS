#include <cmos.h>
#include <portio.h>

#include <stdint.h>

/**
 * \todo Set the NMI bit correctly
 */

/**
 * \todo Add delay between port operations maybe
 */
uint8_t cmos_read(uint8_t reg) {
	out_port_byte(CMOS_ADDRESS, reg);
	return in_port_byte(CMOS_DATA);
}

void cmos_write(uint8_t reg, uint8_t val) {
	out_port_byte(CMOS_ADDRESS, reg);
	out_port_byte(CMOS_DATA, val);
}
