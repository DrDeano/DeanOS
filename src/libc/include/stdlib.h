/**
 *  \file stdlib.h
 *  \brief The C library: stdlib
 */
#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stdnoreturn.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Causes an abnormal program termination.
 */
noreturn void abort(void);

/**
 *  \brief Convert a string to an integer.
 *  
 *  \param [in] str The string to convert.
 *  
 *  \return The integer.
 */
int atoi(const char * str);

#ifdef __cplusplus
}
#endif

#endif /* _STDLIB_H */
