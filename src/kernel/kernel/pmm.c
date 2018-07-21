#include <pmm.h>

#include <stdint.h>

/**
 *  \brief The total amount of physical memory that we have to work with. This is set by the BIOS
 *  memory map.
 */
static uint32_t total_memory_size;

/**
 *  \brief The is the total number of blocks that have been allocated and are being used.
 */
static uint32_t used_blocks;

/**
 *  \brief This is the total number of blocks that can be allocated.
 */
static uint32_t max_blocks;

/**
 *  \brief This is the pointer to the bit map structure for showing what blocks have been allocated
 *  and are in use.
 */
static uint32_t * memory_bit_map;

static void set_map_bit(uint32_t bit) {
	memory_bit_map[bit / 32] |= (1 << (bit % 32));
}

static void unset_map_bit(uint32_t bit) {
	memory_bit_map[bit / 32] &= ~(1 << (bit % 32));
}

static bool get_map_bit(uint32_t bit) {
	return memory_bit_map[bit / 32] & (1 << (bit % 32));
}

static bool get_first_free_block(uint32_t * frame) {
	// Loop through a set of blocks and test if the bit map is 0xFFFFFFFF
	// If not 0xFFFFFFFF, then there is a block that is free so can loop through the set of blocks
	// to find the block not in use.
	for(uint32_t i = 0; i < max_blocks / 32; i++) {
		if(memory_bit_map[i] != 0xFFFFFFFF) {
			for(uint8_t j = 0; j < 32; j++) {
				if(!get_map_bit((i * 32) + j)) {
					(*frame) = (i * 32) + j);
					return true;
				}
			}
		}
	}
	
	return false;	// No free memory
}

static bool get_first_free_blocks(uint32_t * frame, uint32_t num_blocks) {
	if(num_blocks == 0) {
		return false;
	}
	
	if(num_blocks == 1) {
		return get_first_free_block(frame);
	}
	
	for(uint32_t i = 0; i < max_blocks / 32; i++) {
		if(memory_bit_map[i] != 0xFFFFFFFF) {
			for(uint8_t j = 0; j < 32; j++) {
				if(!get_map_bit((i * 32) + j)) {
					uint32_t starting_bit = i * 32;
					starting_bit += 1 << j;
					
					uint32_t free = 0;
					
					for(int k = 0; k < num_blocks; k++) {
						if(!get_map_bit(starting_bit + k)) {
							free++;
						}
						
						if(free == num_blocks) {
							(*frame) = (i * 32) + j);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

uint32_t pmm_get_used_blocks(void) {
	return used_blocks;
}

uint32_t pmm_get_max_blocks(void) {
	return max_blocks;
}

uint32_t pmm_get_free_blocks(void) {
	return max_blocks - used_blocks;
}

void * pmm_alloc_block(void) {
	if(get_free_blocks() <= 0) {
		return NULL;		// No more memory
	}
	
	uint32_t frame;
	if(!get_first_free_block(&frame)) {
		return NULL;		// No more memory
	}
	
	set_map_bit(frame);
	used_blocks++;
	
	return (void *) frame * PMM_BLOCK_SIZE;
}

void * pmm_alloc_blocks(uint32_t num_blocks) {
	if(get_free_blocks() == 0) {
		return NULL;		// No more memory
	}
	
	uint32_t frame;
	if(!get_first_free_blocks(&frame, num_blocks)) {
		return NULL;		// No more memory
	}
	
	for(uint32_t i = 0; i < num_blocks; i++) {
		set_map_bit(frame+i);
	}
	
	used_blocks += num_blocks;
	
	return (void *) frame * PMM_BLOCK_SIZE;
}

void pmm_dealloc_block(void * ptr) {
	uint32_t frame = (uint32_t) ptr / PMM_BLOCK_SIZE;
	
	unset_map_bit(frame);
	used_blocks--;
}

void pmm_dealloc_blocks(void * ptr, uint32_t num_blocks) {
	uint32_t frame = (uint32_t) ptr / PMM_BLOCK_SIZE;
	
	for(int i = 0; i < num_blocks; i++) {
		unset_map_bit(frame + i);
	}
	
	used_blocks -= num_blocks;
}

// Length is in bytes?
void pmm_init_region(uint32_t base, uint32_t length) {
	uint32_t block_offset = base / PMM_BLOCK_SIZE;
	
	for(uint32_t num_blocks = length / PMM_BLOCK_SIZE; num_blocks > 0; num_blocks--) {
		unset_map_bit(block_offset++);
		used_blocks++;
	}
	
	// Fist block is never free so alloc can't allocate block zero as this will be used for 'out of memory'
	set_map_bit(0);
}
	
void pmm_uninit_region(uint32_t base, uint32_t length) {
	uint32_t block_offset = base / PMM_BLOCK_SIZE;
	
	for(uint32_t num_blocks = length / PMM_BLOCK_SIZE; num_blocks > 0; num_blocks--) {
		unset_map_bit(block_offset--);
		used_blocks--;
	}
}

// mem_size is in KB?
void pmm_init(uint32_t mem_size, uint32_t * bit_map) {
	memory_size = mem_size;
	memory_bit_map = bit_map;
	max_blocks = (mem_size * 1024) / PMM_BLOCK_SIZE;
	used_blocks = max_blocks;
	
	// Set all block to be used as will later set the available blocks
	memset(memory_bit_map, 0xFF, max_blocks / PMM_BLOCKS_PER_BYTE);
}