/**
 *  \file pic.h
 *
 *  \brief Functions, definitions and structures for setting up the Programmable Interrupt Controller.
 *  
 *  \todo For remapping, maybe need an io_wait between each PIC sending, see https://wiki.osdev.org/PIC.
 */
#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H

#include <stdint.h>
#include <portio.h>

/**
 *  \brief The port address for reading and writing to registers in the PIT.
 */
enum pic_port_address {
	PIC_COMMAND_REG_MASTER				= 0x20,	/**< The port address for the master PIC command register. (Write only). */
	PIC_STATUS_REG_MASTER				= 0x20,	/**< The port address for the master PIC status register. (Read only). */
	PIC_DATA_REG_MASTER					= 0x21,	/**< The port address for the master PIC data register. */
	PIC_INTERRUPT_MASK_REG_MASTER		= 0x21,	/**< The port address for the master PIC interrupt mask register. */
	PIC_COMMAND_REG_SLAVE				= 0xA0,	/**< The port address for the slave PIC command register. (Write only). */
	PIC_STATUS_REG_SLAVE				= 0xA0,	/**< The port address for the slave PIC status register. (Read only). */
	PIC_DATA_REG_SLAVE					= 0xA1,	/**< The port address for the slave PIC data register. */
	PIC_INTERRUPT_MASK_REG_SLAVE		= 0xA1	/**< The port address for the slave PIC interrupt mask register. */
};

/**
 *  \brief The initialisation control word 1. Primary control word for initialising the PIC.
 */
enum pit_icw1 {
	PIC_ICW1_ICW4						= 0x01,	/**< xxxxxxx1 | If set, then the PIC expects to receive a initialisation control word 4. */
	PIC_ICW1_SINGLE						= 0x02,	/**< xxxxxx1x | If set, then there is only one PIC in the system. If not set, then PIC is cascaded with slave PIC's and initialisation control word 3 must be sent to the controller. */
	PIC_ICW1_INTERVAL_4					= 0x04,	/**< xxxxx1xx | If set, then the internal CALL address is 4. If not set, then is 8. Usually ignored by x86. So default is not set, 0. */
	PIC_ICW1_LEVEL_TRIGGER				= 0x08,	/**< xxxx1xxx | If set, then operating in level triggered mode. If not set, then operating in edge triggered mode. */
	PIC_ICW1_INITIALISATION				= 0x10	/**< xxx1xxxx | If set, then the PIC is to be initialised. */
	// Upper 3 bits must be 0 for x86
};

/**
 *  \brief The initialisation control word 2. Map the base address of the interrupt vector table.
 */
enum pit_icw2 {
	PIC_MASTER_REMAP_OFFSET				= 0x20,	/**< The new port map for the master PIC. */
	PIC_SLAVE_REMAP_OFFSET				= 0x28	/**< The new port map for the slave PIC. */
};

/**
 *  \brief The initialisation control word 3. For Telling the master and slave where the cascading
 *  interrupts are coming from.
 */
enum pit_icw3 {
	PIC_ICW3_SLAVE_IRQ_MAP_TO_MASTER	= 0x02,	/**< xxxxxx1x | Tell the slave PIT to send interrupts to the master PIC on IRQ2, which is binary 2 (010). */
	PIC_ICW3_MASTER_IRQ_MAP_TO_SLAVE	= 0x04	/**< xxxxx1xx | Tell the master PIT to receive interrupts from the slave PIC on IRQ2, which is the third bit. */
};

/**
 *  \brief The initialisation control word 4. Tell the master and slave what mode to operate in.
 */
