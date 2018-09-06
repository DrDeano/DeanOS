#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include <tty.h>

/**
 *  \brief The flags structure that holds what flags have been used in the current formated string.
 */
typedef struct {
	bool left_align;	/**< The left alignment flag. */
	bool plus;			/**< Whether a + is append to positive numbers. */
	bool space;			/**< Whether the extra width is filled with spaces. */
	bool zero;			/**< Whether the extra width is filled with zeros. */
	bool hash;			/**< \see https://en.wikipedia.org/wiki/Printf_format_string */
} flags;

int kputchar(int c) {
	char ch = (char) c;
	terminal_put_char(ch);
	return 1;
}

/**
 *  \brief Helper function for prints a C string.
 *
 *  \param [in] str   The string to print.
 *  \param [in] width The width field.
 *  \return The number for characters printed.
 */
static int kprint_string(char * str, size_t width) {
	int written = 0;

	if (width == 0) { // Print full string
		terminal_write_string(str);
		written += strlen(str);
		/* while(*str) {
			kputchar(*str);
			str++;
			written++;
		} */
	} else { // Print up to width
		written += width;
		while(*str && width) {
			kputchar(*str);
			str++;
			width--;
		}
	}

	return written;
}

/**
 *  \brief Print a 8 bit value in hexadecimal
 *
 *  \param [in] num     The 8 bit value to print
 *  \param [in] capital Whether to print the letters in lower or upper case
 *  \param [in] pad     Whether the 8 bit value is really 4 bits so only print the lower 4 bits
 *
 *  \return The number of characters printed
 */
static int kprint_hex_digit(uint8_t num, bool capital, bool pad) {
	const char * hex = capital ? "0123456789ABCDEF" : "0123456789abcdef";
	const uint8_t high = (num & 0xF0) >> 4;
	const uint8_t low = num & 0x0F;
	int ret = 1;

	// If high is 0, and no padding, then don't print the leading 0
	if (high != 0 || pad) {
		kputchar(hex[high]);
		ret++;
	}
	kputchar(hex[low]);
	return ret;
}

/**
 *  \brief Print a signed 64 bit integer
 *
 *  \param [in] num The integer to print
 *
 *  \return The number of characters printed
 */
static int kprint_int(int32_t num, uint8_t width, flags flag) {
	// If 0, then just print 0
	int j;
	int k = 1;
	if (num == 0) {
		for(j = width - 1; j > 0; j--) {
			if(flag.zero) {
				kputchar('0');
			} else {
				kputchar(' ');
			}
			k++;
		}
		kputchar('0');
		return k;
	}

	// Is the number negative
	bool negative = num < 0;

	// Get the absolute value
	num = negative ? -num : num;

	// Buffer to store the string representation of the number
	// 1 for the sign (+ or -), 20 for digits of 64 bit number, 1 for null terminating
	char buffer[1 + 20 + 1];
	buffer[sizeof(buffer) - 1] = '\0';

	// Walk through the number backwards
	int i = sizeof(buffer) - 2;
	while(num > 0) {
		buffer[i--] = '0' + (num % 10);
		num /= 10;
	}


	if((sizeof(buffer) - i - 2) < width) {
		for(j = width - (sizeof(buffer) - i - 2); j > 0; j--) {
			if(flag.zero) {
				buffer[i--] = '0';
			} else {
				buffer[i--] = ' ';
			}
		}
	}

	// Add sign
	if (negative) {
		buffer[i--] = '-';
	}

	// Print string representation
	kprint_string(&buffer[i + 1], 0);

	return sizeof(buffer) - i - 2;
}

/**
 *  \brief Print a unsigned 64 bit integer
 *
 *  \param [in] num The integer to print
 *
 *  \return The number of characters printed
 */
static int kprint_uint(uint32_t num, uint8_t width, flags flag) {
	// If 0, then just print 0
	if (num == 0) {
		kputchar('0');
		return 1;
	}

	// Buffer to store the string representation of the number
	// 20 for digits of 64 bit number, 1 for null terminating
	char buffer[20 + 1];
	buffer[sizeof(buffer) - 1] = '\0';

	// Walk through the number backwards
	int i = sizeof(buffer) - 2;
	while(num > 0) {
		buffer[i--] = '0' + (num % 10);
		num /= 10;
	}

	int j;
	if((sizeof(buffer) - i - 2) < width) {
		for(j = width - (sizeof(buffer) - i - 2); j > 0; j--) {
			if(flag.zero) {
				buffer[i--] = '0';
				//written += kputchar('0');
			} else {
				buffer[i--] = ' ';
				//written += kputchar(' ');
			}
		}
	}

	// Print string representation
	kprint_string(&buffer[i + 1], 0);

	return sizeof(buffer) - i - 2;
}

/**
 *  \brief Print an unsigned integer as hexadecimal
 *
 *  \param [in] num     The number to print
 *  \param [in] capital Whether to print the hex with upper or lower case letters
 *  \param [in] width   How wide are we printing. How many hex characters are we printing.
 *
 *  \return The number of characters printed
 */
