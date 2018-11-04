/**
 * \file array.h
 * \brief Helper functions for manipulating an array list.
 */
#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <stdbool.h>
#include <stddef.h>

bool array_list_add_str(const char * to_add, char ** array, size_t array_len);

#endif /* ARRAY_LIST_H */
