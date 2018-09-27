#include <string.h>
#include <stdint.h>

#include <idt.h>
#include <gdt.h>

static idt_entry_t idt_entries[IDT_ENTRIES];	/**< The IDT entry table of \ref IDT_ENTRIES entries. */
static idt_ptr_t idt_ptr;						/**< The IDT pointer that the CPU is loaded with that contains the base address of the IDT and the size. */

/**
 *  \brief Inline assembly for loading the IDT into the CPU.
 */
static inline void lidt(void) {
	__asm__ __volatile__ ("lidt [eax]" : : "a" (&idt_ptr));
}

/**
 *  \brief Create a IDT entry with values given
 *  
 *  \param [in] index     The index into the IDT
 *  \param [in] base      The base address of the interrupt to be loaded into the IDT.
 *  \param [in] selector  The selector.
 *  \param [in] type      The gate type for the interrupt.
 *  \param [in] privilege The ring level of the interrupt that can be run at.
 *  \param [in] present   Whether the interrupt entry is present.
 */
static void idt_set_entry(uint8_t index, uint32_t base, uint16_t selector, uint8_t type, uint8_t privilege, bool present) {
	// Set up base address
	idt_entries[index].base_low = base & 0xFFFF;
	idt_entries[index].base_high = (base >> 16) & 0xFFFF;
	
	// Set up selector
	idt_entries[index].selector = selector;
	
	// Set up zero, always 0
	idt_entries[index].zero = 0x00;
	
	// Set up flags
	idt_entries[index].idt_gate_type = type;
	idt_entries[index].storage_segment = false;
	idt_entries[index].privilege = privilege;
	idt_entries[index].present = present;
}

/**
 *  \brief Create the IDT pointer and load it into the CPU.
 */
static void idt_load(void) {
	// Create the pointer to the table
    idt_ptr.size = IDT_SIZE - 1;
    idt_ptr.base_addr = idt_entries;
	
    // Points the processor's internal register to the new IDT
    lidt();
}

void idt_open_interrupt_gate(uint8_t index, uint32_t base) {
    // Open an interrupt gate
    idt_set_entry(index, base, GDT_KERNEL_CODE_OFFSET, IDT_INTERRUPT_GATE, GDT_PRIVILEGE_RING_0, true);
}

void idt_close_interrupt_gate(uint8_t index) {
    // Close an interrupt gate
    idt_set_entry(index, 0, 0, 0, 0, false);
}

void idt_init(void) {
    // Clear out the entire IDT, initializing it to zeros
    memset(idt_entries, 0, IDT_SIZE);
	
	// Load the table
	idt_load();
}
