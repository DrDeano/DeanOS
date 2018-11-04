/**
 * \file kernel_main.c
 * \brief The main kernel.
 */
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <stdio.h>

#include <vga.h>
#include <tty.h>
#include <boot.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <interrupt.h>
#include <pit.h>
#include <keyboard.h>
#include <panic.h>
#include <rtc.h>
#include <speaker.h>
#include <pmm.h>
#include <paging.h>
#include <floppy.h>
#include <kernel_task.h>

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

/**
 * \brief Test physical memory manager by allocating and freeing block of memory.
 */
static void pmm_test(void) {
	uint32_t * p1 = (uint32_t *) pmm_alloc_block();
	kprintf("p1 allocated at 0x%0x\n", p1);
	
	uint32_t * p2 = (uint32_t *) pmm_alloc_blocks(2);
	kprintf("Allocated 2 blocks for p2 at 0x%0x\n", p2);
	
	uint32_t * p3 = (uint32_t *) pmm_alloc_block();
	kprintf("Allocated 1 blocks for p3 at 0x%0x\n", p3);
	
	pmm_free_blocks(p2, 2);
	kprintf("Unallocated p2 to free block 2\n");
	
	uint32_t * p4 = (uint32_t *) pmm_alloc_blocks(3);
	kprintf("Allocated 3 blocks for p4 at 0x%0x\n", p4);
	
	p2 = (uint32_t *) pmm_alloc_blocks(2);
	kprintf("Re-allocated 2 blocks for p2 at 0x%0x\n", p2);
	
	uint32_t * p5 = (uint32_t *) pmm_alloc_block();
	kprintf("Allocated 1 blocks for p5 at 0x%0x\n", p5);
	
	pmm_free_blocks(p4, 3);
	kprintf("Unallocated p4 to free block 3\n");
	
	uint32_t * p6 = (uint32_t *) pmm_alloc_blocks(4);
	kprintf("Allocated 4 blocks for p6 at 0x%0x\n", p6);
	
	pmm_free_block(p1);
	p1 = (uint32_t *) pmm_alloc_block();
	kprintf("Unallocated p1 to free block 1. p1 is reallocated to 0x%0x\n", p1);
	
	pmm_free_block(p1);
	pmm_free_blocks(p2, 2);
	pmm_free_block(p3);
	pmm_free_block(p5);
	pmm_free_blocks(p6, 4);
}

/**
 * \brief Test the paging by causing a page fault.
 */
void paging_test(void) {
	// This should page fault as not mapped
	int * x = (int *) 0x4000000; // 1GB
	int y = *x;
	
	kprintf("y: %d\n", y);
}

/**
 * \brief The kernel main entry point that initiates everything.
 */
noreturn void kernel_main(void) {
	// Get the parameters from the boot loader. The cursor position
	boot_params params;
	
	char * str_type[] = {
		"Available",
		"Reserved",
		"ACPI Reclaim",
		"ACPI NVS",
		"Bad memory"
	};
	
	get_boot_params(&params);
	
	vga_init();
	
	/* Initialize terminal interface */
	tty_init(&params);
	
	if(params.sig == 0) {
		panic("Unable to get parameters from boot loader\n");
	}
	
	kprintf("Kernel size: %u bytes\n", params.kernel_size);
	
	gdt_init();
	
	idt_init();
	
	isr_init();
	
	irq_init();
	
	pit_init();
	
	keyboard_init();
	
	interrupt_enable();
	
	// Add on the first MB.
	// mem_lower is the number of KB between 1MB and 16MB.
	// mem_upper is the number of 64KB blocks above 16MB, so need to multiply by 64 to get the number of KB.
	uint32_t mem_size = 1024 + params.memory_lower + (params.memory_upper * 64);
	
	uint32_t mem_map_len = params.memory_map_length + 0;
	
	memory_map_entry_t * mem_map = (memory_map_entry_t *) params.memory_map;
	
	kprintf("%uKB (%uMB), lower: %uKB (%uMB), upper: %u 64KB blocks (%uMB)\n", mem_size, mem_size / 1024, params.memory_lower, params.memory_lower / 1024, params.memory_upper, params.memory_upper / 16);
	
	kprintf("Memory map addr: 0x%08p. Memory map length: %u\n", mem_map, mem_map_len);
	
	// Place the memory bit map after DMA buffer
	pmm_init(mem_size, (uint32_t *) 0x4000);
	
	// Initiating memory regions
	for(uint32_t i = 0; i < mem_map_len; i++) {
		// Sanity check if type is above 5
		if(mem_map[i].type > 5) {
			mem_map[i].type = 2;		// If so, mark it reserved
		}
		
		// If start address is zero when not the first memory map, then at end so break. Shouldn't need to as got the number of the memory map entries.
		if(i > 0 && mem_map[i].base_addr_lower == 0 && mem_map[i].base_addr_upper == 0) {
			break;
		}
		
		kprintf("%u: Start addr: 0x%08X%08X Len: 0x%08X%08X Type: %u-%s\n", i, mem_map[i].base_addr_upper, mem_map[i].base_addr_lower, mem_map[i].length_upper, mem_map[i].length_lower, mem_map[i].type, str_type[mem_map[i].type - 1]);
		
		// If type is 1 (available), then initiate the region so can be allocated
		if(mem_map[i].type == 1) {
			pmm_init_region(mem_map[i].base_addr_lower, mem_map[i].length_lower);
		}
	}
	
	// Uninitialise the floppy DMA buffer
	pmm_uninit_region(0x1000, 0x3000);
	
	// Uninitialise the kernel stack region
	pmm_uninit_region(0x24000, 0x7AC00);
	
	// Uninitialise the kernel memory region
	pmm_uninit_region(0x100000, params.kernel_size);
	
	kprintf("Total number of blocks: %u. Used blocks: %u. Free blocks: %u\n", pmm_get_max_blocks(), pmm_get_used_blocks(), pmm_get_free_blocks());
	
	pmm_test();
	
	paging_init();
	
	//paging_test();
	
	rtc_init();
	
	floppy_set_working_drive(0);
	
	floppy_init();
	
	kernel_task();
	
	while(1) {
		__asm__ __volatile__ ("sti");
		__asm__ __volatile__ ("hlt");
		__asm__ __volatile__ ("cli");
	}
	__builtin_unreachable();
}
