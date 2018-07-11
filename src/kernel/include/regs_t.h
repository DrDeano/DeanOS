/**
 *  \file regs_t.h
 *  \brief The registers type that contains all registers
 */
#ifndef INCLUDE_REGS_T_H
#define INCLUDE_REGS_T_H

#include <stdint.h>

/**
 *  \struct regs_t
 *  
 *  \brief A structure that contains all the registers that are used for 
 */
typedef struct {
	// Extra segments
	uint32_t gs;			/**< An extra segment called GS */
	uint32_t fs;			/**< An extra segment called FS */
	uint32_t es;			/**< The extra segment */
	uint32_t ds;			/**< The data segment */
	
	// Destination, source, base pointer
	uint32_t edi;			/**< 32bit destination register */
	uint32_t esi;			/**< 32bit source register */
	uint32_t ebp;			/**< 32bit base pointer register */
	uint32_t esp;			/**< 32bit stack pointer register */
	
	// General registers
	uint32_t ebx;			/**< 32bit base	register */
	uint32_t edx;			/**< 32bit data register */
	uint32_t ecx;			/**< 32bit counter register */
	uint32_t eax;			/**< 32bit accumulator register */
	
	// Interrupt number and error code
	uint32_t int_num;		/**< The interrupt number that was called */
	uint32_t error_code;	/**< The error code associated with the interrupt number */
	
	// Instruction pointer, code segment and flags
	uint32_t eip;			/**< 32bit instruction pointer register */
	uint32_t cs;			/**< The code segment */
	uint32_t eflags;		/**< 32bit flags register */
	
	// Stack pointer
	uint32_t user_esp;		/**< 32bit user stack pointer register */
	
	// Stack segment
	uint32_t ss;			/**< The stack segment */
	
} __attribute__((__packed__)) regs_t;

#endif /* INCLUDE_REGS_T_H */
