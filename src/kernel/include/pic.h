/**
 *  \file pic.h
 *
 *  \brief The Programmable Interrupt Controller interface for setting up the PIC and reading and
 *  writing to the PIC
 *  
 *  \todo For remapping, maybe need an io_wait between each PIC sending, see https://wiki.osdev.org/PIC.
 */
#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H

#include <stdint.h>
#include <portio.h>

/**
 * \def PIC_COMMAND_REG_MASTER
 *
 * \brief The port address for the master PIC command register. (Write only).
 */
#define PIC_COMMAND_REG_MASTER			0x20

/**
 * \def PIC_STATUS_REG_MASTER
 *
 * \brief The port address for the master PIC status register. (Read only).
 */
#define PIC_STATUS_REG_MASTER			0x20

/**
 * \def PIC_DATA_REG_MASTER
 *
 * \brief The port address for the master PIC data register.
 */
#define PIC_DATA_REG_MASTER				0x21

/**
 * \def PIC_INTERRUPT_MASK_REG_MASTER
 *
 * \brief The port address for the master PIC interrupt mask register.
 */
#define PIC_INTERRUPT_MASK_REG_MASTER	0x21

/**
 * \def PIC_COMMAND_REG_SLAVE
 *
 * \brief The port address for the slave PIC command register. (Write only).
 */
#define PIC_COMMAND_REG_SLAVE			0xA0

/**
 * \def PIC_STATUS_REG_SLAVE
 *
 * \brief The port address for the slave PIC status register. (Read only).
 */
#define PIC_STATUS_REG_SLAVE			0xA0

/**
 * \def PIC_DATA_REG_SLAVE
 *
 * \brief The port address for the slave PIC data register.
 */
#define PIC_DATA_REG_SLAVE				0xA1

/**
 * \def PIC_INTERRUPT_MASK_REG_SLAVE
 *
 * \brief The port address for the slave PIC interrupt mask register.
 */
#define PIC_INTERRUPT_MASK_REG_SLAVE	0xA1

// ----------------------------------
// The initialisation control word 1. Primary control word for initialising the PIC.
// ----------------------------------

/**
 * \def PIC_ICW1_ICW4
 *
 * \brief If set, then the PIC expects to receive a initialisation control word 4.
 */
#define PIC_ICW1_ICW4					0x01	// xxxxxxx1

/**
 * \def PIC_ICW1_SIGNAL
 *
 * \brief If set, then there is only one PIC in the system. If not set, then PIC is cascaded with
 * slave PIC's and initialisation control word 3 must be sent to the controller.
 */
#define PIC_ICW1_SINGLE					0x02	// xxxxxx1x

/**
 * \def PIC_ICW1_INTERVAL_4
 *
 * \brief If set, then the internal CALL address is 4. If not set, then is 8. Usually ignored by
 * x86. So default is not set, 0.
 */
#define PIC_ICW1_INTERVAL_4				0x04	// xxxxx1xx

/**
 * \def PIC_ICW1_LEVEL_TRIGGER
 *
 * \brief If set, then operating in level triggered mode. If not set, then operating in edge
 * triggered mode.
 */
#define PIC_ICW1_LEVEL_TRIGGER			0x08	// xxxx1xxx

/**
 * \def PIC_ICW1_INITIALISATION
 *
 * \brief If set, then the PIC is to be initialised.
 */
#define PIC_ICW1_INITIALISATION			0x10	// xxx1xxxx

// Upper 3 bits must be 0 for x86

// ----------------------------------
// The initialisation control word 2. Map the base address of the interrupt vector table.
// ----------------------------------

/**
 * \def PIC_MASTER_REMAP_OFFSET
 *
 * \brief The new port map for the master PIC.
 */
#define PIC_MASTER_REMAP_OFFSET			0x20	// 32

/**
 * \def PIC_SLAVE_REMAP_OFFSET
 *
 * \brief The new port map for the slave PIC.
 */
#define PIC_SLAVE_REMAP_OFFSET			0x28	// 40

// ----------------------------------
// The initialisation control word 3. For Telling the master and slave where the cascading
// interrupts are coming from.
// ----------------------------------

