#ifndef INCLUDE_PAGING_H
#define INCLUDE_PAGING_H

#include <stdint.h>
#include <stdbool.h>

// -----------------------
// Page table entry flags.
// -----------------------

#define PTE_PRESENT			0x001		// xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxx1

#define PTE_WRITABLE		0x002		// xxxxxxxx xxxxxxxx xxxxxxxx xxxxxx1x

#define PTE_USER_MODE		0x004		// xxxxxxxx xxxxxxxx xxxxxxxx xxxxx1xx

#define PTE_WRITE_THOUGH	0x008		// xxxxxxxx xxxxxxxx xxxxxxxx xxxx1xxx

#define PTE_NOT_CACHEABLE	0x010		// xxxxxxxx xxxxxxxx xxxxxxxx xxx1xxxx

#define PTE_ACCESSED		0x020		// xxxxxxxx xxxxxxxx xxxxxxxx xx1xxxxx

#define PTE_DIRTY			0x040		// xxxxxxxx xxxxxxxx xxxxxxxx x1xxxxxx

#define PTE_PAT				0x080		// xxxxxxxx xxxxxxxx xxxxxxxx 1xxxxxxx

#define PTE_CPU_GLOBAL		0x100		// xxxxxxxx xxxxxxxx xxxxxxx1 xxxxxxxx

#define PTE_LEVEL_4_GLOBAL	0x200		// xxxxxxxx xxxxxxxx xxxxxx1x xxxxxxxx

#define PTE_PAGE_FRAME		0xFFFFF000	// 11111111 11111111 11111xxx xxxxxxxx

// ---------------------------
// Page directory entry flags.
// ---------------------------

#define PDE_PRESENT			0x01		// xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxx1

#define PDE_WRITABLE		0x02		// xxxxxxxx xxxxxxxx xxxxxxxx xxxxxx1x

#define PDE_USER_MODE		0x04		// xxxxxxxx xxxxxxxx xxxxxxxx xxxxx1xx

#define PDE_WRITE_THOUGH	0x08		// xxxxxxxx xxxxxxxx xxxxxxxx xxxx1xxx

#define PDE_NOT_CACHEABLE	0x10		// xxxxxxxx xxxxxxxx xxxxxxxx xxx1xxxx

#define PDE_ACCESSED		0x20		// xxxxxxxx xxxxxxxx xxxxxxxx xx1xxxxx

#define PDE_DIRTY			0x40		// xxxxxxxx xxxxxxxx xxxxxxxx x1xxxxxx

#define PDE_4MB				0x80		// xxxxxxxx xxxxxxxx xxxxxxxx 1xxxxxxx

#define PDE_CPU_GLOBAL		0x100		// xxxxxxxx xxxxxxxx xxxxxxx1 xxxxxxxx

#define PDE_LEVEL_4_GLOBAL	0x200		// xxxxxxxx xxxxxxxx xxxxxx1x xxxxxxxx

#define PDE_PAGE_FRAME		0xFFFFF000	// 11111111 11111111 11111xxx xxxxxxxx

#define PAGE_DIRECTORY_INDEX(x)			(((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x)				(((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x)	(*x & ~0xfff)

typedef struct {
	bool present : 1;
	bool writeable : 1;
	bool user : 1;
	bool write_through : 1;
	bool not_cacheable : 1;
	bool accessed : 1;
	bool dirty : 1;
	uint8_t unused : 5;
	uint32_t frame : 20;
} __attribute__((__packed__)) pte_t;

typedef struct {
	bool present : 1;
	bool writeable : 1;
	bool user : 1;
	bool write_through : 1;
	bool not_cacheable : 1;
	bool accessed : 1;
	bool dirty : 1;
	bool is_4MB : 1;
	uint8_t unused : 4;
	uint32_t frame : 20;
} __attribute__((__packed__)) pde_t;

typedef struct {
	pte_t pages[1024];
} __attribute__((aligned(4096))) page_table_t;

typedef struct {
	pde_t tables[1024];
} __attribute__((aligned(4096))) page_directory_t;

// -----------------
// Page table entry.
// -----------------

pte_t * pte_add_flag(pte_t * entry, uint32_t flag);

pte_t * pte_delete_flag(pte_t * entry, uint32_t flag);

pte_t * pte_set_frame(pte_t * entry, uint32_t frame);

uint32_t pte_get_frame(pte_t entry);

bool pte_is_present(pte_t entry);

bool pte_is_writable(pte_t entry);

bool pte_is_user(pte_t entry);

bool pte_is_write_though(pte_t entry);

bool pte_is_cacheable(pte_t entry);

bool pte_is_accessed(pte_t entry);

bool pte_is_dirty(pte_t entry);

// ---------------------
// Page directory entry.
// ---------------------

pde_t * pde_add_flag(pde_t * entry, uint32_t flag);

pde_t * pde_delete_flag(pde_t * entry, uint32_t flag);

pde_t * pde_set_frame(pde_t * entry, uint32_t frame);

uint32_t pde_get_frame(pde_t entry);

bool pde_is_present(pde_t entry);

bool pde_is_writable(pde_t entry);

bool pde_is_user(pde_t entry);

bool pde_is_write_though(pde_t entry);

bool pde_is_cacheable(pde_t entry);

bool pde_is_accessed(pde_t entry);

bool pde_is_dirty(pde_t entry);

bool pde_is_4MB(pde_t entry);

// -------
// Others.
// -------

bool vmm_alloc_page(pte_t * entry);

void vmm_free_page(pte_t * entry);

pte_t * vmm_page_table_lookup_entry(page_table_t * p_table, uint32_t virtual_addr);

pde_t * vmm_page_directory_lookup_entry(page_directory_t * p_directory, uint32_t virtual_addr);

bool vmm_switch_page_directory(page_directory_t * p_directory);

page_directory_t * vmm_get_directory();

void vmm_flush_tlb_entry(uint32_t virtual_addr);

void vmm_map_page(void * physical_addr, void * virtual_addr);

void paging_init(void);

#endif /* INCLUDE_PAGING_H */
