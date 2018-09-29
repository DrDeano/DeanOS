#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <panic.h>

noreturn void abort(void) {
#if defined(__is_libk)
	panic("kernel: panic: abort()\n");
#else
	// TODO: Abnormally terminate the process as if by SIGABRT.
	kprintf("abort()\n");
#endif
	while (1);
	__builtin_unreachable();
}

int atoi(const char * str) {
	if (!str) {
        return -1;
    }
	
    int result = 0;
    
    while (*str) {
        char c = *(str++);
		
        if (!isdigit(c)) {
            return -1;
        }
		
        result *= 10;
        result += (c - '0');
    }
	
	return result;
}
