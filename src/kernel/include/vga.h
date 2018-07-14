/**
 *  \file vga.h
 *  \brief A very simple VGA driver that takes a foreground and background colour and character to
 *  print and returns the entry to be places in memory
 */
#ifndef INCLUDE_VGA_H
#define INCLUDE_VGA_H
 
#include <stdint.h>

#define VGA_WIDTH						80						/**< The width of the display. 80 characters wide. */
#define VGA_HEIGHT						25						/**< The height of the display. 25 characters long. */
#define VGA_MEMORY						(uint16_t *) 0xB8000	/**< The memory mapped location of the display memory to write data to to displayt characters and colour. */

#define VGA_PORT_CRTC_ADDRESS			0x03D4					/**< The port address for the VGA address. */
#define	VGA_PORT_CRTC_DATA				0x03D5					/**< The port address for the VGA data. */

#define VGA_REG_CURSOR_START			0x0A					/**< The command for setting the start of the cursor scan line. */
#define VGA_REG_CURSOR_END				0x0B					/**< The command for setting the end of the cursor scan line. */

#define VGA_REG_CURSOR_LOCATION_HIGH	0x0E					/**< The command for setting the upper byte of the curser's linear location. */
#define VGA_REG_CURSOR_LOCATION_LOW		0x0F					/**< The command for setting the lower byte of the curser's linear location. */

#define VGA_CURSOR_SCANLINE_START		0x00					/**< The start of the cursor scan line, the very beginning. */
#define VGA_CURSOR_SCANLINE_END			0x0F					/**< The end of the cursor scan line, the very end. */

/**
 *  \brief The set of colours that VGA supports and can display for the foreground and background
 */
enum vga_colour {
	VGA_COLOUR_BLACK			= 0,
	VGA_COLOUR_BLUE				= 1,
	VGA_COLOUR_GREEN			= 2,
	VGA_COLOUR_CYAN				= 3,
	VGA_COLOUR_RED				= 4,
	VGA_COLOUR_MAGENTA			= 5,
	VGA_COLOUR_BROWN			= 6,
	VGA_COLOUR_LIGHT_GREY		= 7,
	VGA_COLOUR_DARK_GREY		= 8,
	VGA_COLOUR_LIGHT_BLUE		= 9,
	VGA_COLOUR_LIGHT_GREEN		= 10,
	VGA_COLOUR_LIGHT_CYAN		= 11,
	VGA_COLOUR_LIGHT_RED		= 12,
	VGA_COLOUR_LIGHT_MAGENTA	= 13,
	VGA_COLOUR_LIGHT_BROWN		= 14,
	VGA_COLOUR_WHITE			= 15,
};

/**
 *  \brief Take the foreground and background colour and return both combined to be used of the vga
 *  display
 *  
 *  \param [in] fg The foreground colour
 *  \param [in] bg The background colour
 *  \return Both combined into the 1 byte for the colour to be displayed
 *  
 *  \details Takes 2 4 bit values that represent the foreground and background colour of the text
 *  and returns a 8 bit value that gives both to be displayed
 */
uint8_t vga_entry_colour(enum vga_colour fg, enum vga_colour bg);

/**
 *  \brief Create a VGA entry from the character, foreground and background colour
 *  
 *  \param [in] uc The character
 *  \param [in] colour The foreground and background colour
 *  \return The VGA entry
 *  
 *  \details Create the 2 bytes entry that the VGA used to display a character with a foreground
 *  and background colour
 */
uint16_t vga_entry(unsigned char uc, uint8_t colour);

/**
 *  \brief Enables the blinking cursor to that is is visible.
 */
void enable_cursor(void);

/**
 *  \brief Disables the blinking cursor to that is is visible.
 */
void disable_cursor(void);

#endif /* INCLUDE_VGA_H */
