/**
 *  \file gdt.h
 *  \brief To set up the global descriptor table
 */

#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define GDT_ENTRIES 6

#define GDT_SIZE	(sizeof(gdt_entry_t) * GDT_ENTRIES)

#define GDT_PRIVILEGE_KERNEL	0x0
#define GDT_PRIVILEGE_USER		0x3

#define GDT_NULL_INDEX			0

// Kernel mode code and data segments
#define GDT_KERNEL_CODE_INDEX   1
#define GDT_KERNEL_DATA_INDEX   2

// User mode code and data segments 
#define GDT_USER_CODE_INDEX     3
#define GDT_USER_DATA_INDEX     4

// Task state segment
#define GDT_TSS_INDEX 			5

#define GDT_NULL_OFFSET			0x00

#define GDT_KERNEL_CODE_OFFSET	0x08
#define GDT_KERNEL_DATA_OFFSET	0x10

#define GDT_USER_CODE_OFFSET    0x18
#define GDT_USER_DATA_OFFSET	0X20

#define GDT_TSS_OFFSET			0x28

typedef struct {
	// Lower 16 bits of limit
	uint16_t limit_low : 16;
	
	// Lower 24 bits of base address
	uint32_t base_low : 24;
	
	// Access bits
    bool accessed : 1;
    bool writeable : 1;
    bool direction_conforming : 1;
    bool executable : 1;
    bool descriptor_bit : 1;
    uint8_t privilege : 2;
	bool present : 1;
	
	// Higher 4 bits of limit
	uint8_t limit_high : 4;
	
	// Flag bits
    bool reserved_zero : 1;
    bool is_64bits : 1;
    bool is_32bits : 1;
	bool is_limit_4K : 1;
	
	// High 8 bits of base address
	uint8_t  base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
	uint16_t num_of_entries;
	gdt_entry_t * gdt_location;
	// uint32_t base;
	// uintptr_t * base;
} __attribute__((packed)) gdt_ptr_t;

/**
 *  The task state segment struct: https://wiki.osdev.org/Task_State_Segment
 */
typedef struct {
    // Link to next TSS.
    uint32_t prev_tss;

    // Ring stacks.
    uint32_t ESP0;
    uint32_t SS0;
    uint32_t ESP1;
    uint32_t SS1;
    uint32_t ESP2;
    uint32_t SS2;

    // Registers.
    uint32_t CR3;
    uint32_t EIP;
    uint32_t EFLAGS;
    uint32_t EAX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t EBX;
    uint32_t ESP;
    uint32_t EBP;
    uint32_t ESI;
    uint32_t EDI;

    // Segments.
    uint32_t ES;
    uint32_t CS;
    uint32_t SS;
    uint32_t DS;
    uint32_t FS;
    uint32_t GS;
    
    uint32_t LDT;
    uint16_t TRAP;
    uint16_t IO_Permissions_Base_Offset;
} __attribute__((packed)) tss_t;

void gdt_init();

#endif /* INCLUDE_GDT_H */
