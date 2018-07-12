#include <ps2.h>
#include <stdint.h>
#include <portio.h>

void ps2_wait_send(void) {
	uint32_t timeout = 1000000;
	while(timeout--) {
		if ((in_port_byte(PS2_COMMAND_PORT) & PS2_STATUS_INPUT_BUFFER_FULL) == 0) {
			return;
		}
	}
}

void ps2_send_command(uint8_t cmd) {
	ps2_wait_send();
	out_port_byte(PS2_COMMAND_PORT, cmd);
}