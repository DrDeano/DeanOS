#include <paging.h>
#include <isr.h>
#include <regs_t.h>
#include <panic.h>
#include <pmm.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static page_directory_t * current_dir = 0;			/**<  */
static uint32_t current_page_dir_base_register = 0;	/**< Current page directory base register */

static void set_cr3(uint32_t addr) {
	__asm__ __volatile__ ("mov	cr3, eax" : : "a" (addr));
}

static void enable_paging() {
	__asm__ __volatile__ ("mov	eax, cr0");
	__asm__ __volatile__ ("or	eax, 0x80000000");
	__asm__ __volatile__ ("mov	cr0, eax");
}

void page_fault_handler(regs_t * regs) {
	uint32_t addr;
    __asm__ __volatile__ ("mov %0, cr2" : "=r"(addr));
	
    kprintf("EAX: 0x%p, EBX: 0x%p, ECX: 0x%p, EDX: 0x%p\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
    kprintf("ESI: 0x%p, EDI: 0x%p, EBP: 0x%p, ESP: 0x%p\n", regs->esi, regs->edi, regs->ebp, regs->esp);
    kprintf("EIP: 0x%p, EFLAGS: 0x%p\n", regs->eip, regs->eflags);
	panic("PAGING: Page fault at 0x%p (0x%X)!\n", addr, regs->error_code);
}

pte_t * pte_add_flag(pte_t * entry, uint32_t flag) {
	uint32_t * e = (uint32_t *) entry;
	*e |= flag;
	return entry;
}

pte_t * pte_delete_flag(pte_t * entry, uint32_t flag) {
	uint32_t * e = (uint32_t *) entry;
	*e &= ~flag;
	return entry;
}

pte_t * pte_set_frame(pte_t * entry, uint32_t frame) {
	uint32_t * e = (uint32_t *) entry;
	*e = (*e & ~PTE_PAGE_FRAME) | frame;
	return entry;
}

uint32_t pte_get_frame(pte_t entry) {
	return (uint32_t) entry.frame;
}

bool pte_is_present(pte_t entry) {
	return entry.present;
}

bool pte_is_writable(pte_t entry) {
	return entry.writeable;
}

bool pte_is_user(pte_t entry) {
	return entry.user;
}

bool pte_is_write_though(pte_t entry) {
	return entry.write_through;
}

bool pte_is_cacheable(pte_t entry) {
	return entry.not_cacheable;
}

bool pte_is_accessed(pte_t entry) {
	return entry.accessed;
}

bool pte_is_dirty(pte_t entry) {
	return entry.dirty;
}

pde_t * pde_add_flag(pde_t * entry, uint32_t flag) {
	uint32_t * e = (uint32_t *) entry;
	*e |= flag;
	return entry;
}

pde_t * pde_delete_flag(pde_t * entry, uint32_t flag) {
	uint32_t * e = (uint32_t *) entry;
	*e &= ~flag;
	return entry;
}

pde_t * pde_set_frame(pde_t * entry, uint32_t frame) {
	uint32_t * e = (uint32_t *) entry;
	*e = (*e & ~PDE_PAGE_FRAME) | frame;
	return entry;
}

uint32_t pde_get_frame(pde_t entry) {
	return (uint32_t) entry.frame;
}

bool pde_is_present(pde_t entry) {
	return entry.present;
}

bool pde_is_writable(pde_t entry) {
	return entry.writeable;
}

bool pde_is_user(pde_t entry) {
	return entry.user;
}

bool pde_is_write_though(pde_t entry) {
	return entry.write_through;
}

bool pde_is_cacheable(pde_t entry) {
	return entry.not_cacheable;
}

bool pde_is_accessed(pde_t entry) {
	return entry.accessed;
}

bool pde_is_dirty(pde_t entry) {
	return entry.dirty;
}

bool pde_is_4MB(pde_t entry) {
	return entry.is_4MB;
}

bool vmm_alloc_page(pte_t * entry) {
	void * ptr = pmm_alloc_block();
	if(!ptr) {
		return false;
	}
	
	pte_set_frame(entry, (uint32_t) ptr);
	pte_add_flag(entry, PTE_PRESENT);
	
	return true;
}

void vmm_free_page(pte_t * entry) {
	void * ptr = (void *) pte_get_frame(*entry);
	if(ptr) {
		pmm_free_block(ptr);
	}
	
	pte_delete_flag(entry, PTE_PRESENT);
}

pte_t * vmm_page_table_lookup_entry(page_table_t * p_table, uint32_t virtual_addr) {
	if(p_table) {
		return &p_table->pages[PAGE_TABLE_INDEX(virtual_addr)];
	}
	
	return NULL;
}

pde_t * vmm_page_directory_lookup_entry(page_directory_t * p_directory, uint32_t virtual_addr) {
	if(p_directory) {
		return &p_directory->tables[PAGE_DIRECTORY_INDEX(virtual_addr)];
	}
	
	return NULL;
}

bool vmm_switch_page_directory(page_directory_t * p_directory) {
	if(!p_directory) {
		return false;
	}
	
	current_dir = p_directory;
	set_cr3((uint32_t) current_dir);
	return true;
}

page_directory_t * vmm_get_directory() {
	return current_dir;
}

void vmm_flush_tlb_entry(uint32_t virtual_addr) {
	__asm__ __volatile__ ("cli");
	__asm__ __volatile__ ("invlpg	%0" : : "m" (virtual_addr) : "memory");
	__asm__ __volatile__ ("sti");
}

void vmm_map_page(void * physical_addr, void * virtual_addr) {
	page_directory_t * p_dir = current_dir;
	
	pde_t * entry = &p_dir->tables[PAGE_DIRECTORY_INDEX((uint32_t) virtual_addr)];
	
	if(!pde_is_present(*entry)) {
		page_table_t * table = (page_table_t *) pmm_alloc_block();
		if(!table) {
			return;
		}
		
		memset(table, 0, sizeof(page_table_t));
		
		pde_t * entry = &p_dir->tables[PAGE_DIRECTORY_INDEX((uint32_t) virtual_addr)];
		//pde_add_flag(entry, PDE_PRESENT);
		//pde_add_flag(entry, PDE_WRITEABLE);
		pde_add_flag(entry, PDE_PRESENT | PDE_WRITEABLE);				// This is the same as the above two line to is faster
		pde_set_frame(entry, (uint32_t) table);
	}
	
	uint32_t * e = (uint32_t *) entry;
	page_table_t * table = (page_table_t *) PAGE_GET_PHYSICAL_ADDRESS(e);
	
	pte_t * page = &table->pages[PAGE_TABLE_INDEX((uint32_t) virtual_addr)];
	
	pte_set_frame(page, (uint32_t) physical_addr);
	//pte_add_flag(page, PTE_PRESENT);
	//pte_add_flag(page, PTE_WRITEABLE);
	pte_add_flag(page, PTE_PRESENT | PTE_WRITEABLE); // This is faster
}

void paging_init(void) {
	isr_install_handler(EXCEPTION_PAGE_FAULT, page_fault_handler);
	
	page_table_t * table = (page_table_t *) pmm_alloc_block();
	
	if(!table) {
		return;
	}
	
	page_table_t * table_2 = (page_table_t *) pmm_alloc_block();
	
	if(!table_2) {
		return;
	}
	
	memset(table, 0, sizeof(page_table_t));
	
	for(int i = 0, frame = 0x0, virt_addr = 0x0; i < 1024; i++, frame += 4096, virt_addr += 4096) {;
		pte_t page;
		memset(&page, 0, sizeof(pte_t));
		pte_add_flag(&page, PTE_PRESENT);
		pte_set_frame(&page, frame);
		
		table_2->pages[PAGE_TABLE_INDEX(virt_addr)] = page;
	}
	
	for(int i = 0, frame = 0x100000, virt_addr = 0xC0000000; i < 1024; i++, frame += 4096, virt_addr += 4096) {;
		pte_t page;
		memset(&page, 0, sizeof(pte_t));
		pte_add_flag(&page, PTE_PRESENT);
		pte_set_frame(&page, frame);
		
		table->pages[PAGE_TABLE_INDEX(virt_addr)] = page;
	}
	
	page_directory_t * dir = (page_directory_t *) pmm_alloc_blocks(3);
	if(!dir) {
		return;
	}
	
	memset(dir, 0, sizeof(page_directory_t));
	
	pde_t * entry = &dir->tables[PAGE_DIRECTORY_INDEX(0xC0000000)];
	//pde_add_flag(entry, PDE_PRESENT);
	//pde_add_flag(entry, PDE_WRITEABLE);
	pde_add_flag(entry, PDE_PRESENT | PDE_WRITEABLE); // Faster
	pde_set_frame(entry, (uint32_t) table);
	
	pde_t * entry_2 = &dir->tables[PAGE_DIRECTORY_INDEX(0x0)];
	//pde_add_flag(entry_2, PDE_PRESENT);
	//pde_add_flag(entry_2, PDE_WRITEABLE);
	pde_add_flag(entry_2, PDE_PRESENT | PDE_WRITEABLE); // Faster
	pde_set_frame(entry_2, (uint32_t) table_2);
	
	current_page_dir_base_register = (uint32_t) &dir->tables;
	
	vmm_switch_page_directory(dir);
	
	enable_paging();
}
