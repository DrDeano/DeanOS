/**
 *  \file boot.h
 *  \brief Functions and definitions for getting parameters from the bootloader.
 */
#ifndef INCLUDE_BOOT_H
#define INCLUDE_BOOT_H

#include <stddef.h>
#include <stdint.h>

/** 
 *  \brief The signature to check in memory so that the parameters are valid. Needs to match to the
 *  same value in 'bootloader.asm'.
 */
#define SIGNATURE	0x8A3C

/**
 *  \brief The address in memory that the bootloader has places the parameters.
 */
#define ADDRESS		((void *) 0x00007000)

/**
 *  \struct memory_map_entry_t
 *  
 *  \brief 
 */
typedef struct {
	uint32_t base_addr_lower;	/**<  */
	uint32_t base_addr_upper;	/**<  */
	uint32_t length_lower;		/**<  */
	uint32_t length_upper;		/**<  */
	uint32_t type;				/**<  */
	uint32_t acpi;				/**<  */
} __attribute__((__packed__)) memory_map_entry_t;

/**
 *  \struct boot_params
 *  
 *  \brief The boot parameter structure that will hold the parameters saved at a specific location
 *  (ADDRESS). Contains a signature so to check that the parameters are correct.
 */
typedef struct {
	uint16_t sig;					/**< This is the signature that the kernel will check so to make sure is a valid parameter block. */
	uint8_t  cursor_x;				/**< This is the x position of the cursor from the bootloader (column number). */
	uint8_t  cursor_y;				/**< This is the y position of the cursor from the bootloader (row number). */
	uint16_t memory_lower;			/**< The Number of KB between 1MB and 16MB. */
	uint16_t memory_upper;			/**< The number of 64KB blocks above 16MB. */
	uint32_t memory_map;			/**< The address of the memory map structure. */
	uint16_t memory_map_length;		/**< The number of memory map entries. */
	uint32_t kernel_size;			/**< The total size of the kernel loaded into memory. */
} __attribute__((__packed__)) boot_params;

/**
 *  \brief Get the boot parameters from the bootloader and add them to the structure given.
 *  
 *  \param [in] param A pointer to a boot_params structure to be filled with the parameter from the
 *  bootloader.
 *  
 *  \return A pointer to the a boot_params structure that holds data at the address location.
 */
boot_params * get_boot_params(boot_params * param);

#endif /* INCLUDE_BOOT_H */
