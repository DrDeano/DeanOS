/* stddef.h - C Standard Library */

/* Standard way of only allowing one definition of the header */
#ifndef _STDDEF_H
#define _STDDEF_H

/* Define the NULL construct */
#define NULL ((void *) 0)

/* Define the offsetof construct */
/* This is used to determin the byte offset of the given member field in the given type */
#define offsetof(type, member) ((unsigned int) *(((type *) 0)->member))

typedef unsigned int 	size_t;
typedef signed int		ptrdiff_t;
typedef int				wchar_t;

#endif /* _STDDEF_H */
