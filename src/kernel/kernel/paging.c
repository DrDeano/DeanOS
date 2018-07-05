#include <paging.h>
#include <isr.h>
#include <regs_t.h>
#include <main.h>

#include <stdint.h>
#include <stdio.h>

uint32_t page_directory[1024] __attribute__((aligned(4096)));

uint32_t page_table_1[1024] __attribute__((aligned(4096)));

static inline void set_cr3() {
	__asm__ __volatile__ ("mov	cr3, eax" : : "a" (page_directory));
}

static inline void enable_paging() {
	__asm__ __volatile__ ("mov	eax, cr0");
	__asm__ __volatile__ ("or	eax, 0x80000000");
	__asm__ __volatile__ ("mov	cr0, eax");
}

void page_fault_handler(regs_t * regs) {
	uintptr_t addr;
    __asm__ __volatile__ ("mov %0, cr2" : "=r"(addr));
	
    kprintf("EAX: 0x%p, EBX: 0x%p, ECX: 0x%p, EDX: 0x%p\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
    kprintf("ESI: 0x%p, EDI: 0x%p, EBP: 0x%p, ESP: 0x%p\n", regs->esi, regs->edi, regs->ebp, regs->esp);
    kprintf("EIP: 0x%p, EFLAGS: 0x%p\n", regs->eip, regs->eflags);
	panic("PAGING: Page fault at 0x%p (0x%X)!\n", addr, regs->error_code);
}

void paging_init() {
	uint32_t address = 0;
	
	isr_install_handler(EXCEPTION_PAGE_FAULT, page_fault_handler);
	
	for(int i = 0; i < 1024; i++) {
		// This sets the following flags to the pages:
		//   Supervisor: Only kernel-mode can access them
		//   Write Enabled: It can be both read from and written to
		//   Not Present: The page table is not present
		page_directory[i] = 0 | 0x00000002;
		
		// As the address is page aligned, it will always leave 12 bits zeroed.
		// Those bits are used by the attributes ;)
		page_table_1[i] = address | 3; // attributes: supervisor level, read/write, present.
		address += 4096;
	}
	
	page_directory[0] = ((uint32_t) page_table_1) | 3;
	
	set_cr3();
	
	enable_paging();
}
