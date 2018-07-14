/**
 *  \file idt.h
 *  \brief The Interrupt Descriptor table that contains all the interrupt handlers that is index
 *  by the CPU.
 */
#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

#include <stdint.h>
#include <stdbool.h>

/**
 * \brief The total number of table entries for the IDT.
 */
#define IDT_ENTRIES			256

/**
 * \brief The total number of bytes that the IDT takes up.
 */
#define IDT_SIZE			(sizeof(idt_entry_t) * IDT_ENTRIES)

/**
 * \brief The IDT task gate flag
 */
#define IDT_TASK_GATE		0x05

/**
 * \brief The IDT interrupt gate flag
 */
#define IDT_INTERRUPT_GATE	0x0e

/**
 * \brief The IDT trap gate flag
 */
#define IDT_TRAP_GATE		0x0f

/**
 *  \struct idt_entry_t
 *  
 *  \brief The structure that contains all the information that each IDT entry needs.
 */
typedef struct {
    uint16_t base_low : 16;		/**< The lower 16 bits of the base address of the interrupt handler offset. */
    uint16_t selector : 16;		/**< The code segment in the GDT which the handlers will be held. */
    uint8_t zero : 8;			/**< Must be zero, unused. */
	
	// Flags
	uint8_t idt_gate_type : 4;	/**< The IDT gate type. */
	bool storage_segment : 1;	/**< Must be 0 for interrupt gates. */
	uint8_t privilege : 2;		/**< The minimum ring level that the calling code must have to run the handler. So user code may not be able to run some interrupts. */
	bool present : 1;			/**< Whether the IDT entry is present. */
	//uint8_t flags : 8;
	
    uint16_t base_high : 16;	/**< The upper 16 bits of the base address of the interrupt handler offset. */
} __attribute__((__packed__)) idt_entry_t;

/**
 *  \struct idt_ptr_t
 *  
 *  \brief The IDT pointer that tells the CPU where the IDT table is and how bit it is.
 */
typedef struct {
    uint16_t size;				/**< The total size of the IDT (minus 1) in bytes. */
    idt_entry_t * base_addr;	/**< The base address where the IDT is located. */
} __attribute__((__packed__)) idt_ptr_t;

/**
 *  \brief Open a interrupt gate for a particular entry with the base address for the handler that
 *  is associated with the index.
 *  
 *  \param [in] index The index into the IDT for which the interrupt handler is to be loaded.
 *  \param [in] base The base address for the handler to be loaded at index.
 */
void idt_open_interrupt_gate(uint8_t index, uintptr_t base);

/**
 *  \brief Close a interrupt gate for a particular entry.
 *  
 *  \param [in] index The index into the IDT to close.
 */
void idt_close_interrupt_gate(uint8_t index);

/**
 *  \brief Initialise the IDT by first creating 256 blank entries and loading the location and size
 *  of the IDT into the CPU
 */
void idt_init(void);

#endif /* INCLUDE_IDT_H */
