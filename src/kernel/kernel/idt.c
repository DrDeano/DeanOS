#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <idt.h>
#include <gdt.h>

static idt_entry_t idt_entries[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

inline static void _idt_load(idt_ptr_t * ptr) {
	__asm__ __volatile__ ("lidt [eax]" : : "a" (ptr));
}

static void idt_set_entry(uint8_t index, uintptr_t base, uint16_t selector, uint8_t flags /*uint8_t type, uint8_t privilege, bool present*/) {
	// Set up base address
	idt_entries[index].base_low = base & 0xFFFF;
	idt_entries[index].base_high = (base >> 16) & 0xFFFF;
	
	// Set up selector
	idt_entries[index].selector = selector;
	
	// Set up zero, always 0
	idt_entries[index].zero = 0;
	
	// Set up flags
	//idt_entries[index].idt_gate_type = type;
	//idt_entries[index].storage_segment = false;
	//idt_entries[index].privilege = privilege;
	//idt_entries[index].present = present;
	idt_entries[index].flags = flags;
}

static void idt_load() {
	// Create the pointer to the table
    idt_ptr.size = IDT_SIZE - 1;
    idt_ptr.offset = idt_entries;

    // Points the processor's internal register to the new IDT
    _idt_load(&idt_ptr);
}

void idt_open_interrupt_gate(uint8_t index, uintptr_t base) {
    // Open an interrupt gate
    idt_set_entry(index, base, GDT_KERNEL_CODE_OFFSET, 0x8E/*IDT_INTERRUPT_GATE, GDT_PRIVILEGE_KERNEL, true*/);
}

void idt_close_interrupt_gate(uint8_t index) {
    // Close an interrupt gate
    idt_set_entry(index, 0, 0, 0);
}

void idt_init() {
    // Clear out the entire IDT, initializing it to zeros
	kprintf("Initialising IDT at 0x%p\n", idt_entries);
    memset(idt_entries, 0, IDT_SIZE);
	
	// Load the table
	idt_load();
}