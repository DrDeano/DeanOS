/**
 *  \file ps2.h
 *  \brief The PS/2 
 */
#ifndef INCLUDE_PS2_H
#define INCLUDE_PS2_H

#include <stdint.h>

// PS/2 commands
#define PS2_DATA_PORT		0x60
#define PS2_COMMAND_PORT	0x64

#define PS2_STATUS_INPUT_BUFFER_FULL	0x02

void ps2_wait_send(void);

void ps2_send_command(uint8_t cmd);

#endif /* INCLUDE_PS2_H */
