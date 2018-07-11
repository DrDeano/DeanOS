/**
 *  \file tty.h
 *  \brief The terminal interface that uses vga display to display strings to the screen. The
 *  screen is a 80x25 character display using the video memory address to display the characters.
 *  Also initialises the display depending whether parameters where given from the bootloader.
 */
#ifndef INCLUDE_TTY_H
#define INCLUDE_TTY_H

#include <stddef.h>
#include <boot.h>
#include <rtc.h>

#define TERMINAL_ROW_MIN 8

void set_time(rtc_date_time_t * date);
rtc_date_time_t * get_time(rtc_date_time_t * date);
void set_display_time();

void move_cursor_left();
void move_cursor_right();

void clear();

/**
 *  \brief Initialise the terminal
 *  
 *  \param [in] params The parameters from the bootloader that can contain the cursor position.
 *  This may be NULL.
 *  \return No return value
 *  
 *  \details Initialise the terminal based on the parameters from the bootloader. If the parameters
 *  are NULL, no parameters provided or the signature isn't correct, then the terminal is cleared
 *  and the cursor starts at (0, 0) at the top left corner of the display. If there are parameters
 *  provided, then the display continues from where the bootloader left off.
 */
void terminal_initialise(boot_params * params);

/**
 *  \brief Print a character to the display
 *  
 *  \param [in] c The character to be printed
 *  \return No return value
 *  
 *  \details Display a the given character at the current cursor position and update the cursor
 */
void terminal_put_char(char c);

/**
 *  \brief Set the colour (forground and background) of the terminal
 *  
 *  \param [in] colour The forground and background colour
 *  \return No return value
 */
void terminal_set_color(uint8_t colour);

/**
 *  \brief Write a set of bytes/characters to the display of size size.
 *  
 *  \param [in] data A pointer to the data to print to the display
 *  \param [in] size The size in bytes of the data to print
 *  \return No returen value
 *  
 *  \details Iterate through the data byte by byte and display it to the display for length size.
 */
void terminal_write(const char * data, size_t size);

/**
 *  \brief Write a null terminated string to the display
 *  
 *  \param [in] str The string to display
 *  \return No return value
 *  
 *  \details Take a null terminating string and print that to the display
 */
void terminal_write_string(const char * str);

#endif /* INCLUDE_TTY_H */