/**
 * \def PIC_ICW3_MASTER_IRQ_MAP_SLAVE
 *
 * \brief Tell the master PIT to receive interrupts from the slave PIC on IRQ2, which is the third
 * bit.
 */
#define PIC_ICW3_MASTER_IRQ_MAP_TO_SLAVE	0x04	// xxxxx1xx

/**
 * \def PIC_ICW3_SLAVE_IRQ_MAP_TO_MASTER
 *
 * \brief Tell the slave PIT to send interrupts to the master PIC on IRQ2, which is binary 2 (010).
 */
#define PIC_ICW3_SLAVE_IRQ_MAP_TO_MASTER	0x02	// xxxxxx1x

// ----------------------------------
// The initialisation control word 4. Tell the master and slave what mode to operate in.
// ----------------------------------

/**
 * \def PIC_ICW4_80x86_MODE
 *
 * \brief If set, then in 80x86 mode. If not set, then in MCS-80/86 mode
 */
#define PIC_ICW4_80x86_MODE				0x01	// xxxxxxx1

/**
 * \def PIC_ICW4_AUTO_EOI
 *
 * \brief If set, then on last interrupt acknowledge pulse the PIC automatically performs end of
 * interrupt operation.
 */
#define PIC_ICW4_AUTO_EOI				0x02	// xxxxxx1x

/**
 * \def PIC_ICW4_BUFFER_SELECT
 *
 * \brief Only use if PIC_ICW4_BUFFER_MODE is set. If set, then selects master's buffer. If not set
 * then uses slave's buffer.
 */
#define PIC_ICW4_BUFFER_SELECT			0x04	// xxxxx1xx


/**
 * \def PIC_ICW4_BUFFER_MODE
 *
 * \brief If set, then PIC operates in buffered mode.
 */
#define PIC_ICW4_BUFFER_MODE			0x08	// xxxx1xxx


/**
 * \def PIC_ICW4_SPECIAL_FULLY_NESTED_MODE
 *
 * \brief If set, then the the system had many cascaded PIC's. Not supported in x86.
 */
#define PIC_ICW4_SPECIAL_FULLY_NESTED_MODE	0x10	// xxx1xxxx


// The upper 3 bits must be 0

// -------------------------
// Operation control word 1.
// -------------------------

// -------------------------
// Operation control word 2.
// -------------------------

/**
 * \def PIC_END_OF_INTERRUPT
 *
 * \brief The end of interrupt command code.
 */
#define PIC_END_OF_INTERRUPT			0x20

/**
 *  \brief Send a command to the master PIC on its assigned port.
 *  
 *  \param [in] cmd The command to send.
 */
static inline void pic_send_command_master(uint8_t cmd) {
	out_port_byte(PIC_COMMAND_REG_MASTER, cmd);
}

/**
 *  \brief Send data to the master PIC on its assigned port.
 *  
 *  \param [in] data The data to send.
 */
static inline void pic_send_data_master(uint8_t data) {
	out_port_byte(PIC_DATA_REG_MASTER, data);
}

/**
 *  \brief Send a command to the slave PIC on its assigned port.
 *  
 *  \param [in] cmd The command to send.
 */
static inline void pic_send_command_slave(uint8_t cmd) {
	out_port_byte(PIC_COMMAND_REG_SLAVE, cmd);
}

/**
 *  \brief Send data to the slave PIC on its assigned port.
 *  
 *  \param [in] data The data to send.
 */
static inline void pic_send_data_slave(uint8_t data) {
	out_port_byte(PIC_DATA_REG_SLAVE, data);
}

/**
 *  \brief Receive data from the master PIC from its assigned port.
 *  
 *  \return The value from the Interrupt Mask Register.
 */
static inline uint8_t pic_receive_data_master() {
	return in_port_byte(PIC_DATA_REG_MASTER);
}

/**
 *  \brief Receive data from the slave PIC from its assigned port.
 *  
 *  \return The value from the Interrupt Mask Register.
 */
static inline uint8_t pic_receive_data_slave() {
	return in_port_byte(PIC_DATA_REG_SLAVE);
}

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
void pic_remap_irq();

#endif /* INCLUDE_PIC_H */
