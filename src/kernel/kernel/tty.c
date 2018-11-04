#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <tty.h>
#include <vga.h>
#include <portio.h>
#include <rtc.h>

static size_t tty_column;			/**< The current x position of the cursor. */
static size_t tty_row;				/**< The current y position of the cursor. */
static vga_colour_t tty_colour;		/**< The current colour of the display with foreground and background colour. */
static uint16_t * tty_buffer;		/**< The buffer starting from the beginning of the video memory location that contains all data written to the display. */
static rtc_date_time_t tty_time;	/**< The display time that is displayed under the logo. */

static uint16_t blank;				/**< The blank VGA entry to be used to clear the screen. */

static const uint16_t page_total_char = VGA_WIDTH * TTY_ROW_TOTAL;	/**< The total number of characters of all pages combined. */
static uint16_t tty_pages[5][VGA_WIDTH * TTY_ROW_TOTAL];			/**< A total of 5 pages that can be saved. */
static uint8_t tty_page_index;										/**< The current page index. */

/**
 * \brief Internal function for updating the cursor using \ref tty_column and \ref tty_row.
 */
static void tty_update_cursor(void) {
	vga_update_cursor(tty_column, tty_row);
}

/**
 * \brief Internal function for displaying the current page being displayed.
 */
static void display_page_number(void) {
	size_t column_temp = tty_column;
	size_t row_temp = tty_row;
	
	// A bit hard coded as the kprintf below is 11 characters long.
	// Once got a ksnprintf, then can use that value to set how far from the end to print.
	tty_column = VGA_WIDTH - 11;
	tty_row = TTY_ROW_MIN - 1;
	
	kprintf("Page %d of %d", tty_page_index, 4);
	
	tty_column = column_temp;
	tty_row = row_temp;
}

/**
 * \brief Take a character and a x, y position and put it into the video memory so that it is
 * displayed on the screen.
 * 
 * \param [in] c The character to be displayed.
 * \param [in] x The x position on the screen.
 * \param [in] y The y position on the screen.
 */
static void tty_put_entry_at(unsigned char c, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	uint16_t char_entry = vga_entry(c, tty_colour);
	
	if(index >= (TTY_ROW_MIN * VGA_WIDTH)) {
		// If not at page zero, bottom, then display that page
		if(tty_page_index != 0) {
			tty_page_index = 0;
			memmove(tty_buffer + (TTY_ROW_MIN * VGA_WIDTH), tty_pages[tty_page_index], page_total_char * 2);
			display_page_number();
			vga_enable_cursor();
			tty_update_cursor();
		}
		tty_pages[tty_page_index][index - (TTY_ROW_MIN * VGA_WIDTH)] = char_entry;
	}
	
	tty_buffer[index] = char_entry;
}

/**
 *  \brief Internal function for moving lines up across multiple pages.
 */
static void tty_pages_move_rows_up(uint16_t rows) {
	// Move up rows up by "rows"
	memmove(tty_pages[4], tty_pages[4] + (rows * VGA_WIDTH), (TTY_ROW_TOTAL - rows) * VGA_WIDTH * 2);
	
	// Loop for the other 4 pages
	for(int i = 3; i >= 0; i--) {
		// Move the top rows of the current page to the page above
		memmove(tty_pages[i + 1] + ((TTY_ROW_TOTAL - rows) * VGA_WIDTH), tty_pages[i], rows * VGA_WIDTH * 2);
		
		// Move the rows of the current page up
		memmove(tty_pages[i], tty_pages[i] + (rows * VGA_WIDTH), (TTY_ROW_TOTAL - rows) * VGA_WIDTH * 2);
	}
	
	// Clear the last line
	memset(tty_pages[0] + ((TTY_ROW_TOTAL - 1) * VGA_WIDTH), blank, VGA_WIDTH * 2);
}

/**
 * \brief When the text/terminal gets to the bottom of the screen, then move all line up by the
 * amount that are below the bottom of the screen. Usually moves up by one line.
 */
