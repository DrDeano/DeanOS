/**
 * \file floppy.h
 * \brief Functions, definitions and structures for setting up the floppy drive driver for
 * read/writing to a floppy.
 */
#ifndef INCLUDE_FLOPPY_H
#define INCLUDE_FLOPPY_H

#include <stdint.h>

/**
 * \brief The physical memory location where the floppy drive in DMA mode will write to when
 * reading from the floppy.
 */
#define FLOPPY_DMA_BUFFER					0x1000

/**
 * \brief The list of floppy drive controller 0 registers to control floppy drive 0.
 */
enum floppy_registers {
	FLOPPY_STATUS_REGISTER_A				= 0x03F0,	/**< Read only. PS/2 only status register A. Monitors the state of several interface pins on the controller. */
	FLOPPY_STATUS_REGISTER_B				= 0x03F1,	/**< Read only. PS/2 only status register B. Monitors the state of several lines of the floppy drive controller. */
	FLOPPY_DIGITAL_OUTPUT_REGISTER			= 0x03F2,	/**< Write only. Control different function of the floppy drive controller. See \ref floppy_digital_output_register for list of options. */
	FLOPPY_TAPE_DRIVE_REGISTER				= 0x03F3,	/**< Allows to assign tape drive support to a specific drive. The last 3 bits are used to select the drive. 00: drive 0, 11: drive 3. */
	FLOPPY_MAIN_STATUS_REGISTER				= 0x03F4,	/**< Read only. Contains the current status information for the floppy drive controller. See \ref floppy_main_status_register for list of options. */
	FLOPPY_DATARATE_SELECT_REGISTER			= 0x03F4,	/**< Write only. PS/2 only. Change the timings of the drive control signals. */
	FLOPPY_DATA_FIFO						= 0x03F5,	/**<  */
	FLOPPY_DIGITAL_INPUT_REGISTER			= 0x03F7,	/**< Ready only.  */
	FLOPPY_CONFIGURATION_CONTROL_REGISTER	= 0x03F7	/**< Write only.  */
};

/**
 * \brief The list of digital output register options for controlling the floppy drive controller.\n
 * Bits 0-1:\n
 * 		00 - Drive 0\n
 * 		01 - Drive 1\n
 * 		10 - Drive 2\n
 * 		11 - Drive 3\n
 * Bit 2: REST:\n
 * 		0 - Reset controller\n
 * 		1 - Controller enabled\n
 * Bit 3: Mode:\n
 * 		0 - IRQ channel\n
 * 		1 - DMA mode\n
 * Bits 4 - 7: Motor Control (Drives 0 - 3):\n
 * 		0 - Stop Motor for drive\n
 * 		1 - Start Motor for drive
 */
enum floppy_digital_output_register {
	FLOPPY_DOR_DRIVE_0			= 0x00,	/**< xxxxxx00 |  */
	FLOPPY_DOR_DRIVE_1			= 0x01,	/**< xxxxxx01 |  */
	FLOPPY_DOR_DRIVE_2			= 0x02,	/**< xxxxxx10 |  */
	FLOPPY_DOR_DRIVE_3			= 0x03,	/**< xxxxxx11 |  */
	FLOPPY_DOR_RESET			= 0x04,	/**< xxxxx1xx |  */
	FLOPPY_DOR_DMA				= 0x08,	/**< xxxx1xxx |  */
	FLOPPY_DOR_DRIVE_0_MOTOR	= 0x10,	/**< xxx1xxxx |  */
	FLOPPY_DOR_DRIVE_1_MOTOR	= 0x20,	/**< xx1xxxxx |  */
	FLOPPY_DOR_DRIVE_2_MOTOR	= 0x40,	/**< x1xxxxxx |  */
	FLOPPY_DOR_DRIVE_3_MOTOR	= 0x80	/**< 1xxxxxxx |  */
};

