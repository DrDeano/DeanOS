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
 *  \brief Brief description
 *  
 *  \param [in] size_t Description for size_t
 *  \return Return description
 *  
 *  \details More details
 */
int memcmp(const void *, const void *, size_t);

/**
 *  \brief Brief description
 *  
 *  \param [in] restrict Description for restrict
 *  \param [in] restrict Description for restrict
 *  \param [in] size_t Description for size_t
 *  \return Return description
 *  
 *  \details More details
 */
void * memcpy(void * restrict, const void * restrict, size_t);

/**
 *  \brief Brief description
 *  
 *  \param [in] size_t Description for size_t
 *  \return Return description
 *  
 *  \details More details
 */
void * memmove(void *, const void *, size_t);

/**
 *  \brief Brief description
 *  
 *  \param [in] restrict Description for restrict
 *  \param [in] int Description for int
 *  \param [in] size_t Description for size_t
 *  \return Return description
 *  
 *  \details More details
 */
void * memset(void * restrict, int, size_t);

/**
 *  \brief Brief description
 *  
 *  \param [in] restrict Description for restrict
 *  \return Return description
 *  
 *  \details More details
 */
size_t strlen(const char * restrict);

/**
 *  \brief Brief description
 *  
 *  \return Return description
 *  
 *  \details More details
 */
int strcmp(const char *, const char *);
//char * strcpy(const char *, const char *);

#ifdef __cplusplus
}
#endif

#endif /* _STRING_H 1 */
