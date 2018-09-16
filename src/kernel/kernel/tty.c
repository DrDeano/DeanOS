#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <tty.h>
#include <vga.h>
#include <portio.h>
#include <rtc.h>

static size_t terminal_column;			/**< The current x position of the cursor. */
static size_t terminal_row;				/**< The current y position of the cursor. */
static uint8_t terminal_colour;			/**< The current colour of the display with foreground and background colour. */
static uint16_t * terminal_buffer;		/**< The buffer starting from the beginning of the video memory location that contains all data written to the display. */
static rtc_date_time_t on_screen_time;	/**< The display time that is displayed under the logo. */

static const uint16_t page_total_char = VGA_WIDTH * TERMINAL_ROW_TOTAL;
static uint16_t terminal_pages[5][VGA_WIDTH * TERMINAL_ROW_TOTAL]; /**< 5 pages. */
static uint8_t terminal_page_index;

/**
 *  \brief Update the cursor position to the end of the current line or character printed.
 */
static void update_cursor(void) {
	const uint16_t pos = terminal_row * VGA_WIDTH + terminal_column;

	out_port_byte(VGA_PORT_CRTC_ADDRESS, VGA_REG_CURSOR_LOCATION_LOW);
	out_port_byte(VGA_PORT_CRTC_DATA, (uint8_t) (pos & 0xFF));

	out_port_byte(VGA_PORT_CRTC_ADDRESS, VGA_REG_CURSOR_LOCATION_HIGH);
	out_port_byte(VGA_PORT_CRTC_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

static void display_page_number(void) {
	size_t column_temp = terminal_column;
	size_t row_temp = terminal_row;

	// A bit hard coded as the kprintf below is 11 characters long.
	// Once got a ksnprintf, then can use that value to set how far from the end to print.
	terminal_column = VGA_WIDTH - 11;
	terminal_row = TERMINAL_ROW_MIN - 1;

	kprintf("Page %d of %d", terminal_page_index, 4);

	terminal_column = column_temp;
	terminal_row = row_temp;
	//update_cursor();
}

/**
 *  \brief Take a character and a x/y position and put it into the video memory so that it is
 *  displayed on the screen.
 *
 *  \param [in] c The character to be displayed.
 *  \param [in] x The x position on the screen.
 *  \param [in] y The y position on the screen.
 */
static void terminal_put_entry_at(unsigned char c, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	uint16_t char_entry = vga_entry(c, terminal_colour);

	if(index >= (TERMINAL_ROW_MIN * VGA_WIDTH)) {
		// If not at page zero, bottom, then display that page
		if(terminal_page_index != 0) {
			terminal_page_index = 0;
			memmove(terminal_buffer + (TERMINAL_ROW_MIN * VGA_WIDTH), terminal_pages[terminal_page_index], page_total_char * 2);
			display_page_number();
			enable_cursor();
			update_cursor();
		}
		terminal_pages[terminal_page_index][index - (TERMINAL_ROW_MIN * VGA_WIDTH)] = char_entry;
	}

	terminal_buffer[index] = char_entry;
}

static void pages_move_rows_up(uint16_t rows) {
    // Move up rows up by "rows"
    memmove(terminal_pages[4], terminal_pages[4] + (rows * VGA_WIDTH), (TERMINAL_ROW_TOTAL - rows) * VGA_WIDTH * 2);

    // Loop for the other 4 pages
    for(int i = 3; i >= 0; i--) {
		// Move the top rows of the current page to the page above
		memmove(terminal_pages[i + 1] + ((TERMINAL_ROW_TOTAL - rows) * VGA_WIDTH), terminal_pages[i], rows * VGA_WIDTH * 2);

		// Move the rows of the current page up
		memmove(terminal_pages[i], terminal_pages[i] + (rows * VGA_WIDTH), (TERMINAL_ROW_TOTAL - rows) * VGA_WIDTH * 2);
    }

    // Clear the last line
    memset(terminal_pages[0] + ((TERMINAL_ROW_TOTAL - 1) * VGA_WIDTH), 0, VGA_WIDTH * 2);
}

/**
 *  \brief When the text/terminal gets to the bottom of the screen, then move all line up by the
 *  amount that are below the bottom of the screen. Usually moves up by one line.
 */
static void terminal_scroll(void) {
	// If at the end of the screen, scroll
	size_t temp;
	if(terminal_row >= VGA_HEIGHT) {
		temp = terminal_row - VGA_HEIGHT + 1;

		// Move rows up pages by temp, will usually be one.
		pages_move_rows_up(temp);

		// Move all rows up by temp
		memmove(terminal_buffer + (TERMINAL_ROW_MIN * VGA_WIDTH), terminal_buffer + (temp * VGA_WIDTH) + (TERMINAL_ROW_MIN * VGA_WIDTH), (VGA_HEIGHT - temp) * VGA_WIDTH * 2);


		memset(terminal_buffer + ((VGA_HEIGHT - 1) * VGA_WIDTH), 0, VGA_WIDTH * 2);
		// Set the last row to blanks
		//for(int i = 0; i < VGA_WIDTH; i++) {
		//	terminal_put_entry_at('\0', i, 24);
		//}
		terminal_row = VGA_HEIGHT - 1;
	}
}

/**
 *  \brief The internal function to print a character without updating the cursor. For speed when
 *  printing a string as only need to update the cursor once.
 *
 *  \param [in] c The character to print
 */
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
			if(terminal_column == 0) {
				if(terminal_row != 0) {
					terminal_column = VGA_WIDTH - 1;
					terminal_row--;
				}
			} else {
				terminal_column--;
			}
			break;

		default:
			terminal_put_entry_at(uc, terminal_column, terminal_row);
			if (++terminal_column == VGA_WIDTH) {
				terminal_column = 0;
				terminal_row++;
				terminal_scroll();
			}
			break;
	}
}

