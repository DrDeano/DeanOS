#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

#include <stdint.h>
#include <stdbool.h>

#define IDT_ENTRIES			256
#define IDT_SIZE			(sizeof(idt_entry_t) * IDT_ENTRIES)

#define IDT_TASK_GATE		0x5
#define IDT_INTERRUPT_GATE	0xe
#define IDT_TRAP_GATE		0xf

typedef struct {
    uint16_t base_low : 16;
    uint16_t selector : 16;
    uint8_t zero : 8;
	
	// Flags
	//uint8_t idt_gate_type : 4;
	//bool storage_segment : 1;
	//uint8_t privilege : 2;
	//bool present : 1;
	uint8_t flags : 8;
	
    uint16_t base_high : 16;
} __attribute__((__packed__)) idt_entry_t;

typedef struct {
    uint16_t size;
    idt_entry_t * offset;
} __attribute__((__packed__)) idt_ptr_t;

void idt_open_interrupt_gate(uint8_t index, uintptr_t base);
void idt_close_interrupt_gate(uint8_t index);
void idt_init();

#endif /* INCLUDE_IDT_H */
