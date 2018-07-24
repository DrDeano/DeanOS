/**
 *  \file pmm.h
 *  \brief The physical memory manager. Using a bit map to say whether a block (4KB) of memory is
 *  used by using a single bit.
 */
#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include <stdint.h>

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

void pmm_free_block(void * ptr);

void pmm_free_blocks(void * ptr, uint32_t num_blocks);

void pmm_init_region(uint32_t base, uint32_t length);
	
void pmm_uninit_region(uint32_t base, uint32_t length);

/**
 *  \brief Initiate the physical memory manager. Given the size of available memory in kilobytes so
 *  the pmm knows how much memory to work with. Give the location for the memory bit map for saying
 *  what memory block (4KB) is free or not. Uninitiates all blocks to later be initialised from the
 *  BIOS memory map.
 *  
 *  \param [in] mem_size The size of available memory in kilobytes.
 *  \param [in] bit_map The location for the bit map.
 *  
 *  \todo Added upper address for init regions.
 */
void pmm_init(uint32_t mem_size, uint32_t * bit_map);

#endif /* INCLUDE_PMM_H */