enum pit_icw4 {
	PIC_ICW4_80x86_MODE					= 0x01,	/**< xxxxxxx1 | If set, then in 80x86 mode. If not set, then in MCS-80/86 mode */
	PIC_ICW4_AUTO_EOI					= 0x02,	/**< xxxxxx1x | If set, then on last interrupt acknowledge pulse the PIC automatically performs end of interrupt operation. */
	PIC_ICW4_BUFFER_SELECT				= 0x04,	/**< xxxxx1xx | Only use if \ref PIC_ICW4_BUFFER_MODE is set. If set, then selects master's buffer. If not set then uses slave's buffer. */
	PIC_ICW4_BUFFER_MODE				= 0x08,	/**< xxxx1xxx | If set, then PIC operates in buffered mode. */
	PIC_ICW4_SPECIAL_FULLY_NESTED_MODE	= 0x10,	/**< xxx1xxxx | If set, then the the system had many cascaded PIC's. Not supported in x86. */
	// The upper 3 bits must be 0
};

/**
 *  \brief Operation control word 1. Interrupt mask register.
 */
enum pit_ocw1 {
	PIC_OCW1_MASK_IRQ_0					= 0x01,	/**< xxxxxxx1 | Mask off IRQ 0 so won't send interrupt. */
	PIC_OCW1_MASK_IRQ_1					= 0x02,	/**< xxxxxx1x | Mask off IRQ 1 so won't send interrupt. */
	PIC_OCW1_MASK_IRQ_2					= 0x04,	/**< xxxxx1xx | Mask off IRQ 2 so won't send interrupt. */
	PIC_OCW1_MASK_IRQ_3					= 0x08,	/**< xxxx1xxx | Mask off IRQ 3 so won't send interrupt. */
	PIC_OCW1_MASK_IRQ_4					= 0x10,	/**< xxx1xxxx | Mask off IRQ 4 so won't send interrupt. */
	PIC_OCW1_MASK_IRQ_5					= 0x20,	/**< xx1xxxxx | Mask off IRQ 5 so won't send interrupt. */
	PIC_OCW1_MASK_IRQ_6					= 0x40,	/**< x1xxxxxx | Mask off IRQ 6 so won't send interrupt. */
	PIC_OCW1_MASK_IRQ_7					= 0x80,	/**< 1xxxxxxx | Mask off IRQ 7 so won't send interrupt. */
};

/**
 *  \brief Operation control word 2.
 */
enum pit_ocw2 {
	PIC_OCW2_INTERRUPT_LEVEL_1			= 0x01,	/**< xxxxxxx1 | The interrupt level 1, which the controller must react to. */
	PIC_OCW2_INTERRUPT_LEVEL_2			= 0x02,	/**< xxxxxx1x | The interrupt level 2, which the controller must react to. */
	PIC_OCW2_INTERRUPT_LEVEL_3			= 0x04,	/**< xxxxx1xx | The interrupt level 3, which the controller must react to. */
	// The next 2 bits must be 0
	PIC_OCW2_END_OF_INTERRUPT			= 0x20,	/**< xx1xxxxx | The end of interrupt command code. */
	PIC_OCW2_SELECTION					= 0x40,	/**< x1xxxxxx | Select command. */
	PIC_OCW2_ROTATION					= 0x80,	/**< 1xxxxxxx | Rotation command */
};

/**
 *  \brief Operation control word 3.
 */
enum pit_ocw3 {
	PIC_OCW3_READ_ISR					= 0x00,	/**< xxxxxxx0 | Read the In Service Register register, bit 0 is unset. */
	PIC_OCW3_READ_IRR					= 0x01,	/**< xxxxxxx1 | Read the Interrupt Request Register register, bit 0 is set. */
	PIC_OCW3_ACT_ON_READ				= 0x02,	/**< xxxxxx1x | If set, then bit 0 will be acted on, so read ISR or IRR. If not set, then no action taken. */
	PIC_OCW3_POLL_COMMAND_ISSUED		= 0x04,	/**< xxxxx1xx | If set, then poll command issued. If not set, then no pool command issued. */
	PIC_OCW3_DEFAULT					= 0x08,	/**< 0xx01xxx | As some bits need to be set and unset, use the following define for each use of the OCW3. */
	// Next bit must be 1
	// Next bit must be 0
	PIC_OCW3_SPECIAL_MASK				= 0x20,	/**< xx1xxxxx | If set, then the special mask is set. If not set, then resets special mask. */
	PIC_OCW3_ACT_ON_SPECIAL_MASK		= 0x40,	/**< x1xxxxxx | If set, then bit 5 will be acted on, so setting the special mask. If not set, then no action it taken. */
	// Next bit is unused so must be 0
};

