/**
 *  \file pmm.h
 *  \brief Functions, definitions and structures for setting up the physical memory manager. Using
 *  a bit map to say whether a block (4KB) of memory is used by using a single bit.
 */
#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include <stdint.h>

/**
 *  \brief The number of blocks of memory the the bitmap can represent to be allocate or unallocated.
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

/** 
 *  \brief Get the number of used/allocated physical blocks. A block is 4KB in size.
 *  
 *  \return The number of used/allocated physical blocks.
 */
uint32_t pmm_get_used_blocks(void);

/** 
 *  \brief Get the maximum number of physical blocks that can be allocated. The total amount of
 *  memory in the system (in bytes) divided by 4KB.
 *  
 *  \return The maximum number of physical blocks.
 */
uint32_t pmm_get_max_blocks(void);

/** 
 *  \brief Get the number of unallocated blocks that can be allocated.
 *  
 *  \return The number of unallocated blocks.
 */
uint32_t pmm_get_free_blocks(void);

/** 
 *  \brief Allocate a physical block of memory, 4KB in size. This gets the next available block. If
 *  no available block can be allocated, then returns NULL.
 *  
 *  \return A pointer to a physical memory locations that is allocated.
 */
void * pmm_alloc_block(void);

/** 
 *  \brief Allocates a continues physical block of memory, 4KB * \p num_blocks in size. This gets
 *  the next continues available blocks. If no available continues blocks can be allocated, then
 *  returns NULL.
 *  
 *  \param [in] num_blocks The number of continues blocks to allocate.
 *  
 *  \return A pointer to a physical memory locations at the beginning of the allocated memory.
 */
void * pmm_alloc_blocks(uint32_t num_blocks);

/** 
 *  \brief Free a physical block of memory. Given a pointer that was allocated by \ref pmm_alloc_block.
 *  
 *  \param [in] ptr The pointer to free.
 */
void pmm_free_block(void * ptr);

/** 
 *  \brief Free a continues physical blocks of memory. Given a pointer that was allocated by \ref pmm_alloc_blocks.
 *  
 *  \param [in] ptr        The pointer to the continues memory to be free'd.
 *  \param [in] num_blocks The number of blocks that was allocated by \ref pmm_alloc_blocks.
 */
void pmm_free_blocks(void * ptr, uint32_t num_blocks);

/** 
 *  \brief Initiate a region on memory starting at \p base with length \p length that can be
 *  allocated. Won't initiate the zero'th block as this is used for the NULL block. Also won't
 *  initiate the memory that the memory bitmap is stored at.
 *  
 *  \param [in] base   The base/start address where the memory will be initiated.
 *  \param [in] length The length, in bytes, that will be initiates.
 */
void pmm_init_region(uint32_t base, uint32_t length);

/** 
 *  \brief Uninitiate a region on memory starting at \p base with length \p length that can't be
 *  allocated. This will be used to the stack, kernel code... can't be allocated.
 *  
 *  \param [in] base   The base/start address where the memory will be uninitiated.
 *  \param [in] length The length, in bytes, that will be uninitiates.
 */
void pmm_uninit_region(uint32_t base, uint32_t length);

/**
 *  \brief Initiate the physical memory manager. Given the size of available memory in kilobytes so
 *  the PMM knows how much memory to work with. Give the location for the memory bit map for saying
 *  what memory block (4KB) is free or not. Uninitiate all blocks to later be initialised from the
 *  BIOS memory map.
 *  
 *  \param [in] mem_size The size of available memory in kilobytes.
 *  \param [in] bit_map  The location for the bit map.
 *  
 *  \todo Added upper address for init regions.
 */
void pmm_init(uint32_t mem_size, uint32_t * bit_map);

#endif /* INCLUDE_PMM_H */