static int kprint_hex(const uint32_t num, const bool capital, uint8_t width) {
	// This is the fist hex. If has a leading 0, don't print the leading 0
	bool first = true;

	// The amount written
	int written = 0;

	// Loop over the number
	for(int i = sizeof(num) - 1; i >= 0; i--) {
		// Get the next bytes
		const uint8_t byte = (num >> (8 * i)) & 0xFF;

		// Is width specified and has we printed the width
		if (width && (i * 2) >= width) {
			continue;
		} else if (width == 0) { // No width specified
			// Continue until we get the first non zero byte
			if (first && byte == 0 && i > 0) {
				continue;
			}
		}

		// Print the hex digit
		written += kprint_hex_digit(byte, capital, !first ? true : width > 0);
		// No long the fist hex digit
		first = false;
		// Decrease the width by 2 as hex characters are 2 characters wide
		if(width <= 0) {
			width = 0;
		} else {
			width -= 2;
		}
	}

	return written;
}

int kvprintf(const char * format, va_list args) {
	int written = 0;

	flags flag;

	size_t width;
	//size_t precision;

	char c;
	while (*format) {
		// Get the next character
		c = *format++;

		// Reset flags
		flag.left_align = false;
		flag.plus = false;
		flag.space = false;
		flag.zero = false;
		flag.hash = false;

		width = 0;
		//precision = 0;

		if (c == '%') { // Is it the start of a format
			char f = *format++;

			// Check for flag field
			while(f == '-' || f == '+' || f == ' ' || f == '0' || f == '#') {
				switch (f) {
					case '-': // Left align
						//flag.left_align = true;
						break;
					case '+': // Append a + to positive numbers
						//flag.plus = true;
						//flag.space = false;
						break;
					case ' ': // Append a (space) to positive numbers if + isn't used
						//if (!flag.plus) {
							//flag.space = true;
						//}
						break;
					case '0': // Append leading zero. Default a space
						flag.zero = true;
						break;
					case '#': // See https://en.wikipedia.org/wiki/Printf_format_string
						//flag.hash = true;
						break;
					default:
						/**
						 *  \todo Return error?
						 */
						break;
				}
				f = *format++;
			}


			// Check for width field. A minimum width
			if (f >= '0' && f <= '9') {
				width = f - '0';
				f = *format++;
			} else if (f == '*') { // Argument is given
				width = va_arg(args, size_t);
				f = *format++;
			}

			// Check for precision, will have a . at the start
			if (f == '.') {
				// Get the precision field
				/**
				 *  \todo Use precision
				 */
				f = *format++;

				if (f >= '0' && f <= '9') {
					//precision = f - '0';
					f = *format++;
				} else if (f == '*') { // Argument is given
					//precision = va_arg(args, size_t);
					f = *format++;
				}
			}

			// Check the length field
			// Use flags?
			// See https://en.wikipedia.org/wiki/Printf_format_string
			char f_next = *format;
			if (f == 'h' && f_next == 'h') { // int size from promoted char
			/**
			 *  \todo
			 */
			kputchar('F');
			} else if (f == 'h') { // int size from promoted short
			/**
			 *  \todo
			 */
			kputchar('F');
			} else if (f == 'l' && f_next == 'l') { // long long 64 bit
			/**
			 *  \todo
			 */
			kputchar('F');
			} else if (f == 'l') { // long 32 bit
			/**
			 *  \todo
			 */
			kputchar('F');
			} else if (f == 'L') { // long double float
			/**
			 *  \todo
			 */
			kputchar('F');
			} else if (f == 'z') { // size_t
			/**
			 *  \todo
			 */
			kputchar('F');
			} else if (f == 'j') { // intmax_t
			/**
			 *  \todo
			 */
			kputchar('F');
			} else if (f == 't') { // ptrdiff_t
			/**
			 *  \todo
			 */
			kputchar('F');
			} else {
				const void * ptr;
				switch (f) {
					case '%':
						written += kputchar('%');
						break;

					case 'd':
					case 'i':
						written += kprint_int(va_arg(args, int32_t), width, flag);
						break;

					case 'u':
						written += kprint_uint(va_arg(args, uint32_t), width, flag);
						break;

					case 'f':
					case 'F':
						/**
						 *  \todo
						 */
						kputchar('N');
						break;

					case 'e':
					case 'E':
						/**
						 *  \todo
						 */
						kputchar('N');
						break;

					case 'g':
					case 'G':
						/**
						 *  \todo
						 */
						kputchar('N');
						break;

					case 'x':
						written += kprint_hex(va_arg(args, uint32_t), false, width);
						break;

					case 'X':
						written += kprint_hex(va_arg(args, uint32_t), true, width);
						break;
					case 'o':
						/**
						 *  \todo
						 */
						kputchar('N');
						break;

					case 's':
						written += kprint_string(va_arg(args, char *), width);
						break;

					case 'c':
						written += kputchar((char) va_arg(args, int));
						break;

					case 'p':
						ptr = (void *) va_arg(args, void *);

						if (ptr == NULL) { // Print '(null)'
							// Make sure to print the full string
							/* if (precision < 5) {
								precision = 5;
							} */
							written += kprint_string("(null)", 0);
						} else { // Print as a hex number
							written += kprint_hex((uint32_t) ptr, false, width);
						}
						break;

					case 'a':
					case 'A':
						/**
						 *  \todo
						 */
						kputchar('N');
						break;

					case 'n':
						/**
						 *  \todo
						 */
						kputchar('N');
						break;

					default:
						/**
						 *  \todo
						 */
						kputchar('N');
						break;
				}
			}
		} else { // Just print the character
			written += kputchar(c);
		}
	}

	/**
	 *  \todo Added support so can change colour of display
	 */
	return written;
}

int kprintf(const char * format, ...) {
	int written = 0;

	va_list args;
	va_start(args, format);

	written += kvprintf(format, args);

	va_end(args);
	return written;
}

int kputs(const char * str) {
	return kprintf("%s\n", str);
}
