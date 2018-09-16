/**
 *  \file floppy.h
 *  \brief The floppy drive driver for read/writing to a floppy.
 */
#ifndef INCLUDE_FLOPPY_H
#define INCLUDE_FLOPPY_H

#include <stdint.h>

enum floppy_registers {
	FLOPPY_STATUS_REGISTER_A                = 0x03F0,	// Read-only
	FLOPPY_STATUS_REGISTER_B                = 0x03F1,	// Read-only
	FLOPPY_DIGITAL_OUTPUT_REGISTER          = 0x03F2,
	FLOPPY_TAPE_DRIVE_REGISTER              = 0x03F3,
	FLOPPY_MAIN_STATUS_REGISTER             = 0x03F4,	// Read-only
	FLOPPY_DATARATE_SELECT_REGISTER         = 0x03F4,	// Write-only
	FLOPPY_DATA_FIFO                        = 0x03F5,
	FLOPPY_DIGITAL_INPUT_REGISTER           = 0x03F7,	// Read-only
	FLOPPY_CONFIGURATION_CONTROL_REGISTER	= 0x03F7	// Write-only
};

/*
	Bits 0-1 DR1, DR2
		00 - Drive 0
		01 - Drive 1
		10 - Drive 2
		11 - Drive 3
	Bit 2 REST
		0 - Reset controller
		1 - Controller enabled
	Bit 3 Mode
		0 - IRQ channel
		1 - DMA mode
	Bits 4 - 7 Motor Control (Drives 0 - 3)
		0 - Stop Motor for drive
		1 - Start Motor for drive
*/

enum floppy_digital_output_register {
	FLOPPY_DOR_DRIVE_0			= 0x00,	// xxxxxx00
	FLOPPY_DOR_DRIVE_1			= 0x01,	// xxxxxx01
	FLOPPY_DOR_DRIVE_2			= 0x02,	// xxxxxx10
	FLOPPY_DOR_DRIVE_3			= 0x03,	// xxxxxx11
	FLOPPY_DOR_RESET			= 0x04,	// xxxxx1xx
	FLOPPY_DOR_DMA				= 0x08,	// xxxx1xxx
	FLOPPY_DOR_DRIVE_0_MOTOR	= 0x10,	// xxx1xxxx
	FLOPPY_DOR_DRIVE_1_MOTOR	= 0x20,	// xx1xxxxx
	FLOPPY_DOR_DRIVE_2_MOTOR	= 0x40,	// x1xxxxxx
	FLOPPY_DOR_DRIVE_3_MOTOR	= 0x80	// 1xxxxxxx
};

/*
	Bit 0 - FDD 0: 1 if FDD is busy in seek mode
	Bit 1 - FDD 1: 1 if FDD is busy in seek mode
	Bit 2 - FDD 2: 1 if FDD is busy in seek mode
	Bit 3 - FDD 3: 1 if FDD is busy in seek mode
		0: The selected FDD is not busy
		1: The selected FDD is busy
	Bit 4 - FDC Busy; Read or Write command in progress
		0: Not busy
		1: Busy
	Bit 5 - FDC in Non DMA mode
		0: FDC in DMA mode
		1: FDC not in DMA mode
	Bit 6 - DIO: direction of data transfer between the FDC IC and the CPU
		0: FDC expecting data from CPU
		1: FDC has data for CPU
	Bit 7 - RQM: Data register is ready for data transfer
		0: Data register not ready
		1: Data register ready
*/

enum floppy_main_status_register {
	FLOPPY_MSR_DRIVE_0_BUSY		= 0x01,	// xxxxxxx1
	FLOPPY_MSR_DRIVE_1_BUSY		= 0x02,	// xxxxxx1x
	FLOPPY_MSR_DRIVE_2_BUSY		= 0x04,	// xxxxx1xx
	FLOPPY_MSR_DRIVE_3_BUSY		= 0x08,	// xxxx1xxx
	FLOPPY_MSR_BUSY				= 0x10,	// xxx1xxxx
	FLOPPY_MSR_DMA_MODE			= 0x20,	// xx1xxxxx
	FLOPPY_MSR_DATA_DIRECTION	= 0x40,	// x1xxxxxx
	FLOPPY_MSR_DATA_REG_READY	= 0x80	// 1xxxxxxx
};



