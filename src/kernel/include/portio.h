/**
 *  \file portio.h
 *  \brief A set of inlined assembly that enables to read and write to IO ports
 */
#ifndef INCLUDE_PORTIO_H
#define INCLUDE_PORTIO_H

#include <stdint.h>

/**
 *  \brief To write a byte to a port
 *  
 *  \param [in] port The port to write to
 *  \param [in] data The byte of data that will be sent
 *  
 *  \details Inline assembly to write to a given port with a byte of data
 */
static inline void out_port_byte(uint16_t port, uint8_t data) {
	__asm__ __volatile__ ("outb %1, %0" : : "a" (data), "dN" (port));
}

/**
 *  \brief To read a byte from a port
 *  
 *  \param [in] port The port to read data from
 *  \return The data that the port returns
 *  
 *  \details Inline assembly that reads data from a given port and returns its value
 */
static inline uint8_t in_port_byte(uint16_t port) {
	uint8_t ret;
    __asm__ __volatile__ ("inb %0, %1" : "=a" (ret) : "dN" (port));
    return ret;
}

#endif /* INCLUDE_PORTIO_H */
