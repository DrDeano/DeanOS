/**
 *  \file main.h
 *  \brief A set of functions for kernel main.
 */
#ifndef MAIN_H
#define MAIN_H

/**
 *  \brief The kernel panic that will print a message of the problem and then halt the system.
 *  
 *  \param [in] format The formated string to be printed
 *  \param [in] ... The extra parameters that will be used in the formatted string.
 */
void panic(const char * format, ...);

#endif /* MAIN_H */