static void tty_scroll(void) {
	// If at the end of the screen, scroll
	size_t temp;
	if(tty_row >= VGA_HEIGHT) {
		temp = tty_row - VGA_HEIGHT + 1;
		
		// Move rows up pages by temp, will usually be one.
		tty_pages_move_rows_up(temp);
		
		// Move all rows up by temp
		memmove(tty_buffer + (TTY_ROW_MIN * VGA_WIDTH), tty_buffer + (temp * VGA_WIDTH) + (TTY_ROW_MIN * VGA_WIDTH), (VGA_HEIGHT - temp) * VGA_WIDTH * 2);
		
		// Set the last row to blanks
		memset(tty_buffer + ((VGA_HEIGHT - 1) * VGA_WIDTH), blank, VGA_WIDTH * 2);
		
		tty_row = VGA_HEIGHT - 1;
	}
}

/**
 * \brief The internal function to print a character without updating the cursor. For speed when
 * printing a string as only need to update the cursor once.
 * 
 * \param [in] c The character to print
 */
static void _tty_put_char(char c) {
	unsigned char uc = c;
	
	switch (uc) {
		case '\n':
			tty_column = 0;
			++tty_row;
			tty_scroll();
			break;
			
		case '\t':
			tty_column += 4;
			if(tty_colour >= VGA_WIDTH) {
				tty_column -= VGA_HEIGHT;
				tty_row++;
				tty_scroll();
			}
			break;
			
		case '\r':
			tty_column = 0;
			break;
			
		case '\b':
			if(tty_column == 0) {
				if(tty_row != 0) {
					tty_column = VGA_WIDTH - 1;
					tty_row--;
				}
			} else {
				tty_column--;
			}
			break;
			
		default:
			tty_put_entry_at(uc, tty_column, tty_row);
			if (++tty_column == VGA_WIDTH) {
				tty_column = 0;
				tty_row++;
				tty_scroll();
			}
			break;
	}
}

/**
 * \brief Print the DeanOS logo to the top of the screen with version number. Don't need to update
 * the cursor as is called before the cursor is enabled.
 */
static void tty_print_logo(void) {
	size_t column_temp = tty_column;
	size_t row_temp = tty_row;
	
	tty_column = 0;
	tty_row = 0;
	
	tty_write_string("  _____                            ____     _____            ___        ____ \n");
	tty_write_string(" |  __ \\                          / __ \\   / ____|          / _ \\      |___ \\\n");
	tty_write_string(" | |  | |   ___    __ _   _ __   | |  | | | (___   __   __ | | | |       __) |\n");
	tty_write_string(" | |  | |  / _ \\  / _` | | '_ \\  | |  | |  \\___ \\  \\ \\ / / | | | |      |__ <\n");
	tty_write_string(" | |__| | |  __/ | (_| | | | | | | |__| |  ____) |  \\ V /  | |_| |  _   ___) |\n");
	tty_write_string(" |_____/   \\___|  \\__,_| |_| |_|  \\____/  |_____/    \\_/    \\___/  (_) |____/\n");
	
	tty_column = column_temp;
	tty_row = row_temp;
}

void tty_page_up(void) {
	if(tty_page_index == 4) {
		return;
	}
	// Copy page to display
	memmove(tty_buffer + (TTY_ROW_MIN * VGA_WIDTH), tty_pages[++tty_page_index], page_total_char * 2);
	display_page_number();
	vga_disable_cursor();
}

void tty_page_down(void) {
	if(tty_page_index == 0) {
		return;
	}
	// Copy page to display
	memmove(tty_buffer + (TTY_ROW_MIN * VGA_WIDTH), tty_pages[--tty_page_index], page_total_char * 2);
	display_page_number();
	if(tty_page_index == 0) {
		vga_enable_cursor();
		tty_update_cursor();
	} else {
		vga_disable_cursor();
	}
}

/**
 * \todo Change clear so to move all rows up into the above pages.
 */
void tty_clear(void) {
	memset(tty_buffer + (TTY_ROW_MIN * VGA_WIDTH), (int) blank, VGA_WIDTH * VGA_HEIGHT * 2);
	tty_column = 0;
	tty_row = TTY_ROW_MIN;
	tty_update_cursor();
}