/**
 *  \brief IRQ's numbers for the PIC.
 */
enum pit_irqs {
	PIC_IRQ_TIMER						= 0x00,	/**< The PIC IRQ for the timer. */
	PIC_IRQ_KEYBOARD					= 0x01,	/**< The PIC IRQ for the keyboard. */
	PIC_IRQ_CASCADE_FOR_SLAVE			= 0x02,	/**< The PIC IRQ for the cascade slave PIC. */
	PIC_IRQ_SERIAL_PORT_2				= 0x03,	/**< The PIC IRQ for the 2nd serial port. */
	PIC_IRQ_SERIAL_PORT_1				= 0x04,	/**< The PIC IRQ for the 1st serial port. */
	PIC_IRQ_PARALLEL_PORT_2				= 0x05,	/**< The PIC IRQ for the 2nd parallel port. */
	PIC_IRQ_DISKETTE_DRIVE				= 0x06,	/**< The PIC IRQ for the diskette drive. */
	PIC_IRQ_PARALLEL_PORT_1				= 0x07,	/**< The PIC IRQ for the 1st parallel port. */
	PIC_IRQ_CMOS_REALT_TIME_CLOCK		= 0x08,	/**< The PIC IRQ for the real time clock. */
	PIC_IRQ_CGA_VERTICAL_RETRACE		= 0x09,	/**< The PIC IRQ for the CGA vertical retrace. */
	PIC_IRQ_AUXILIARY_DEVICE			= 0x0C,	/**< The PIC IRQ for the auxiliary device. */
	PIC_IRQ_FPU							= 0x0D,	/**< The PIC IRQ for the floating point unit. */
	PIC_IRQ_HARD_DISK_CONTROLLER		= 0x0E,	/**< The PIC IRQ for the hard disk drive controller. */
};

/**
 *  \brief Send a command to the master PIC on its assigned port.
 *  
 *  \param [in] cmd The command to send.
 */
void pic_send_command_master(uint8_t cmd);

/**
 *  \brief Send data to the master PIC on its assigned port.
 *  
 *  \param [in] data The data to send.
 */
void pic_send_data_master(uint8_t data);

/**
 *  \brief Send a command to the slave PIC on its assigned port.
 *  
 *  \param [in] cmd The command to send.
 */
void pic_send_command_slave(uint8_t cmd);

/**
 *  \brief Send data to the slave PIC on its assigned port.
 *  
 *  \param [in] data The data to send.
 */
void pic_send_data_slave(uint8_t data);

/**
 *  \brief Receive data from the master PIC from its assigned port.
 *  
 *  \return The value from the Interrupt Mask Register.
 */
uint8_t pic_receive_data_master(void);

/**
 *  \brief Receive data from the slave PIC from its assigned port.
 *  
 *  \return The value from the Interrupt Mask Register.
 */
uint8_t pic_receive_data_slave(void);

/**
 *  \brief Send the end of interrupt command for a specific IRQ to the master and/or slave PIC's
 *  after an interrupt has completed.
 *  
 *  \param [in] irq The IRQ number for the interrupt that has finished.
 */
void pic_send_end_of_interrupt(uint8_t irq);

/**
 *  \brief Remap all the PIC IRQ's so that they don't conflict with the processors interrupts.
 */
void pic_remap_irq(void);

#endif /* INCLUDE_PIC_H */
