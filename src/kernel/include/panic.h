/**
 *  \file panic.h
 *  \brief The panic.
 */
#ifndef INCLUDE_PANIC_H
#define INCLUDE_PANIC_H

/**
 *  \brief The kernel panic that will print a message of the problem and then halt the system.
 *  
 *  \param [in] format The formatted string to be printed.
 *  \param [in] ... The extra parameters that will be used in the formatted string.
 */
void panic(const char * format, ...);

#endif /* INCLUDE_PANIC_H */
