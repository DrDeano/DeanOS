#include <vga.h>
#include <portio.h>

uint8_t vga_entry_colour(enum vga_colour fg, enum vga_colour bg) {
	return fg | bg << 4;
}

uint16_t vga_entry(unsigned char uc, uint8_t colour) {
	return (uint16_t) uc | (uint16_t) colour << 8;
}

void enable_cursor(void) {
	out_port_byte(VGA_PORT_CRTC_ADDRESS, VGA_REG_CURSOR_START);
	out_port_byte(VGA_PORT_CRTC_DATA, VGA_CURSOR_SCANLINE_START);
	
	out_port_byte(VGA_PORT_CRTC_ADDRESS, VGA_REG_CURSOR_END);
	out_port_byte(VGA_PORT_CRTC_DATA, VGA_CURSOR_SCANLINE_END);
}

void disable_cursor(void) {
	out_port_byte(VGA_PORT_CRTC_ADDRESS, VGA_REG_CURSOR_START);
	out_port_byte(VGA_PORT_CRTC_DATA, 0x20);
}
