#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <tty.h>
#include <vga.h>
#include <portio.h>
#include <rtc.h>

static size_t terminal_column;								/**< The current x position of the cursor */
static size_t terminal_row;									/**< The current y position of the cursor */
static uint8_t terminal_colour;								/**< The current colour of the display with forground and background colour */
static uint16_t * terminal_buffer;							/**< The buffer starting from the beginning of the video memory
																 locaiton that contains all data writen to the display */
rtc_date_time_t on_screen_time;

static void terminal_put_entry_at(unsigned char c, uint8_t colour, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, colour);
}

static void update_cursor() {
	const uint16_t pos = terminal_row * VGA_WIDTH + terminal_column;
	
	out_port_byte(VGA_PORT_CRTC_ADDRESS, VGA_REG_CURSOR_LOCATION_LOW);
	out_port_byte(VGA_PORT_CRTC_DATA, (uint8_t) (pos & 0xFF));
	
	out_port_byte(VGA_PORT_CRTC_ADDRESS, VGA_REG_CURSOR_LOCATION_HIGH);
	out_port_byte(VGA_PORT_CRTC_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

static void terminal_scroll() {
	// If at the end of the screen, scroll
	size_t temp;
	if(terminal_row >= 25) {
		temp = terminal_row - 25 + 1;
		
		// Move all rows up one
		memmove(terminal_buffer + (TERMINAL_ROW_MIN * 80), terminal_buffer + (temp * 80) + (TERMINAL_ROW_MIN * 80), (25 - temp) * 80 * 2);
		
		// Set the last row to blanks
		for(int i = 0; i < 80; i++) {
			terminal_put_entry_at('\0', terminal_colour, i, 24);
		}
		terminal_row = 25 - 1;
		
	}
}

static void _terminal_put_char(char c) {
	unsigned char uc = c;
	
	switch (uc) {
		case '\n':
			terminal_column = 0;
			++terminal_row;
			terminal_scroll();
			break;
		
		case '\t':
			terminal_column += 4;
			if(terminal_colour >= VGA_WIDTH) {
				terminal_column -= VGA_HEIGHT;
				terminal_row++;
				terminal_scroll();
			}
			break;
			
		case '\r':
			terminal_column = 0;
			break;
			
		case '\b':
			terminal_column--;
			//_terminal_put_char('\0');
			//terminal_column--;
			break;
			
		default:
			terminal_put_entry_at(uc, terminal_colour, terminal_column, terminal_row);
			if (++terminal_column == VGA_WIDTH) {
				terminal_column = 0;
				terminal_row++;
				terminal_scroll();
			}
			break;
	}
}

static void print_logo() {
	size_t column_temp = terminal_column;
	size_t row_temp = terminal_row;
	
	terminal_column = 0;
	terminal_row = 0;
	
	terminal_write_string("   _____                            ____     _____            ___         ___  \n");
	terminal_write_string("  |  __ \\                          / __ \\   / ____|          / _ \\       |__ \\ \n");
	terminal_write_string("  | |  | |   ___    __ _   _ __   | |  | | | (___   __   __ | | | |         ) |\n");
	terminal_write_string("  | |  | |  / _ \\  / _` | | '_ \\  | |  | |  \\___ \\  \\ \\ / / | | | |        / / \n");
	terminal_write_string("  | |__| | |  __/ | (_| | | | | | | |__| |  ____) |  \\ V /  | |_| |  _    / /_ \n");
	terminal_write_string("  |_____/   \\___|  \\__,_| |_| |_|  \\____/  |_____/    \\_/    \\___/  (_)  |____|\n");
	
	terminal_column = column_temp;
	terminal_row = row_temp;
	update_cursor();
}

void clear() {
	for (size_t y = TERMINAL_ROW_MIN; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_colour);
		}
	}
	terminal_column = 0;
	terminal_row = TERMINAL_ROW_MIN;
	update_cursor();
}

void move_cursor_left() {
	if(terminal_column == 0) {
		if(terminal_row != 0) {
			terminal_column = VGA_HEIGHT - 1;
			terminal_row--;
		}
	} else {
		terminal_column--;
	}
	update_cursor();
}

void move_cursor_right() {
	if(terminal_column == (VGA_HEIGHT - 1)) {
		if(terminal_row != (VGA_WIDTH - 1)) {
			terminal_column = 0;
			terminal_row++;
		}
	} else {
		terminal_column++;
	}
	update_cursor();
}

void set_display_time() {
	// Time is on line 7
	static char * str_day[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	size_t column_temp = terminal_column;
	size_t row_temp = terminal_row;
	
	terminal_column = 0;
	terminal_row = TERMINAL_ROW_MIN - 2;
	
	read_rtc(&on_screen_time, true, true);
	
	kprintf("                         %s %02d-%02d-%04d %02d:%02d:%02d   \n", str_day[on_screen_time.day_of_week], on_screen_time.day, on_screen_time.month, on_screen_time.year, on_screen_time.hour, on_screen_time.minute, on_screen_time.second);
	
	terminal_column = column_temp;
	terminal_row = row_temp;
	update_cursor();
}

void set_time(rtc_date_time_t * date) {
	copy_date_time(&on_screen_time, date);
}

rtc_date_time_t * get_time(rtc_date_time_t * date) {
	return copy_date_time(date, &on_screen_time);
}

void terminal_set_color(uint8_t colour) {
	terminal_colour = colour;
}

void terminal_put_char(char c) {
	_terminal_put_char(c);
	update_cursor();
}

void terminal_write(const char * data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		_terminal_put_char(data[i]);
	}
	update_cursor();
}

void terminal_write_string(const char * str) {
	terminal_write(str, strlen(str));
}

void terminal_initialise(boot_params * params) {
	size_t row_offset = 0;
	terminal_colour = vga_entry_colour(VGA_COLOUR_LIGHT_GREY, VGA_COLOUR_BLACK);
	terminal_buffer = VGA_MEMORY;
	
	zero_date_time(&on_screen_time);
	
	if(params) {
		terminal_row = params->cursor_y;
		terminal_column = params->cursor_x;
		
		// Copy rows 7 down to make room for logo
		// If there isn't enough room, only take the bottom rows
		if(24 - terminal_row < TERMINAL_ROW_MIN) {
			row_offset = TERMINAL_ROW_MIN - (24 - terminal_row);
		}
		
		// Move terminal_row rows down 7
		memmove(terminal_buffer + (TERMINAL_ROW_MIN * 80), terminal_buffer + (row_offset * 80), terminal_row * 80 * 2);
		
		// Set the top 7 rows blank
		for(int j = 0; j < TERMINAL_ROW_MIN; j++) {
			for(int i = 0; i < 80; i++) {
				terminal_put_entry_at(' ', terminal_colour, i, j);
			}
		}
		
		terminal_row += row_offset + TERMINAL_ROW_MIN;
		
	} else {
		terminal_row = 0;
		terminal_column = 0;
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t index = y * VGA_WIDTH + x;
				terminal_buffer[index] = vga_entry(' ', terminal_colour);
			}
		}
	}
	
	print_logo();
	
	enable_cursor();
	update_cursor();
}
