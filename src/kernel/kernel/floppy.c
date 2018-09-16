#include <floppy.h>
#include <portio.h>
#include <regs_t.h>
#include <pic.h>
#include <irq.h>
#include <pit.h>

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static volatile bool floppy_irq_fired = false;

static uint8_t current_drive = 0;

/**
 *  \brief The PIT handler that is called when the PIT creates an interrupt.
 *
 *  \param [in] regs The register of the CPU when the interrupt was called
 */
static void floppy_handler(regs_t * regs) {
	(void) regs;		// Not using the registers
	floppy_irq_fired = true;
}

static void floppy_wait_irq(void) {
	while(!floppy_irq_fired) {
		__asm__ __volatile__ ("sti");
		__asm__ __volatile__ ("hlt");
		__asm__ __volatile__ ("cli");
	}
	floppy_irq_fired = false;
}

static void floppy_init_dma(void) {
	out_port_byte(0x0A, 0x06);	// Mask DMA channel 2
	out_port_byte(0x0C, 0xFF);	// Reset master flip-flop
	out_port_byte(0x04, 0x00);	// Address = 0x1000
	out_port_byte(0x04, 0x10);
	out_port_byte(0x0C, 0xFF);	// Reset master flip-flop
	out_port_byte(0x05, 0xFF);	// Count to 0x23FF (number of bytes in a 3.5" floppy disk track)
	out_port_byte(0x05, 0x23);
	out_port_byte(0x81, 0x00);	// External page register = 0
	out_port_byte(0x0A, 0x02);	// Unmask DMA channel 2
}

static void floppy_dma_read(void) {
	out_port_byte(0x0A, 0x06);	// Mask DMA channel 2
	out_port_byte(0x0B, 0x56);	// Single transfer, address increment, autoinit, read, channel 2
	out_port_byte(0x0A, 0x02);	// Unmask DMA channel 2
}

static void floppy_dma_write(void) {
	out_port_byte(0x0A, 0x06);	// Mask DMA channel 2
	out_port_byte(0x0B, 0x5A);	// Single transfer, address increment, autoinit, read, channel 2
	out_port_byte(0x0A, 0x02);	// Unmask DMA channel 2
}

static uint8_t floppy_read_status(void) {
	return in_port_byte(FLOPPY_MAIN_STATUS_REGISTER);
}

static void floppy_write_dor(uint8_t val) {
	out_port_byte(FLOPPY_DIGITAL_OUTPUT_REGISTER, val);
}

static void floppy_send_command(uint8_t cmd) {
    /**
	 * \todo Maybe change to have a time out and return error if fail
     */

    while(1) {
		if(floppy_read_status() & FLOPPY_MSR_DATA_REG_READY) {
			return out_port_byte(FLOPPY_DATA_FIFO, cmd);
		}
    }
}

static uint8_t floppy_read_data(void) {
	/**
	 * \todo Maybe change to have a time out and return error if fail
     */

    while(1) {
		if(floppy_read_status() & FLOPPY_MSR_DATA_REG_READY) {
			return in_port_byte(FLOPPY_DATA_FIFO);
		}
    }
}

static void floppy_write_ccr(uint8_t val) {
	out_port_byte(FLOPPY_CONFIGURATION_CONTROL_REGISTER, val);
}

static void floppy_check_interrupt(uint8_t * status_reg_0, uint8_t * cylinder) {
	floppy_send_command(FLOPPY_CMD_CHECK_INTERRUPT);

	*status_reg_0 = floppy_read_data();
	*cylinder = floppy_read_data();
}

static uint8_t floppy_seek(uint8_t cyl, uint8_t head) {
	uint8_t status_reg_0;
	uint8_t cylinder;

	if(current_drive > 3) {
		return -1;
	}

	for(int i = 0; i < 10; i++) {
		floppy_send_command(FLOPPY_CMD_SEEK);
		floppy_send_command(head << 2 | current_drive);
		floppy_send_command(cyl);

		floppy_wait_irq();

		floppy_check_interrupt(&status_reg_0, &cylinder);

		if(cyl == cylinder) {
			return 0;
		}
	}
	return -1;
}

static void floppy_drive_data(uint32_t step_rate, uint32_t load_time, uint32_t unload_time, bool dma) {
	floppy_send_command(FLOPPY_CMD_SPECIFY);

	floppy_send_command(((step_rate & 0x0F) << 4) | (unload_time & 0x0F));

	floppy_send_command((load_time << 1) | dma);
}

static void floppy_disable(void) {
	floppy_write_dor(0x00);
}

static void floppy_enable(void) {
	floppy_write_dor(FLOPPY_DOR_RESET | FLOPPY_DOR_DMA);
}

/**
 *  \todo Add error code
 */
