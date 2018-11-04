#include <ctype.h>

/**
 * \brief The different types of charactures to be used for ctype characture checking dunctions.
 */
enum char_types {
	UPPER_CHAR			= 0x01,	/**< A upper case mask for checking if the character is a upper case letter. */
	LOWER_CHAR			= 0x02,	/**< A lower case mask for checking if the character is a lower case letter. */
	DIGIT_CHAR			= 0x04,	/**< A digit case mask for checking if the character is a digit. */
	CONTROL_CHAR		= 0x08,	/**< A control character mask for checking if the character is a control character. */
	PUNCTUATION_CHAR	= 0x10,	/**< A punctuation mask for checking if the character is a punctuation letter. */
	WHITE_SPACE_CHAR	= 0x20,	/**< A white space mask for checking if the character is a white space like space, tab, return carriage... */
	HEX_DIGIT_CHAR		= 0x40,	/**<  A hex digit mask for checking if the character is a hex digit.*/
	EXTENDED_CHAR		= 0x80	/**< The extended ASCII characters mask for checking if the character is a extended ASCII characters. Used for checking if is printable, as the extended range aren't in the above categories but can be printed. */
};

/**
 * \brief The list of all character represented as the masks so can be masked off to determine,
 * for example, is a digit or not.
 */
static const char characters[257] = {
	CONTROL_CHAR,						// 0x00 (NUL)
	CONTROL_CHAR,						// 0x01 (SOH)
	CONTROL_CHAR,						// 0x02 (STX)
	CONTROL_CHAR,						// 0x03 (ETX)
	CONTROL_CHAR,						// 0x04 (EOT)
	CONTROL_CHAR,						// 0x05 (ENQ)
	CONTROL_CHAR,						// 0x06 (ACK)
	CONTROL_CHAR,						// 0x07 (BEL)
	CONTROL_CHAR,						// 0x08 (BS)
	WHITE_SPACE_CHAR | CONTROL_CHAR,	// 0x09 (HT)
	WHITE_SPACE_CHAR | CONTROL_CHAR,	// 0x0A (LF)
	WHITE_SPACE_CHAR | CONTROL_CHAR,	// 0x0B (VT)
	WHITE_SPACE_CHAR | CONTROL_CHAR,	// 0x0C (FF)
	WHITE_SPACE_CHAR | CONTROL_CHAR,	// 0x0D (CR)
	CONTROL_CHAR,						// 0x0E (SI)
	CONTROL_CHAR,						// 0x0F (SO)
	CONTROL_CHAR,						// 0x10 (DLE)
	CONTROL_CHAR,						// 0x11 (DC1)
	CONTROL_CHAR,						// 0x12 (DC2)
	CONTROL_CHAR,						// 0x13 (DC3)
	CONTROL_CHAR,						// 0x14 (DC4)
	CONTROL_CHAR,						// 0x15 (NAK)
	CONTROL_CHAR,						// 0x16 (SYN)
	CONTROL_CHAR,						// 0x17 (ETB)
	CONTROL_CHAR,						// 0x18 (CAN)
	CONTROL_CHAR,						// 0x19 (EM)
	CONTROL_CHAR,						// 0x1A (SUB)
	CONTROL_CHAR,						// 0x1B (ESC)
	CONTROL_CHAR,						// 0x1C (FS)
	CONTROL_CHAR,						// 0x1D (GS)
	CONTROL_CHAR,						// 0x1E (RS)
	CONTROL_CHAR,						// 0x1F (US)
	WHITE_SPACE_CHAR,					// 0x20 SPACE
	PUNCTUATION_CHAR,					// 0x21 !
	PUNCTUATION_CHAR,					// 0x22 "
	PUNCTUATION_CHAR,					// 0x23 #
	PUNCTUATION_CHAR,					// 0x24 $
	PUNCTUATION_CHAR,					// 0x25 %
	PUNCTUATION_CHAR,					// 0x26 &
	PUNCTUATION_CHAR,					// 0x27 '
	PUNCTUATION_CHAR,					// 0x28 (
	PUNCTUATION_CHAR,					// 0x29 )
	PUNCTUATION_CHAR,					// 0x2A *
	PUNCTUATION_CHAR,					// 0x2B +
	PUNCTUATION_CHAR,					// 0x2C ,
	PUNCTUATION_CHAR,					// 0x2D -
	PUNCTUATION_CHAR,					// 0x2E .
	PUNCTUATION_CHAR,					// 0x2F /
	DIGIT_CHAR | HEX_DIGIT_CHAR,		// 0x30 0
	DIGIT_CHAR | HEX_DIGIT_CHAR,		// 0x31 1
	DIGIT_CHAR | HEX_DIGIT_CHAR,		// 0x32 2
	DIGIT_CHAR | HEX_DIGIT_CHAR,		// 0x33 3
	DIGIT_CHAR | HEX_DIGIT_CHAR,		// 0x34 4
	DIGIT_CHAR | HEX_DIGIT_CHAR,		// 0x35 5
	DIGIT_CHAR | HEX_DIGIT_CHAR,		// 0x36 6
	DIGIT_CHAR | HEX_DIGIT_CHAR,		// 0x37 7
	DIGIT_CHAR | HEX_DIGIT_CHAR,		// 0x38 8
	DIGIT_CHAR | HEX_DIGIT_CHAR,		// 0x39 9
	PUNCTUATION_CHAR,					// 0x3A :
	PUNCTUATION_CHAR,					// 0x3B ;
	PUNCTUATION_CHAR,					// 0x3C <
	PUNCTUATION_CHAR,					// 0x3D =
	PUNCTUATION_CHAR,					// 0x3E >
	PUNCTUATION_CHAR,					// 0x3F ?
	PUNCTUATION_CHAR,					// 0x40 @
	UPPER_CHAR | HEX_DIGIT_CHAR,		// 0x41 A
	UPPER_CHAR | HEX_DIGIT_CHAR,		// 0x42 B
	UPPER_CHAR | HEX_DIGIT_CHAR,		// 0x43 C
	UPPER_CHAR | HEX_DIGIT_CHAR,		// 0x44 D
	UPPER_CHAR | HEX_DIGIT_CHAR,		// 0x45 E
	UPPER_CHAR | HEX_DIGIT_CHAR,		// 0x46 F
	UPPER_CHAR,							// 0x47 G
	UPPER_CHAR,							// 0x48 H
	UPPER_CHAR,							// 0x49 I
	UPPER_CHAR,							// 0x4A J
	UPPER_CHAR,							// 0x4B K
	UPPER_CHAR,							// 0x4C L
	UPPER_CHAR,							// 0x4D M
	UPPER_CHAR,							// 0x4E N
	UPPER_CHAR,							// 0x4F O
	UPPER_CHAR,							// 0x50 P
	UPPER_CHAR,							// 0x51 Q
	UPPER_CHAR,							// 0x52 R
	UPPER_CHAR,							// 0x53 S
	UPPER_CHAR,							// 0x54 T
	UPPER_CHAR,							// 0x55 U
	UPPER_CHAR,							// 0x56 V
	UPPER_CHAR,							// 0x57 W
	UPPER_CHAR,							// 0x58 X
	UPPER_CHAR,							// 0x59 Y
	UPPER_CHAR,							// 0x5A Z
	PUNCTUATION_CHAR,					// 0x5B [
	PUNCTUATION_CHAR,					// 0x5C '\'
	PUNCTUATION_CHAR,					// 0x5D ]
	PUNCTUATION_CHAR,					// 0x5E ^
	PUNCTUATION_CHAR,					// 0x5F _
	PUNCTUATION_CHAR,					// 0x60 `
	LOWER_CHAR | HEX_DIGIT_CHAR,		// 0x61 a
	LOWER_CHAR | HEX_DIGIT_CHAR,		// 0x62 b
	LOWER_CHAR | HEX_DIGIT_CHAR,		// 0x63 c
	LOWER_CHAR | HEX_DIGIT_CHAR,		// 0x64 d
	LOWER_CHAR | HEX_DIGIT_CHAR,		// 0x65 e
	LOWER_CHAR | HEX_DIGIT_CHAR,		// 0x66 f
	LOWER_CHAR,							// 0x67 g
	LOWER_CHAR,							// 0x68 h
	LOWER_CHAR,							// 0x69 i
	LOWER_CHAR,							// 0x6A j
	LOWER_CHAR,							// 0x6B k
	LOWER_CHAR,							// 0x6C l
	LOWER_CHAR,							// 0x6D m
	LOWER_CHAR,							// 0x6E n
	LOWER_CHAR,							// 0x6F o
	LOWER_CHAR,							// 0x70 p
	LOWER_CHAR,							// 0x71 q
	LOWER_CHAR,							// 0x72 r
	LOWER_CHAR,							// 0x73 s
	LOWER_CHAR,							// 0x74 t
	LOWER_CHAR,							// 0x75 u
	LOWER_CHAR,							// 0x76 v
	LOWER_CHAR,							// 0x77 w
	LOWER_CHAR,							// 0x78 x
	LOWER_CHAR,							// 0x79 y
	LOWER_CHAR,							// 0x7A z
	PUNCTUATION_CHAR,					// 0x7B {
	PUNCTUATION_CHAR,					// 0x7C |
	PUNCTUATION_CHAR,					// 0x7D }
	PUNCTUATION_CHAR,					// 0x7E ~
	CONTROL_CHAR,						// 0x7F (DEL)
	EXTENDED_CHAR,						// 0x80 
	EXTENDED_CHAR,						// 0x81 
	EXTENDED_CHAR,						// 0x82
	EXTENDED_CHAR,						// 0x83
	EXTENDED_CHAR,						// 0x84
	EXTENDED_CHAR,						// 0x85
	EXTENDED_CHAR,						// 0x86
	EXTENDED_CHAR,						// 0x87
	EXTENDED_CHAR,						// 0x88
	EXTENDED_CHAR,						// 0x89
	EXTENDED_CHAR,						// 0x8A
	EXTENDED_CHAR,						// 0x8B
	EXTENDED_CHAR,						// 0x8C
	EXTENDED_CHAR,						// 0x8D
	EXTENDED_CHAR,						// 0x8E
	EXTENDED_CHAR,						// 0x8F
	EXTENDED_CHAR,						// 0x90
	EXTENDED_CHAR,						// 0x91
	EXTENDED_CHAR,						// 0x92
	EXTENDED_CHAR,						// 0x93
	EXTENDED_CHAR,						// 0x94
	EXTENDED_CHAR,						// 0x95
	EXTENDED_CHAR,						// 0x96
	EXTENDED_CHAR,						// 0x97
	EXTENDED_CHAR,						// 0x98
	EXTENDED_CHAR,						// 0x99
	EXTENDED_CHAR,						// 0x9A
	EXTENDED_CHAR,						// 0x9B
	EXTENDED_CHAR,						// 0x9C
	EXTENDED_CHAR,						// 0x9D
	EXTENDED_CHAR,						// 0x9E
	EXTENDED_CHAR,						// 0x9F
	EXTENDED_CHAR,						// 0xA0
	EXTENDED_CHAR,						// 0xA1
	EXTENDED_CHAR,						// 0xA2
	EXTENDED_CHAR,						// 0xA3
	EXTENDED_CHAR,						// 0xA4
	EXTENDED_CHAR,						// 0xA5
	EXTENDED_CHAR,						// 0xA6
	EXTENDED_CHAR,						// 0xA7
	EXTENDED_CHAR,						// 0xA8
	EXTENDED_CHAR,						// 0xA9
	EXTENDED_CHAR,						// 0xAA
	EXTENDED_CHAR,						// 0xAA
	EXTENDED_CHAR,						// 0xAB
	EXTENDED_CHAR,						// 0xAC
	EXTENDED_CHAR,						// 0xAD
	EXTENDED_CHAR,						// 0xAE
	EXTENDED_CHAR,						// 0xAF
	EXTENDED_CHAR,						// 0xB0
	EXTENDED_CHAR,						// 0xB1
	EXTENDED_CHAR,						// 0xB2
	EXTENDED_CHAR,						// 0xB3
	EXTENDED_CHAR,						// 0xB4
	EXTENDED_CHAR,						// 0xB5
	EXTENDED_CHAR,						// 0xB6
	EXTENDED_CHAR,						// 0xB7
	EXTENDED_CHAR,						// 0xB8
	EXTENDED_CHAR,						// 0xB9
	EXTENDED_CHAR,						// 0xBA
	EXTENDED_CHAR,						// 0xBB
	EXTENDED_CHAR,						// 0xBC
	EXTENDED_CHAR,						// 0xBD
	EXTENDED_CHAR,						// 0xBE
	EXTENDED_CHAR,						// 0xBF
	EXTENDED_CHAR,						// 0xC0
	EXTENDED_CHAR,						// 0xC1
	EXTENDED_CHAR,						// 0xC2
	EXTENDED_CHAR,						// 0xC3
	EXTENDED_CHAR,						// 0xC4
	EXTENDED_CHAR,						// 0xC5
	EXTENDED_CHAR,						// 0xC6
	EXTENDED_CHAR,						// 0xC7
	EXTENDED_CHAR,						// 0xC8
	EXTENDED_CHAR,						// 0xC9
	EXTENDED_CHAR,						// 0xCA
	EXTENDED_CHAR,						// 0xCB
	EXTENDED_CHAR,						// 0xCC
	EXTENDED_CHAR,						// 0xCD
	EXTENDED_CHAR,						// 0xCE
	EXTENDED_CHAR,						// 0xCF
	EXTENDED_CHAR,						// 0xD0
	EXTENDED_CHAR,						// 0xD1
	EXTENDED_CHAR,						// 0xD2
	EXTENDED_CHAR,						// 0xD3
	EXTENDED_CHAR,						// 0xD4
	EXTENDED_CHAR,						// 0xD5
	EXTENDED_CHAR,						// 0xD6
	EXTENDED_CHAR,						// 0xD7
	EXTENDED_CHAR,						// 0xD8
	EXTENDED_CHAR,						// 0xD9
	EXTENDED_CHAR,						// 0xDA
	EXTENDED_CHAR,						// 0xDB
	EXTENDED_CHAR,						// 0xDC
	EXTENDED_CHAR,						// 0xDD
	EXTENDED_CHAR,						// 0xDE
	EXTENDED_CHAR,						// 0xDF
	EXTENDED_CHAR,						// 0xE0
	EXTENDED_CHAR,						// 0xE1
	EXTENDED_CHAR,						// 0xE2
	EXTENDED_CHAR,						// 0xE3
	EXTENDED_CHAR,						// 0xE4
	EXTENDED_CHAR,						// 0xE5
	EXTENDED_CHAR,						// 0xE6
	EXTENDED_CHAR,						// 0xE7
	EXTENDED_CHAR,						// 0xE8
	EXTENDED_CHAR,						// 0xE9
	EXTENDED_CHAR,						// 0xEA
	EXTENDED_CHAR,						// 0xEB
	EXTENDED_CHAR,						// 0xEC
	EXTENDED_CHAR,						// 0xED
	EXTENDED_CHAR,						// 0xEE
	EXTENDED_CHAR,						// 0xEF
	EXTENDED_CHAR,						// 0xF0
	EXTENDED_CHAR,						// 0xF1
	EXTENDED_CHAR,						// 0xF2
	EXTENDED_CHAR,						// 0xF3
	EXTENDED_CHAR,						// 0xF4
	EXTENDED_CHAR,						// 0xF5
	EXTENDED_CHAR,						// 0xF6
	EXTENDED_CHAR,						// 0xF7
	EXTENDED_CHAR,						// 0xF8
	EXTENDED_CHAR,						// 0xF9
	EXTENDED_CHAR,						// 0xFA
	EXTENDED_CHAR,						// 0xFB
	EXTENDED_CHAR,						// 0xFC
	EXTENDED_CHAR,						// 0xFD
	EXTENDED_CHAR,						// 0xFE
	EXTENDED_CHAR						// 0xFF
};

