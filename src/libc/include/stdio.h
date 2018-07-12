/**
 *  \file stdio.h
 *  \brief The C Library: stdio
 */
#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdarg.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int kputchar(int c);
int kprint_string(char * str, size_t width);
int kprintf(const char * format, ...);
int kprintf_va(const char * format, va_list args);
int kputs(const char * str);

#ifdef __cplusplus
}
#endif
 
#endif /* _STDIO_H */
