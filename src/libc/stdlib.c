#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <main.h>

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
