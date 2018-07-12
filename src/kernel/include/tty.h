/**
 *  \file tty.h
 *  \brief The terminal interface that uses vga display to display strings to the screen. The
 *  screen is a 80x25 character display using the video memory address to display the characters.
 *  Also initialises the display depending whether parameters where given from the bootloader.
 *  
 *  \todo Don't have RTC use the TTY for setting the time, maybe? Could just have a loop that gets
 *  the the current time and the RTC just sets a variable so the interrupt don't have to print to
 *  the screen as this takes time.
 */
#ifndef INCLUDE_TTY_H
#define INCLUDE_TTY_H

#include <stddef.h>
#include <boot.h>
#include <rtc.h>

/**
 *  The upper 7 rows are reserved for the logo and the logo is fixed and won't scroll, so this is
 *  the start of the top of the screen below the logo.
 */
#define TERMINAL_ROW_MIN 8

/**
 *  \brief Set the display time to the time given.
 *  
 *  \param [in] date The date and time that will be copied to the internal variable.
 */
void set_time(rtc_date_time_t * date);

/**
 *  \brief Get the current time that is being used for the display.
 *  
 *  \param [in] date The data and time pointer that the display time will be copied into.
 *  \return The data and time from the display, same pointer as parameter given with new values
 *  set.
 *  
 *  \todo Maybe remove and just use the RTC interface.
 */
rtc_date_time_t * get_time(rtc_date_time_t * date);

/**
 *  \brief Set the display time/clock on the display below the logo. Displays the day, date and
 *  time.
 */
void set_display_time(void);

/**
 *  \brief Move the cursor to the left by one place.
 */
void move_cursor_left(void);

/**
 *  \brief Move the cursor to the right by one place.
 */
void move_cursor_right(void);

/**
 *  \brief Clear the display below the logo.
 */
void clear(void);

/**
 *  \brief Initialise the terminal.
 *  
 *  \param [in] params The parameters from the bootloader that can contain the cursor position.
 *  This may be NULL.
 *  
 *  \details Initialise the terminal based on the parameters from the bootloader. If the parameters
 *  are NULL, no parameters provided or the signature isn't correct, then the terminal is cleared
 *  and the cursor starts at (0, 0) at the top left corner of the display. If there are parameters
 *  provided, then the display continues from where the bootloader left off.
 */
void terminal_initialise(boot_params * params);

/**
 *  \brief Print a character to the display with the set colour.
 *  
 *  \param [in] c The character to be printed.
 */
void terminal_put_char(char c);

/**
 *  \brief Set the colour (foreground and background) of the terminal.
 *  
 *  \param [in] colour The foreground and background colour.
 */
void terminal_set_color(uint8_t colour);

/**
 *  \brief Write a set of bytes/characters to the display of size size.
 *  
 *  \param [in] data A pointer to the data to print to the display.
 *  \param [in] size The size in bytes of the data to print.
 */
void terminal_write(const char * data, size_t size);

/**
 *  \brief Write a null terminated string to the display.
 *  
 *  \param [in] str The string to display.
 */
void terminal_write_string(const char * str);

#endif /* INCLUDE_TTY_H */
