/**
 *  \file gdt.h
 *  \brief To set up the Global Descriptor Table
 */

#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 *  \brief The total number of GDT entries
 */
#define GDT_ENTRIES				6

/**
 *  \brief The total size of the GDT table in bytes
 */
#define GDT_SIZE				(sizeof(gdt_entry_t) * GDT_ENTRIES)

/**
 *  \brief Ring level 0.
 */
#define GDT_PRIVILEGE_RING_0	0x0

/**
 *  \brief Ring level 1.
 */
#define GDT_PRIVILEGE_RING_1	0x1

/**
 *  \brief Ring level 2.
 */
#define GDT_PRIVILEGE_RING_2	0x2

/**
 *  \brief Ring level 3.
 */
#define GDT_PRIVILEGE_RING_3	0x3

/**
 *  \brief The index of the NULL GDT entry.
 */
#define GDT_NULL_INDEX			0

/**
 *  \brief The index of the kernel code GDT entry.
 */
#define GDT_KERNEL_CODE_INDEX   1

/**
 *  \brief The index of the kernel data GDT entry.
 */
#define GDT_KERNEL_DATA_INDEX   2

/**
 *  \brief The index of the user code GDT entry.
 */
#define GDT_USER_CODE_INDEX     3

/**
 *  \brief The index of the user data GDT entry.
 */
#define GDT_USER_DATA_INDEX     4

/**
 *  \brief The index of the task state segment GDT entry.
 */
#define GDT_TSS_INDEX 			5

/**
 *  \brief The offset of the NULL GDT entry.
 */
#define GDT_NULL_OFFSET			0x00

/**
 *  \brief The offset of the kernel code GDT entry.
 */
#define GDT_KERNEL_CODE_OFFSET	0x08

/**
 *  \brief The offset of the kernel data GDT entry.
 */
#define GDT_KERNEL_DATA_OFFSET	0x10

/**
 *  \brief The offset of the user code GDT entry.
 */
#define GDT_USER_CODE_OFFSET    0x18

/**
 *  \brief The offset of the user data GDT entry.
 */
#define GDT_USER_DATA_OFFSET	0x20

/**
 *  \brief The offset of the TTS GDT entry.
 */
#define GDT_TSS_OFFSET			0x28

/**
 *  \struct gdt_entry_t
 *  
 *  \brief The structure that contains all the information that each GDT entry needs. 
 */
typedef struct {
	// Lower 16 bits of limit
	uint16_t limit_low : 16;			/**< The lower 16 bits of the limit address. Describes the size of memory that can be addressed. */
	
	// Lower 24 bits of base address
	uint32_t base_low : 24;				/**< The lower 24 bits of the base address. Describes the start of memory for the entry. */
	
	// Access bits
    bool accessed : 1;					/**< The CPU will set this when the GDT entry is accessed. */
    bool writable : 1;					/**< The writable bit to say if the memory region is writable. If set, then memory region is readable and writable. If not set, then the memory region is just readable. */
    bool direction_conforming : 1;		/**< For a code segment: if set (1), then the code segment can be executed from a lower ring level. If unset (0), then the code segment can only be executed from the same ring level in the privilege flag. For the data segment: if set (1), then the data segment grows downwards. If unset (0), then the data segment grows upwards.											 */
    bool executable : 1;				/**< The execution bit to say that the memory region is executable. */
    bool descriptor_bit : 1;			/**< The descriptor bit, always 1. */
    uint8_t privilege : 2;				/**< The ring level of the memory region. */
	bool present : 1;					/**< The present bit to tell that this GDT entry is present. */
	
	// Higher 4 bits of limit
	uint8_t limit_high : 4;				/**< The upper 4 bits of the limit address. Describes the size of memory that can be addressed. */
	
	// Flag bits
    bool reserved_zero : 1;				/**< This must always be zero. */
    bool is_64bits : 1;					/**< Whether this is a 64 bit system. */
    bool is_32bits : 1;					/**< Whether this is a 32 bit system. */
	bool is_limit_4K : 1;				/**< Whether paging is turned on, and each address is addressed as if it is a page number not physical/logical linear address. */
	
	// High 8 bits of base address
	uint8_t  base_high : 8;				/**< The upper 8 bits of the base address. Describes the start of memory for the entry. */
} __attribute__((packed)) gdt_entry_t;

/**
 *  \struct gdt_ptr_t
 *  
 *  \brief The GDT pointer structure that contains the pointer to the beginning of the GDT and the
 *  number of the table (minus 1). Used to load the GDT with LGDT instruction.
 */
typedef struct {
	uint16_t num_of_entries;	/**< 16bit entry for the number of entries (minus 1). */
	gdt_entry_t * gdt_location;	/**< 32bit entry for the base address for the GDT. */
	// uint32_t base;
	// uintptr_t * base;
} __attribute__((packed)) gdt_ptr_t;

/**
 *  \struct tss_t
 *  
 *  \brief The task state segment structure. Used for when switching tasks/processes. Stores the
 *  CPU state before switching so can be reloaded when the previous task is reloaded.
 *  \see https://wiki.osdev.org/Task_State_Segment
 */
typedef struct {
    // Link to next TSS.
    uint32_t prev_tss;	/**< A pointer to the previous TSS entry */

    // Ring stacks.
    uint32_t ESP0;		/**< Ring 0 32 bit stack pointer. */
    uint32_t SS0;		/**< Ring 0 32 bit stack segment. */
    uint32_t ESP1;		/**< Ring 1 32 bit stack pointer. */
    uint32_t SS1;		/**< Ring 1 32 bit stack segment. */
    uint32_t ESP2;		/**< Ring 2 32 bit stack pointer. */
    uint32_t SS2;		/**< Ring 2 32 bit stack segment. */

    // Registers.
    uint32_t CR3;		/**< The CR3 control register 3. */
    uint32_t EIP;		/**< 32 bit instruction pointer. */
    uint32_t EFLAGS;	/**< 32 bit flags register. */
    uint32_t EAX;		/**< 32 bit accumulator register. */
    uint32_t ECX;		/**< 32 bit counter register. */
    uint32_t EDX;		/**< 32 bit data register. */
    uint32_t EBX;		/**< 32 bit base register. */
    uint32_t ESP;		/**< 32 bit stack pointer register. */
    uint32_t EBP;		/**< 32 bit base pointer register. */
    uint32_t ESI;		/**< 32 bit source register. */
    uint32_t EDI;		/**< 32 bit destination register. */

    // Segments.
    uint32_t ES;		/**< The extra segment. */
    uint32_t CS;		/**< The code segment. */
    uint32_t SS;		/**< The stack segment. */
    uint32_t DS;		/**< The data segment. */
    uint32_t FS;		/**< A extra segment FS. */
    uint32_t GS;		/**< A extra segment GS. */
    
    uint32_t LDT;		/**< The local descriptor table register. */
    uint16_t TRAP;		/**< ? */
    uint16_t IO_Permissions_Base_Offset;	/**< A pointer to a I/O port bitmap for the current task which specifies individual ports the program should have access to. */
} __attribute__((packed)) tss_t;

/**
 *  \brief Set up the GDT table with 6 entries. The NULL, kernel code, kernel data, user code, user
 *  data, and TSS entries.
 */
void gdt_init(void);

#endif /* INCLUDE_GDT_H */
