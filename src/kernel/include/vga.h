/**
 *  \file vga.h
 *  \brief A very simple VGA driver.
 */
#ifndef INCLUDE_VGA_H
#define INCLUDE_VGA_H
 
#include <stdint.h>

/**
 *  \brief The width of the display, 80 characters wide.
 */
#define VGA_WIDTH		80

/**
 *  \brief The height of the display, 25 characters long.
 */
#define VGA_HEIGHT		25

/**
 *  \brief The memory mapped location of the display memory to write data to to display characters
 *  and colour.
 */
#define VGA_MEMORY		(uint16_t *) 0x000B8000

/**
 *  \brief If set, disables the cursor.
 */
#define VGA_CURSOR_DISABLE	0x20

/**
 *  \brief The VGA port addresses.
 */
enum vga_port_address {
	VGA_PORT_CRT_ADDRESS			= 0x03D4,	/**< The port address for the VGA register selection. */
	VGA_PORT_CRT_DATA				= 0x03D5	/**< The port address for the VGA data. */
};

/**
 *  \brief The indexes that is passed to the CRT address port to select the register for the data
 *  to be read or written to.
 */
enum vga_cursor_scan_line {
	VGA_REG_HORIZONTAL_TOTAL				= 0x00,		/**< \todo TODO. */
	VGA_REG_HORIZONTAL_DISPLAY_ENABLE_END	= 0x01,		/**< \todo TODO. */
	VGA_REG_START_HORIZONTAL_BLINKING		= 0x02,		/**< \todo TODO. */
	VGA_REG_END_HORIZONTAL_BLINKING			= 0x03,		/**< \todo TODO. */
	VGA_REG_START_HORIZONTAL_RETRACE_PULSE	= 0x04,		/**< \todo TODO. */
	VGA_REG_END_HORIZONTAL_RETRACE_PULSE	= 0x05,		/**< \todo TODO. */
	VGA_REG_VERTICAL_TOTAL					= 0x06,		/**< \todo TODO. */
	VGA_REG_OVERFLOW						= 0x07,		/**< \todo TODO. */
	VGA_REG_PRESET_ROW_SCAN					= 0x08,		/**< \todo TODO. */
	VGA_REG_MAXIMUM_SCAN_LINE				= 0x09,		/**< \todo TODO. */
	VGA_REG_CURSOR_START					= 0x0A,		/**< The command for setting the start of the cursor scan line. */
	VGA_REG_CURSOR_END						= 0x0B,		/**< The command for setting the end of the cursor scan line. */
	VGA_REG_START_ADDRESS_HIGH				= 0x0C,		/**< \todo TODO. */
	VGA_REG_START_ADDRESS_LOW				= 0x0D,		/**< \todo TODO. */
	VGA_REG_CURSOR_LOCATION_HIGH			= 0x0E,		/**< The command for setting the upper byte of the cursor's linear location. */
	VGA_REG_CURSOR_LOCATION_LOW				= 0x0F,		/**< The command for setting the lower byte of the cursor's linear location. */
	VGA_REG_VERTICAL_RETRACE_START			= 0x10,		/**< \todo TODO. */
	VGA_REG_VERTICAL_RETRACE_END			= 0x11,		/**< \todo TODO. */
	VGA_REG_VERTICAL_DISPLAY_ENABLE_END		= 0x12,		/**< \todo TODO. */
	VGA_REG_OFFSET							= 0x13,		/**< \todo TODO. */
	VGA_REG_UNDERLINE_LOCATION				= 0x14,		/**< \todo TODO. */
	VGA_REG_START_VERTICAL_BLINKING			= 0x15,		/**< \todo TODO. */
	VGA_REG_END_VERTICAL_BLINKING			= 0x16,		/**< \todo TODO. */
	VGA_REG_CRT_MODE_CONTROL				= 0x17,		/**< \todo TODO. */
	VGA_REG_LINE_COMPARE					= 0x18		/**< \todo TODO. */
};

/**
 *  \brief The values for the cursor scan line command.
 */