void tty_move_cursor_left(void) {
	if(tty_column == 0) {
		if(tty_row != 0) {
			tty_column = VGA_HEIGHT - 1;
			tty_row--;
		}
	} else {
		tty_column--;
	}
	tty_update_cursor();
}

void tty_move_cursor_right(void) {
	if(tty_column == (VGA_HEIGHT - 1)) {
		if(tty_row != (VGA_WIDTH - 1)) {
			tty_column = 0;
			tty_row++;
		}
	} else {
		tty_column++;
	}
	tty_update_cursor();
}

void tty_set_display_time(void) {
	// Time is on row 7
	static char * str_day[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	size_t column_temp = tty_column;
	size_t row_temp = tty_row;
	
	tty_column = 0;
	tty_row = TTY_ROW_MIN - 1;
	
	read_rtc(&tty_time, true);
	
	kprintf("                         %s %02d-%02d-%04d %02d:%02d:%02d", str_day[tty_time.day_of_week], tty_time.day, tty_time.month, tty_time.year, tty_time.hour, tty_time.minute, tty_time.second);
	
	tty_column = column_temp;
	tty_row = row_temp;
	tty_update_cursor();
}

void tty_set_time(rtc_date_time_t * date) {
	copy_date_time(&tty_time, date);
}

rtc_date_time_t * tty_get_time(rtc_date_time_t * date) {
	return copy_date_time(date, &tty_time);
}

void tty_set_color(vga_colour_t colour) {
	tty_colour = colour;
	blank = vga_entry('\0', tty_colour);
}

void tty_put_char(char c) {
	_tty_put_char(c);
	tty_update_cursor();
}

void tty_write(const char * data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		_tty_put_char(data[i]);
	}
	tty_update_cursor();
}

void tty_write_string(const char * str) {
	tty_write(str, strlen(str));
}

void tty_init(boot_params * params) {
	// Row offset that is given from the boot loader. Where to start printing from the kernel.
	size_t row_offset = 0;
	
	// Set the colour to Black on white.
	tty_set_color(vga_entry_colour(VGA_COLOUR_LIGHT_GREY, VGA_COLOUR_BLACK));
	
	// Set the display memory map location.
	tty_buffer = VGA_MEMORY;
	
	// Initialise indexes to zero.
	tty_page_index = 0;
	
	// Initialise pages to zeros
	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < page_total_char; j++) {
			tty_pages[i][j] = 0;
		}
	}
	
	// Zero out the time (for now).
	zero_date_time(&tty_time);
	
	// Get the kernel parameters from the boot loader.
	if(params && (params->sig != 0)) {
		tty_row = params->cursor_y;
		tty_column = params->cursor_x;
		
		// Copy rows 7 down to make room for logo
		// If there isn't enough room, only take the bottom rows
		if(24 - tty_row < TTY_ROW_MIN) {
			row_offset = TTY_ROW_MIN - (24 - tty_row);
		}
		
		// Make a copy into the terminal_pages
		// Assuming that there is only one page
		memmove(tty_pages[0], tty_buffer, tty_row * VGA_WIDTH * 2);
		
		// Move terminal_row rows down 7
		memmove(tty_buffer + (TTY_ROW_MIN * VGA_WIDTH), tty_buffer + (row_offset * VGA_WIDTH), tty_row * VGA_WIDTH * 2);
		
		// Set the top 7 rows blank
		memset(tty_buffer, (int) blank, VGA_WIDTH * TTY_ROW_MIN * 2);
		tty_row += row_offset + TTY_ROW_MIN;
	} else { // If no parameters
		// Clear the screen
		tty_row = 0;
		tty_column = 0;
		memset(tty_buffer, (int) blank, VGA_WIDTH * VGA_HEIGHT * 2);
		tty_row = TTY_ROW_MIN;
	}
	
	// Print the logo
	tty_print_logo();
	
	display_page_number();
	
	//vga_set_cursor_shape(VGA_CURSOR_SHAPE_BLOCK);
	
	// Enable and show the cursor at the end of the line
	vga_enable_cursor();
	tty_update_cursor();
}
