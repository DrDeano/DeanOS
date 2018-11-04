/**
 * \file stdio.h
 * \brief The C Library: stdio
 */
#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdarg.h>

/**
 * \brief This macro is a negative integer, which indicates that the end-of-file has been reached.
 */
#define EOF		-1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Writes a character (an unsigned char) specified by the argument char to the display.
 * 
 * \param [in] c This is the character to be written. This is passed as its int promotion.
 * \return This function returns the character written as an unsigned char cast to an int or EOF on error.
 */
int kputchar(int c);

/**
 * \brief Prints the formatted string to the display.
 * 
 * \param [in] format This is the string that contains the text to be written to the display. It
 * can optionally contain embedded format tags that are replaced by the values specified in
 * subsequent additional arguments and formatted as requested.
 * 
 * \see https://en.wikipedia.org/wiki/Printf_format_string for formating options and prototype.
 * 
 * \param [in] ... Depending on the format string, the function may expect a sequence of
 * additional arguments, each containing one value to be inserted instead of each %-tag specified
 * in the format parameter (if any). There should be the same number of these arguments as the
 * number of %-tags that expect a value.
 * \return If successful, non-negative value is returned. On error, the function returns EOF.
 */
int kprintf(const char * format, ...);

/**
 * \brief Prints formatted output to the display using an argument list.
 * 
 * \param [in] format This is the string that contains the text to be written to the display. It
 * can optionally contain embedded format tags that are replaced by the values specified in
 * subsequent additional arguments and formatted as requested.
 * 
 * \see https://en.wikipedia.org/wiki/Printf_format_string for formating options and prototype.
 * 
 * \param [in] args An object representing the variable arguments list. This should be initialized
 * by the va_start macro defined in <stdarg.h>.
 * \return If successful, non-negative value is returned. On error, the function returns EOF.
 */
int kvprintf(const char * format, va_list args);

/**
 * \brief Writes a string to the display up to but not including the null character. A newline
 * character is appended to the output.
 * 
 * \param [in] str This is the C string to be written.
 * \return If successful, non-negative value is returned. On error, the function returns EOF.
 */
int kputs(const char * str);

#ifdef __cplusplus
}
#endif
 
#endif /* _STDIO_H */
