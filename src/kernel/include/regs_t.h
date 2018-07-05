#ifndef INCLUDE_REGS_T_H
#define INCLUDE_REGS_T_H

#include <stdint.h>

typedef struct {
	// Extra segments
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	
	// Destination, source, base pointer
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	
	// General registers
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	
	// Interrupt number and error code
	uint32_t int_num;
	uint32_t error_code;
	
	// Instruction pointer, code segment and flags
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	
	// Stack pointer
	uint32_t user_esp;
	
	// Stack segment
	uint32_t ss;
	
} __attribute__((__packed__)) regs_t;

#endif /* INCLUDE_REGS_T_H */