/**
 *  \brief Print the DeanOS logo to the top of the screen with version number.
 */
static void print_logo(void) {
	size_t column_temp = terminal_column;
	size_t row_temp = terminal_row;

	terminal_column = 0;
	terminal_row = 0;

	terminal_write_string("  _____                            ____     _____            ___        ____ \n");
	terminal_write_string(" |  __ \\                          / __ \\   / ____|          / _ \\      |___ \\\n");
	terminal_write_string(" | |  | |   ___    __ _   _ __   | |  | | | (___   __   __ | | | |       __) |\n");
	terminal_write_string(" | |  | |  / _ \\  / _` | | '_ \\  | |  | |  \\___ \\  \\ \\ / / | | | |      |__ <\n");
	terminal_write_string(" | |__| | |  __/ | (_| | | | | | | |__| |  ____) |  \\ V /  | |_| |  _   ___) |\n");
	terminal_write_string(" |_____/   \\___|  \\__,_| |_| |_|  \\____/  |_____/    \\_/    \\___/  (_) |____/\n");

	terminal_column = column_temp;
	terminal_row = row_temp;
	update_cursor();
}

void page_up(void) {
	if(terminal_page_index == 4) {
		return;
	}
	// Copy page to display
	memmove(terminal_buffer + (TERMINAL_ROW_MIN * VGA_WIDTH), terminal_pages[++terminal_page_index], page_total_char * 2);
	display_page_number();
	disable_cursor();
}

void page_down(void) {
	if(terminal_page_index == 0) {
		return;
	}
	// Copy page to display
	memmove(terminal_buffer + (TERMINAL_ROW_MIN * VGA_WIDTH), terminal_pages[--terminal_page_index], page_total_char * 2);
	display_page_number();
	if(terminal_page_index == 0) {
		enable_cursor();
		update_cursor();
	} else {
		disable_cursor();
	}
}

void clear(void) {
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

void move_cursor_left(void) {
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

void move_cursor_right(void) {
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

void set_display_time(void) {
	// Time is on line 7
	static char * str_day[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	size_t column_temp = terminal_column;
	size_t row_temp = terminal_row;

	terminal_column = 0;
	terminal_row = TERMINAL_ROW_MIN - 1;

	read_rtc(&on_screen_time, true, true);

	kprintf("                         %s %02d-%02d-%04d %02d:%02d:%02d", str_day[on_screen_time.day_of_week], on_screen_time.day, on_screen_time.month, on_screen_time.year, on_screen_time.hour, on_screen_time.minute, on_screen_time.second);

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
	// Row offset that is given from the boot loader. Where to start printing from the kernel.
	size_t row_offset = 0;

	// Set the colour to Black on white.
	terminal_colour = vga_entry_colour(VGA_COLOUR_LIGHT_GREY, VGA_COLOUR_BLACK);

	// Set the display memory map location.
	terminal_buffer = VGA_MEMORY;

	// Initialise indexes to zero.
	terminal_page_index = 0;

	// Initialise pages to zeros
	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < page_total_char; j++) {
			terminal_pages[i][j] = 0;
		}
	}

	// Zero out the time (for now).
	zero_date_time(&on_screen_time);

	// Get the kernel parameters from the boot loader.
	if(params && (params->sig != 0)) {
		terminal_row = params->cursor_y;
		terminal_column = params->cursor_x;

		// Copy rows 7 down to make room for logo
		// If there isn't enough room, only take the bottom rows
		if(24 - terminal_row < TERMINAL_ROW_MIN) {
			row_offset = TERMINAL_ROW_MIN - (24 - terminal_row);
		}

		// Make a copy into the terminal_pages
		// Assuming that there is only one page
		memmove(terminal_pages[0], terminal_buffer, terminal_row * VGA_WIDTH * 2);

		// Move terminal_row rows down 7
		memmove(terminal_buffer + (TERMINAL_ROW_MIN * VGA_WIDTH), terminal_buffer + (row_offset * VGA_WIDTH), terminal_row * VGA_WIDTH * 2);

		// Set the top 7 rows blank
		memset(terminal_buffer, 0, VGA_WIDTH * TERMINAL_ROW_MIN * 2);
//		for(int j = 0; j < TERMINAL_ROW_MIN; j++) {
//			for(int i = 0; i < VGA_WIDTH; i++) {
//				terminal_put_entry_at('\0', i, j);
//			}
//		}

		terminal_row += row_offset + TERMINAL_ROW_MIN;

	} else { // If no parameters
		// Clear the screen
		terminal_row = 0;
		terminal_column = 0;
		memset(terminal_buffer, 0, VGA_WIDTH * VGA_HEIGHT * 2);
//		for (size_t y = 0; y < VGA_HEIGHT; y++) {
//			for (size_t x = 0; x < VGA_WIDTH; x++) {
//				const size_t index = y * VGA_WIDTH + x;
//				terminal_buffer[index] = vga_entry(' ', terminal_colour);
//			}
//		}
		terminal_row = TERMINAL_ROW_MIN;
	}

	// Print the logo
	print_logo();

	display_page_number();

	// Enable and show the cursor at the end of the line
	enable_cursor();
	update_cursor();
}
