/**
 *  \file paging.h
 *  \brief Functions, definitions and structures for setting up paging.
 */
#ifndef INCLUDE_PAGING_H
#define INCLUDE_PAGING_H

#include <stdint.h>
#include <stdbool.h>

/**
 *  \brief The flags used in the page table entry.
 */
enum pte_flag_masks {
	PTE_PRESENT			= 0x001,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxx1 |  */
	PTE_WRITABLE		= 0x002,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xxxxxx1x |  */
	PTE_USER_MODE		= 0x004,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xxxxx1xx |  */
	PTE_WRITE_THOUGH	= 0x008,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xxxx1xxx |  */
	PTE_NOT_CACHEABLE	= 0x010,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xxx1xxxx |  */
	PTE_ACCESSED		= 0x020,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xx1xxxxx |  */
	PTE_DIRTY			= 0x040,		/**< xxxxxxxx xxxxxxxx xxxxxxxx x1xxxxxx |  */
	PTE_PAT				= 0x080,		/**< xxxxxxxx xxxxxxxx xxxxxxxx 1xxxxxxx |  */
	PTE_CPU_GLOBAL		= 0x100,		/**< xxxxxxxx xxxxxxxx xxxxxxx1 xxxxxxxx |  */
	PTE_LEVEL_4_GLOBAL	= 0x200,		/**< xxxxxxxx xxxxxxxx xxxxxx1x xxxxxxxx |  */
	PTE_PAGE_FRAME		= 0xFFFFF000	/**< 11111111 11111111 11111xxx xxxxxxxx |  */
};

/**
 *  \brief The flags used in the page directory entry.
 */
enum pde_flag_masks {
	PDE_PRESENT			= 0x001,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxx1 |  */
	PDE_WRITABLE		= 0x002,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xxxxxx1x |  */
	PDE_USER_MODE		= 0x004,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xxxxx1xx |  */
	PDE_WRITE_THOUGH	= 0x008,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xxxx1xxx |  */
	PDE_NOT_CACHEABLE	= 0x010,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xxx1xxxx |  */
	PDE_ACCESSED		= 0x020,		/**< xxxxxxxx xxxxxxxx xxxxxxxx xx1xxxxx |  */
	PDE_DIRTY			= 0x040,		/**< xxxxxxxx xxxxxxxx xxxxxxxx x1xxxxxx |  */
	PDE_4MB				= 0x080,		/**< xxxxxxxx xxxxxxxx xxxxxxxx 1xxxxxxx |  */
	PDE_CPU_GLOBAL		= 0x100,		/**< xxxxxxxx xxxxxxxx xxxxxxx1 xxxxxxxx |  */
	PDE_LEVEL_4_GLOBAL	= 0x200,		/**< xxxxxxxx xxxxxxxx xxxxxx1x xxxxxxxx |  */
	PDE_PAGE_FRAME		= 0xFFFFF000	/**< 11111111 11111111 11111xxx xxxxxxxx |  */
};

/**
 *  \brief 
 */
#define PAGE_DIRECTORY_INDEX(x)			(((x) >> 22) & 0x3ff)

/**
 *  \brief 
 */
#define PAGE_TABLE_INDEX(x)				(((x) >> 12) & 0x3ff)

/**
 *  \brief 
 */
#define PAGE_GET_PHYSICAL_ADDRESS(x)	(*x & ~0xfff)

/**
 *  \struct pte_t
 *  
 *  \brief The structure for which the memory map is formatted by the BIOS.
 */
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

/**
 *  \struct pde_t
 *  
 *  \brief The structure for which the memory map is formatted by the BIOS.
 */
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

/**
 *  \struct page_table_t
 *  
 *  \brief The structure for which the memory map is formatted by the BIOS.
 */
typedef struct {
	pte_t pages[1024];
} __attribute__((aligned(4096))) page_table_t;

/**
 *  \struct page_directory_t
 *  
 *  \brief The structure for which the memory map is formatted by the BIOS.
 */
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