/*
	Bit 0 - 4 - Command
	Bit 5 - Skip Mode Setting
		0: Do not skip deleted data address marks
		1: Skip deleted data address marks
	Bit 6 - FM/MFM Mode Setting
		0: Operate in FM (Single Density) mode
		1: Operate in MFM (Double Density) mode
	Bit 7 - MultiTrack Operation
		0: Operate on one track of the cylinder
		1: Operate on both tracks of the cylinder
*/

enum floppy_comands {
	FLOPPY_CMD_READ_TRACK			= 0x02,
	FLOPPY_CMD_SPECIFY				= 0x03,
	FLOPPY_CMD_CHECK_STATUS			= 0x04,
	FLOPPY_CMD_WRITE_DATA			= 0x05,
	FLOPPY_CMD_READ_DATA			= 0x06,
	FLOPPY_CMD_RECALIBRATE			= 0x07,
	FLOPPY_CMD_CHECK_INTERRUPT		= 0x08,
	FLOPPY_CMD_WRITE_DELETED_DATA	= 0x09,
	FLOPPY_CMD_READ_ID				= 0x0A,
	FLOPPY_CMD_READ_DELETED_DATA	= 0x0C,
	FLOPPY_CMD_FORMATE_TRACK		= 0x0D,
	FLOPPY_CMD_DUMP_REG				= 0x0E,
	FLOPPY_CMD_SEEK					= 0x0F,
	FLOPPY_CMD_VERSION				= 0x10,	// Used during initialization, once
	FLOPPY_CMD_SCAN_EQUAL			= 0x11,
	FLOPPY_CMD_PERPENDICULAR_MODE	= 0x12,	// Used during initialization, once, maybe
	FLOPPY_CMD_CONFIGURE			= 0x13,	// Set controller parameters
	FLOPPY_CMD_LOCK					= 0x14,	// Protect controller parameters from a reset
	FLOPPY_CMD_VERIFY				= 0x16,
	FLOPPY_CMD_SCAN_LOW_OR_EQUAL	= 0x19,
	FLOPPY_CMD_SCAN_HIGH_OR_EQUAL	= 0x1D,

	FLOPPY_CMD_EXTENDED_SKIP		= 0x20,	// xx1xxxxx
	FLOPPY_CMD_EXTENDED_DENSITY		= 0x40,	// x1xxxxxx
	FLOPPY_CMD_EXTENDED_MULTITRACK	= 0x80	// 1xxxxxxx
};

enum floppy_gap {
	FLOPPY_GAP_LENGTH_STANDARD		= 0x2A,
	FLOPPY_GAP_LENGTH_5_25			= 0x20,
	FLOPPY_GAP_LENGTH_3_5			= 0x1B
};

enum floppy_options {
	FLOPPY_BYTES_PER_SECTOR_128		= 0x00,
	FLOPPY_BYTES_PER_SECTOR_256		= 0x01,
	FLOPPY_BYTES_PER_SECTOR_512		= 0x02,
	FLOPPY_BYTES_PER_SECTOR_1024	= 0x03,
	FLOPPY_BYTES_PER_SECTOR_2048	= 0x04,
	FLOPPY_BYTES_PER_SECTOR_4096	= 0x05,
	FLOPPY_BYTES_PER_SECTOR_8192	= 0x06,
	FLOPPY_BYTES_PER_SECTOR_16384	= 0x07,

	FLOPPY_144_SECTORS_PER_TRACK	= 0x12,	// 18
	FLOPPY_144_NUMBER_OF_HEADS		= 0x02
};

enum floppy_configuation_control_register {
	FLOPPY_CCR_500		= 0x00,
	FLOPPY_CCR_300		= 0x01,
	FLOPPY_CCR_250		= 0x02,
	FLOPPY_CCR_1000		= 0x03
};

#define FLOPPY_DMA_BUFFER				0x1000

void floppy_set_working_drive(uint8_t drive);

uint8_t floppy_get_working_drive(void);

uint8_t * floppy_read_sector(int sector_lba);

void floppy_init(void);

#endif /* INCLUDE_FLOPPY_H */