enum vga_cursor_scan_line_values {
	VGA_CURSOR_SCANLINE_MINIMUM		= 0x00,		/**< The minimum scan line of the cursor, the very beginning. */
	VGA_CURSOR_SCANLINE_START		= 0x00,		/**< The start of the cursor scan line, the very beginning. */
	VGA_CURSOR_SCANLINE_MIDDLE		= 0x0E,		/**< The scan line for use in the underline cursor shape. */
	VGA_CURSOR_SCANLINE_END			= 0x0F,		/**< The end of the cursor scan line, the very end. */
	VGA_CURSOR_SCANLINE_MAXIMUM		= 0x0F		/**< The maximum scan line of the cursor, the very end. */
};

/**
 *  \brief The set of colours that VGA supports and can display for the foreground and background.
 */
typedef enum vga_colour {
	VGA_COLOUR_BLACK				= 0x00,		/**< The VGA colour value of black. */
	VGA_COLOUR_BLUE					= 0x01,		/**< The VGA colour value of blue. */
	VGA_COLOUR_GREEN				= 0x02,		/**< The VGA colour value of green. */
	VGA_COLOUR_CYAN					= 0x03,		/**< The VGA colour value of cyan. */
	VGA_COLOUR_RED					= 0x04,		/**< The VGA colour value of red. */
	VGA_COLOUR_MAGENTA				= 0x05,		/**< The VGA colour value of magenta. */
	VGA_COLOUR_BROWN				= 0x06,		/**< The VGA colour value of brown. */
	VGA_COLOUR_LIGHT_GREY			= 0x07,		/**< The VGA colour value of light grey. */
	VGA_COLOUR_DARK_GREY			= 0x08,		/**< The VGA colour value of dark grey. */
	VGA_COLOUR_LIGHT_BLUE			= 0x09,		/**< The VGA colour value of light blue. */
	VGA_COLOUR_LIGHT_GREEN			= 0x0A,		/**< The VGA colour value of light green. */
	VGA_COLOUR_LIGHT_CYAN			= 0x0B,		/**< The VGA colour value of light cyan. */
	VGA_COLOUR_LIGHT_RED			= 0x0C,		/**< The VGA colour value of light red. */
	VGA_COLOUR_LIGHT_MAGENTA		= 0x0D,		/**< The VGA colour value of light magenta. */
	VGA_COLOUR_LIGHT_BROWN			= 0x0E,		/**< The VGA colour value of light brown. */
	VGA_COLOUR_WHITE				= 0x0F		/**< The VGA colour value of white. */
} vga_colour_t;

/**
 *  \brief The set of shapes that can be displayed.
 */
typedef enum vga_cursor_shape {
	VGA_CURSOR_SHAPE_UNDERLINE,	/**< The cursor has the underline shape. */
	VGA_CURSOR_SHAPE_BLOCK		/**< The cursor has the block shape. */
} vga_cursor_shape_t;

/**
 *  \brief Takes two 4 bit values that represent the foreground and background colour of the text
 *  and returns a 8 bit value that gives both to be displayed
 *  
 *  \param [in] fg The foreground colour
 *  \param [in] bg The background colour
 *  \return Both combined into the 1 byte for the colour to be displayed
 */
uint8_t vga_entry_colour(vga_colour_t fg, vga_colour_t bg);

/**
 *  \brief Create the 2 bytes entry that the VGA used to display a character with a foreground
 *  and background colour
 *  
 *  \param [in] uc The character
 *  \param [in] colour The foreground and background colour
 *  \return The VGA entry
 */
uint16_t vga_entry(unsigned char uc, uint8_t colour);

/**
 *  \brief Enables the blinking cursor to that is is visible.
 */
void vga_enable_cursor(void);

/**
 *  \brief Disables the blinking cursor to that is is visible.
 */
void vga_disable_cursor(void);

/**
 *  \brief Update the cursor position to the end of the current line or character printed.
 *  
 *  \param [in] x The horizontal position of the cursor.
 *  \param [in] y The vertical position of the cursor.
 */
void vga_update_cursor(uint16_t x, uint16_t y);

/** 
 *  \brief Set the shape of the cursor. This can be and underline or block shape.
 *
 *  \param [in] shape The enum \ref vga_cursor_shape_t that selects which shape to use.
 */     

void vga_set_cursor_shape(vga_cursor_shape_t shape);

/**
 *  \brief Initialise the VGA text mode. This just sets the cursor and underline shape.
 */
void vga_init(void);

#endif /* INCLUDE_VGA_H */
