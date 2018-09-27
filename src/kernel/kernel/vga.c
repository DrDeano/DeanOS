#include <vga.h>
#include <portio.h>

static uint8_t cursor_scanline_start;	/**< The cursor scan line start so to know whether is in block or underline mode. */
static uint8_t cursor_scanline_end;		/**< The cursor scan line end so to know whether is in block or underline mode. */

uint8_t vga_entry_colour(enum vga_colour fg, enum vga_colour bg) {
	return fg | bg << 4;
}

uint16_t vga_entry(unsigned char uc, uint8_t colour) {
	return (uint16_t) uc | (uint16_t) colour << 8;
}

void vga_enable_cursor(void) {
	out_port_byte(VGA_PORT_CRT_ADDRESS, VGA_REG_CURSOR_START);
	out_port_byte(VGA_PORT_CRT_DATA, cursor_scanline_start);
	
	out_port_byte(VGA_PORT_CRT_ADDRESS, VGA_REG_CURSOR_END);
	out_port_byte(VGA_PORT_CRT_DATA, cursor_scanline_end);
}

void vga_disable_cursor(void) {
	out_port_byte(VGA_PORT_CRT_ADDRESS, VGA_REG_CURSOR_START);
	out_port_byte(VGA_PORT_CRT_DATA, VGA_CURSOR_DISABLE);
}

void vga_update_cursor(uint16_t x, uint16_t y) {
	const uint16_t pos = y * VGA_WIDTH + x;
	
	out_port_byte(VGA_PORT_CRT_ADDRESS, VGA_REG_CURSOR_LOCATION_LOW);
	out_port_byte(VGA_PORT_CRT_DATA, (uint8_t) (pos & 0xFF));
	
	out_port_byte(VGA_PORT_CRT_ADDRESS, VGA_REG_CURSOR_LOCATION_HIGH);
	out_port_byte(VGA_PORT_CRT_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

void vga_set_cursor_shape(vga_cursor_shape_t shape) {
	switch(shape) {
		case VGA_CURSOR_SHAPE_UNDERLINE:
			out_port_byte(VGA_PORT_CRT_ADDRESS, VGA_REG_CURSOR_START);
			out_port_byte(VGA_PORT_CRT_DATA, VGA_CURSOR_SCANLINE_MIDDLE);
			
			out_port_byte(VGA_PORT_CRT_ADDRESS, VGA_REG_CURSOR_END);
			out_port_byte(VGA_PORT_CRT_DATA, VGA_CURSOR_SCANLINE_END);
			
			cursor_scanline_start = VGA_CURSOR_SCANLINE_MIDDLE;
			cursor_scanline_end = VGA_CURSOR_SCANLINE_END;
			break;
		case VGA_CURSOR_SHAPE_BLOCK:
			out_port_byte(VGA_PORT_CRT_ADDRESS, VGA_REG_CURSOR_START);
			out_port_byte(VGA_PORT_CRT_DATA, VGA_CURSOR_SCANLINE_START);
			
			out_port_byte(VGA_PORT_CRT_ADDRESS, VGA_REG_CURSOR_END);
			out_port_byte(VGA_PORT_CRT_DATA, VGA_CURSOR_SCANLINE_END);
			
			cursor_scanline_start = VGA_CURSOR_SCANLINE_START;
			cursor_scanline_end = VGA_CURSOR_SCANLINE_END;
			break;
	}
}

void vga_init(void) {
	// Set the maximum scan line to 0x0F
	out_port_byte(VGA_PORT_CRT_ADDRESS, VGA_REG_MAXIMUM_SCAN_LINE);
	out_port_byte(VGA_PORT_CRT_DATA, VGA_CURSOR_SCANLINE_MAXIMUM);
	
	// Set by default the underline cursor
	cursor_scanline_start = VGA_CURSOR_SCANLINE_MIDDLE;
	cursor_scanline_end = VGA_CURSOR_SCANLINE_END;
}
