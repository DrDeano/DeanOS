/**
 *  \file string.h
 *  \brief C Library string
 */
#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void *, const void *, size_t);
void * memcpy(void * restrict, const void * restrict, size_t);
void * memmove(void *, const void *, size_t);
void * memset(void * restrict, int, size_t);
size_t strlen(const char * restrict);
int strcmp(const char *, const char *);
//char * strcpy(const char *, const char *);

#ifdef __cplusplus
}
#endif

#endif /* _STRING_H 1 */
