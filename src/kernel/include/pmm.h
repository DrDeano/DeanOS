/**
 *  \file pmm.h
 *  \brief The physical memory manager. Using a bit map to say whether a block (4KB) of memory is
 *  used by using a single bit.
 */
#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

/**
 *  \brief The number of blocks of memory the the bit map can 
 */
#define PMM_BLOCKS_PER_BYTE		8

/**
 *  \brief The size of each block that can be allocated. The is also the same size as a page so
 *  makes it easier to use paging.
 */
#define PMM_BLOCK_SIZE			4096

/**
 *  \brief The alignment of each block. All blocks are aligned to a 4KB boundary.
 */
#define PMM_BLOCK_ALIGNMENT		4096

uint32_t pmm_get_used_blocks(void);

uint32_t pmm_get_max_blocks(void);

uint32_t pmm_get_free_blocks(void);

void * pmm_alloc_block(void);

void * pmm_alloc_blocks(uint32_t num_blocks);

void pmm_dealloc_block(void * ptr);

void pmm_dealloc_blocks(void * ptr, uint32_t num_blocks);

void pmm_init_region(uint32_t base, uint32_t length);
	
void pmm_uninit_region(uint32_t base, uint32_t length);

void pmm_init(uint32_t mem_size, uint32_t * bit_map);

#endif /* INCLUDE_PMM_H */
