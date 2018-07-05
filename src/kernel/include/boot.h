/**
 *  \file boot.h
 *  \brief Functions and definitions for getting parameters from the bootloader
 */
#ifndef INCLUDE_BOOT_H
#define INCLUDE_BOOT_H

#include <stddef.h>
#include <stdint.h>

/**
 *  \def SIGNATURE
 *  
 *  \brief The signature to check in memory so that the parameters are valid
 */
#define SIGNATURE 0x00008a3c

/**
 *  \def ADDRESS
 *  
 *  \brief The address in memory that the bootloader has places the parameters
 */
#define ADDRESS ((void *) 0x00007000)

/**
 *  \struct boot_params boot.h <boot.h>
 *  
 *  \brief The boot parameter stucture that will hold the parameters saved at a specifice location (ADDRESS).
 *  Contains a signature so to check that the parameters are correct.
 *  
 */
struct boot_params {
	uint16_t sig;			/**< This is the signature that the kernel will check so to make sure is a valid parameter block */
	uint8_t  cursor_x;		/**< This is the x position of the cursor from the bootloader (column number) */
	uint8_t  cursor_y;		/**< This is the y position of the cursor from the bootloader (row number) */
} __attribute__((packed));

/**
 *  \brief Take the address given and take the data at that location and store into the boot_params
 *  struct
 *  
 *  \param [in] addr The location of the parameters provided by the bootloader
 *  \return A pointer to the a boot_params struct that holds data at the
 *  address location
 *  
 *  \details The address given will be a set address that will be the same for the ADDRESS defined
 *  and the address that the bootloader places the information for the kernel.
 */
struct boot_params * get_boot_params(void * addr);

#endif /* INCLUDE_BOOT_H */