static void floppy_motor(bool enable) {
	if(current_drive > 3) {
		return;
	}

	uint8_t motor = 0;

	switch(current_drive) {
		case 0:
			motor = FLOPPY_DOR_DRIVE_0_MOTOR;
			break;
		case 1:
			motor = FLOPPY_DOR_DRIVE_1_MOTOR;
			break;
		case 2:
			motor = FLOPPY_DOR_DRIVE_2_MOTOR;
			break;
		case 3:
			motor = FLOPPY_DOR_DRIVE_3_MOTOR;
			break;
	}

	if(enable) {
		floppy_write_dor(current_drive | motor | FLOPPY_DOR_RESET | FLOPPY_DOR_DMA);
	} else {
		floppy_write_dor(FLOPPY_DOR_RESET);
	}
	// Sleep for 300 millisecond
	pit_wait(300);
}

static int floppy_calibrate(uint8_t drive) {
	uint8_t status_reg_0;
	uint8_t cylinder;

	if(drive > 3) {
		return -2;
	}

    floppy_motor(true);

    for(int i = 0; i < 10; i++) {
		floppy_send_command(FLOPPY_CMD_RECALIBRATE);
		floppy_send_command(drive);

		floppy_wait_irq();

		floppy_check_interrupt(&status_reg_0, &cylinder);

		// Are we back at cylinder 0, at the beginning
		if(!cylinder) {
			floppy_motor(false);
			return 0;
		}
    }

    floppy_motor(false);
    return -1;
}

static void floppy_reset(void) {
	uint8_t status_reg_0;
	uint8_t cylinder;

	floppy_disable();
	floppy_enable();

	floppy_wait_irq();

	for(int i = 0; i < 4; i++) {
		floppy_check_interrupt(&status_reg_0, &cylinder);
	}

	floppy_write_ccr(FLOPPY_CCR_500);

	// Pass mechanical drive info. step rate=3ms, unload time=240ms, load time=16ms
	floppy_drive_data(3, 16, 240, true);

	floppy_calibrate(current_drive);
}

void floppy_set_working_drive(uint8_t drive) {
	if(drive > 3) {
		return;
	}
	current_drive = drive;
}

uint8_t floppy_get_working_drive(void) {
	return current_drive;
}

static void floppy_lba_to_chs(int lba, uint8_t * head, uint8_t * track, uint8_t * sector) {
	*head = (lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) / FLOPPY_144_SECTORS_PER_TRACK;
	*track = lba / (2 * FLOPPY_144_SECTORS_PER_TRACK);
	*sector = (lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) % FLOPPY_144_SECTORS_PER_TRACK + 1;

	//*sector = (lba % FLOPPY_144_SECTORS_PER_TRACK) + 1
	//*track = (lba / FLOPPY_144_SECTORS_PER_TRACK) / FLOPPY_144_NUMBER_OF_HEADS
	//*head = (lba / FLOPPY_144_SECTORS_PER_TRACK) % FLOPPY_144_NUMBER_OF_HEADS
}

static void floppy_read_sector_chs(uint8_t head, uint8_t track, uint8_t sector) {
	uint8_t status_reg_0;
	uint8_t cylinder;

	floppy_dma_read();

	floppy_send_command(FLOPPY_CMD_READ_DATA | FLOPPY_CMD_EXTENDED_MULTITRACK | FLOPPY_CMD_EXTENDED_SKIP | FLOPPY_CMD_EXTENDED_DENSITY);
	floppy_send_command(head << 2 | current_drive);
	floppy_send_command(track);
	floppy_send_command(head);
	floppy_send_command(sector);
	floppy_send_command(FLOPPY_BYTES_PER_SECTOR_512);
	floppy_send_command((sector + 1) >= FLOPPY_144_SECTORS_PER_TRACK ? FLOPPY_144_SECTORS_PER_TRACK : sector + 1);
	floppy_send_command(FLOPPY_GAP_LENGTH_3_5);
	floppy_send_command(0xFF);

	floppy_wait_irq();

	for(int i = 0; i < 7; i++) {
		floppy_read_data();
	}

	floppy_check_interrupt(&status_reg_0, &cylinder);
}

uint8_t * floppy_read_sector(int sector_lba) {
	if(current_drive > 3) {
		return NULL;
	}

	uint8_t head = 0;
	uint8_t track = 0;
	uint8_t sector = 0;
	floppy_lba_to_chs(sector_lba, &head, &track, &sector);

	floppy_motor(true);
	if(floppy_seek(track, head) != 0) {
		return NULL;
	}

	floppy_read_sector_chs(head, track, sector);
	floppy_motor(false);

	return (uint8_t *) FLOPPY_DMA_BUFFER;
}

void floppy_init(void) {
	irq_install_handler(PIC_IRQ_DISKETTE_DRIVE, floppy_handler);

	floppy_init_dma();

	floppy_reset();

	floppy_drive_data(13, 1, 0x0F, true);
}