/**
 * \brief The list of main status register options for the status of the floppy drive controller.\n
 * Bit 0 - FDD 0: 1 if FDD is busy in seek mode:\n
 * Bit 1 - FDD 1: 1 if FDD is busy in seek mode:\n
 * Bit 2 - FDD 2: 1 if FDD is busy in seek mode:\n
 * Bit 3 - FDD 3: 1 if FDD is busy in seek mode:\n
 * 		0: The selected FDD is not busy\n
 * 		1: The selected FDD is busy\n
 * Bit 4 - FDC Busy; Read or Write command in progress:\n
 * 		0: Not busy\n
 * 		1: Busy\n
 * Bit 5 - FDC in Non DMA mode:\n
 * 		0: FDC in DMA mode\n
 * 		1: FDC not in DMA mode\n
 * Bit 6 - DIO: direction of data transfer between the FDC IC and the CPU:\n
 * 		0: FDC expecting data from CPU\n
 * 		1: FDC has data for CPU\n
 * Bit 7 - RQM: Data register is ready for data transfer:\n
 * 		0: Data register not ready\n
 * 		1: Data register ready
 */
enum floppy_main_status_register {
	FLOPPY_MSR_DRIVE_0_BUSY		= 0x01,	/**< xxxxxxx1 | Set if drive 0 is busy. */
	FLOPPY_MSR_DRIVE_1_BUSY		= 0x02,	/**< xxxxxx1x | Set if drive 1 is busy. */
	FLOPPY_MSR_DRIVE_2_BUSY		= 0x04,	/**< xxxxx1xx | Set if drive 2 is busy. */
	FLOPPY_MSR_DRIVE_3_BUSY		= 0x08,	/**< xxxx1xxx | Set if drive 3 is busy. */
	FLOPPY_MSR_BUSY				= 0x10,	/**< xxx1xxxx | Set if there is a read or write happening. */
	FLOPPY_MSR_DMA_MODE			= 0x20,	/**< xx1xxxxx | Set if the controller is in DMA mode. */
	FLOPPY_MSR_DATA_DIRECTION	= 0x40,	/**< x1xxxxxx | Set if the floppy drive controller had data for the CPU else expects data from the CPU. */
	FLOPPY_MSR_DATA_REG_READY	= 0x80	/**< 1xxxxxxx | Set if the data register is ready to write to. */
};

/**
 * \brief The list of commands to control the floppy drive controller.\n
 * Bit 0 - 4 - Command:\n
 * Bit 5 - Skip Mode Setting:\n
 * 		0: Do not skip deleted data address marks\n
 * 		1: Skip deleted data address marks\n
 * Bit 6 - FM/MFM Mode Setting:\n
 * 		0: Operate in FM (Single Density) mode\n
 * 		1: Operate in MFM (Double Density) mode\n
 * Bit 7 - MultiTrack Operation:\n
 * 		0: Operate on one track of the cylinder\n
 * 		1: Operate on both tracks of the cylinder
 */
enum floppy_comands {
	FLOPPY_CMD_READ_TRACK			= 0x02,	/**< xxx00010 |  */
	FLOPPY_CMD_SPECIFY				= 0x03,	/**< xxx00011 |  */
	FLOPPY_CMD_CHECK_STATUS			= 0x04,	/**< xxx00100 |  */
	FLOPPY_CMD_WRITE_DATA			= 0x05,	/**< xxx00101 |  */
	FLOPPY_CMD_READ_DATA			= 0x06,	/**< xxx00110 |  */
	FLOPPY_CMD_RECALIBRATE			= 0x07,	/**< xxx00111 |  */
	FLOPPY_CMD_CHECK_INTERRUPT		= 0x08,	/**< xxx01000 |  */
	FLOPPY_CMD_WRITE_DELETED_DATA	= 0x09,	/**< xxx01001 |  */
	FLOPPY_CMD_READ_ID				= 0x0A,	/**< xxx01010 |  */
	FLOPPY_CMD_READ_DELETED_DATA	= 0x0C,	/**< xxx01100 |  */
	FLOPPY_CMD_FORMATE_TRACK		= 0x0D,	/**< xxx01101 |  */
	FLOPPY_CMD_DUMP_REG				= 0x0E,	/**< xxx01110 |  */
	FLOPPY_CMD_SEEK					= 0x0F,	/**< xxx01111 |  */
	FLOPPY_CMD_VERSION				= 0x10,	/**< xxx10000 |  */	// Used during initialization, once
	FLOPPY_CMD_SCAN_EQUAL			= 0x11,	/**< xxx10001 |  */
	FLOPPY_CMD_PERPENDICULAR_MODE	= 0x12,	/**< xxx10010 |  */	// Used during initialization, once, maybe
	FLOPPY_CMD_CONFIGURE			= 0x13,	/**< xxx10011 |  */	// Set controller parameters
	FLOPPY_CMD_LOCK					= 0x14,	/**< xxx10100 |  */	// Protect controller parameters from a reset
	FLOPPY_CMD_VERIFY				= 0x16,	/**< xxx10110 |  */
	FLOPPY_CMD_SCAN_LOW_OR_EQUAL	= 0x19,	/**< xxx11001 |  */
	FLOPPY_CMD_SCAN_HIGH_OR_EQUAL	= 0x1D,	/**< xxx11101 |  */
	
