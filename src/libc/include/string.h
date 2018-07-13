/**
 *  \file string.h
 *  \brief The C Library: string
 */
#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Compares the first n bytes of ptr1 and ptr2.
 *  
 *  \param [in] ptr1 This is the pointer to a block of memory.
 *  \param [in] ptr2 This is the pointer to a block of memory.
 *  \param [in] n This is the number of bytes to be compared
 *  \return This function return values that are as follows -
 *			If return value < 0 then it indicates ptr1 is less than ptr2.
 *			If return value > 0 then it indicates ptr2 is less than ptr1.
 *			If return value = 0 then it indicates ptr1 is equal to ptr2.
 */
int memcmp(const void * ptr1, const void * ptr2, size_t n);

/**
 *  \brief Copies n characters from src to dest.
 *  
 *  \param [in] dest This is pointer to the destination array where the content is to be copied, type-casted to a pointer of type void *.
 *  \param [in] src  This is pointer to the source of data to be copied, type-casted to a pointer of type void *.
 *  \param [in] n This is the number of bytes to be copied.
 *  \return This function returns a pointer to destination, which is dest.
 */
void * memcpy(void * restrict dest, const void * restrict src, const size_t n);

/**
 *  \brief  A function to copy n characters from dest to src. Handle the case where dest and src overlap.
 *  
 *  \param [in] dest This is a pointer to the destination array where the content is to be copied, type-casted to a pointer of type void *.
 *  \param [in] src This is a pointer to the source of data to be copied, type-casted to a pointer of type void *.
 *  \param [in] n This is the number of bytes to be copied.
 *  \return This function returns a pointer to the destination, which is dest.
 */
void * memmove(void * dest, const void * src, const size_t n);

/**
 *  \brief Copies the character c (an unsigned char) to the first n characters of the string pointed to, by the argument src.
 *  
 *  \param [in] src This is a pointer to the block of memory to fill.
 *  \param [in] c This is the value to be set. The value is passed as an int, but the function fills the block of memory using the unsigned char conversion of this value.
 *  \param [in] n This is the number of bytes to be set to the value.
 *  \return This function returns a pointer to the memory area src.
 */
void * memset(void * restrict src, const int c, size_t n);

/**
 *  \brief Computes the length of the string src up to but not including the terminating null character.
 *  
 *  \param [in] src This is the string whose length is to be found.
 *  \return This function returns the length of string.
 */
size_t strlen(const char * restrict src);

/**
 *  \brief Compares the string pointed to, by s1 to the string pointed to by s2.
 *  
 *  \param [in] s1 This is the first string to be compared.
 *  \param [in] s2 This is the second string to be compared.
 *  \return This function return values that are as follows -
 *			If return value < 0 then it indicates s1 is less than s2.
 *			If return value > 0 then it indicates s2 is less than s1.
 *			If return value = 0 then it indicates s1 is equal to s2.
 */
int strcmp(const char * s1, const char * s2);
//char * strcpy(const char *, const char *);

#ifdef __cplusplus
}
#endif

#endif /* _STRING_H 1 */
