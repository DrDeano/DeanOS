/**
 * \file ctype.h
 * \brief The C Library: ctype.
 */
#ifndef CTYPE_H
#define CTYPE_H 1

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \brief Check if the character is a alpha numeric character (upper or lower letter of digit).
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false. Non zero if true, else returns 0 for false.
 */
int isalnum(int c);

/**
 * \brief Check if the character is a alpha character (upper or lower letters).
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int isalpha(int c);

/**
 * \brief Check if the character is a control character.
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int iscntrl(int c);

/**
 * \brief Check if the character is a digit (0-9).
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int isdigit(int c);

/**
 * \brief Check if the character is printable but not a white space.
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int isgraph(int c);

/**
 * \brief Check if the character is a lower case letter.
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int islower(int c);

/**
 * \brief Check if the character is printable.
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int isprint(int c);

/**
 * \brief Check if the character is a punctuation character.
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int ispunct(int c);

/**
 * \brief Check if the character is a space character.
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int isspace(int c);

/**
 * \brief Check if the character is a upper case letter.
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int isupper(int c);

/**
 * \brief Check if the character is a hex digit (0-9,A-F).
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int isxdigit(int c);

/**
 * \brief Check if the character is a ASCII value. (Only lower 7 bits set).
 * 
 * \param [in] c The character to check.
 * \return Non zero if true, else returns 0 for false.
 */
int isascii(int c);

/**
 * \brief Convert the character to ASCII by making off the most significant bit.
 * 
 * \param [in] c The character to check.
 * \return Convert the integer to a acsii characture by striping off the upper bit.
 */
int toascii(int c);

/**
 * \brief Convert the character to lower case if it is upper case, else return the character.
 * 
 * \param [in] c The character to check.
 * \return The character c in uppercase else the same character is not a letter.
 */
int tolower(int c);

/**
 * \brief Convert the character to upper case if it is lower case, else return the character.
 * 
 * \param [in] c The character to check.
 * \return The character c in lowercase else the same character is not a letter.
 */
int toupper(int c);

#ifdef __cplusplus
}
#endif

#endif /* CTYPE_H */