	FLOPPY_CMD_EXTENDED_SKIP		= 0x20,	/**< xx1xxxxx |  */
	FLOPPY_CMD_EXTENDED_DENSITY		= 0x40,	/**< x1xxxxxx |  */
	FLOPPY_CMD_EXTENDED_MULTITRACK	= 0x80	/**< 1xxxxxxx |  */
};

/**
 * \brief The gap length between sectors.
 */
enum floppy_gap {
	FLOPPY_GAP_LENGTH_STANDARD		= 0x2A,	/**<  */
	FLOPPY_GAP_LENGTH_5_25			= 0x20,	/**<  */
	FLOPPY_GAP_LENGTH_3_5			= 0x1B	/**<  */
};

/**
 * \brief The floppy options for different sector sizes.
 */
enum floppy_options {
	FLOPPY_BYTES_PER_SECTOR_128		= 0x00,	/**<  */
	FLOPPY_BYTES_PER_SECTOR_256		= 0x01,	/**<  */
	FLOPPY_BYTES_PER_SECTOR_512		= 0x02,	/**<  */
	FLOPPY_BYTES_PER_SECTOR_1024	= 0x03,	/**<  */
	FLOPPY_BYTES_PER_SECTOR_2048	= 0x04,	/**<  */
	FLOPPY_BYTES_PER_SECTOR_4096	= 0x05,	/**<  */
	FLOPPY_BYTES_PER_SECTOR_8192	= 0x06,	/**<  */
	FLOPPY_BYTES_PER_SECTOR_16384	= 0x07,	/**<  */
	
	FLOPPY_144_SECTORS_PER_TRACK	= 0x12,	/**<  */	// 18
	FLOPPY_144_NUMBER_OF_HEADS		= 0x02	/**<  */
};

/**
 * \brief Values used when writing to the floppy configuration control register.
 */
enum floppy_configuation_control_register {
	FLOPPY_CCR_500		= 0x00,	/**<  */
	FLOPPY_CCR_300		= 0x01,	/**<  */
	FLOPPY_CCR_250		= 0x02,	/**<  */
	FLOPPY_CCR_1000		= 0x03	/**<  */
};

/**
 * \brief Set the current drive.
 * 
 * \param [in] drive The drive to set.
 */
void floppy_set_working_drive(uint8_t drive);

/**
 * \brief Get the current drive.
 * 
 * \return The current drive.
 */
uint8_t floppy_get_working_drive(void);

/**
 * \brief Read a sector off the floppy drive and return the pointer to the physical memory
 * location where the data is stored. Takes the logical block address starting from 0.
 * 
 * \param [in] sector_lba The logical block address for reading a sector.
 * 
 * \return The physical memory location where the data was written to.
 */
uint8_t * floppy_read_sector(uint32_t sector_lba);

/**
 * \brief Initialise the floppy driver in DMA mode and attach the handler for the IRQ.
 */
void floppy_init(void);

#endif /* INCLUDE_FLOPPY_H */