int isalnum(int c) {
	return characters[c] & (UPPER_CHAR | LOWER_CHAR | DIGIT_CHAR);
}

int isalpha(int c) {
	return characters[c] & (UPPER_CHAR | LOWER_CHAR);
}

int iscntrl(int c) {
	return characters[c] & (CONTROL_CHAR);
}

int isdigit(int c) {
	return characters[c] & (DIGIT_CHAR);
}

int isgraph(int c) {
	return characters[c] & (PUNCTUATION_CHAR | UPPER_CHAR | LOWER_CHAR | DIGIT_CHAR);
}

int islower(int c) {
	return characters[c] & (LOWER_CHAR);
}

int isprint(int c) {
	return characters[c] & (PUNCTUATION_CHAR | UPPER_CHAR | LOWER_CHAR | DIGIT_CHAR | WHITE_SPACE_CHAR | EXTENDED_CHAR);
}

int ispunct(int c) {
	return characters[c] & (PUNCTUATION_CHAR);
}

int isspace(int c) {
	return characters[c] & (WHITE_SPACE_CHAR);
}

int isupper(int c) {
	return characters[c] & (UPPER_CHAR);
}

int isxdigit(int c) {
	return characters[c] & (DIGIT_CHAR | HEX_DIGIT_CHAR);
}

int isascii(int c) {
	return c <= 0x7F;
}

int toascii(int c) {
	return c & 0x7F;
}

int tolower(int c) {
	return isupper(c) ? c + 'a' - 'A' : c;
}

int toupper(int c) {
	return islower(c) ? c + 'A' - 'a' : c;
}
