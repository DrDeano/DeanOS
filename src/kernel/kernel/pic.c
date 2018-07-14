#include <pic.h>

void pic_send_command_master(uint8_t cmd) {
	out_port_byte(PIC_COMMAND_REG_MASTER, cmd);
}

void pic_send_data_master(uint8_t data) {
	out_port_byte(PIC_DATA_REG_MASTER, data);
}

void pic_send_command_slave(uint8_t cmd) {
	out_port_byte(PIC_COMMAND_REG_SLAVE, cmd);
}

void pic_send_data_slave(uint8_t data) {
	out_port_byte(PIC_DATA_REG_SLAVE, data);
}

uint8_t pic_receive_data_master(void) {
	return in_port_byte(PIC_DATA_REG_MASTER);
}

uint8_t pic_receive_data_slave(void) {
	return in_port_byte(PIC_DATA_REG_SLAVE);
}

void pic_send_end_of_interrupt(uint8_t irq) {
	// If IRQ is between 8 and 15 (inclusive) then send an EOI to the slave controller
	if (irq >= 8) {
		pic_send_command_slave(PIC_OCW2_END_OF_INTERRUPT);
	}

	// Then send an EOI to the master interrupt controller
	pic_send_command_master(PIC_OCW2_END_OF_INTERRUPT);
}

void irq_set_mask(uint8_t irq_num) {
    uint16_t port;
    uint8_t value;
	
    if(irq_num < 8) {
        port = PIC_INTERRUPT_MASK_REG_MASTER;
    } else {
        port = PIC_INTERRUPT_MASK_REG_SLAVE;
        irq_num -= 8;
    }
    value = in_port_byte(port) | (1 << irq_num);
    out_port_byte(port, value);        
}

void irq_clear_mask(uint8_t irq_num) {
    uint16_t port;
    uint8_t value;
	
    if(irq_num < 8) {
        port = PIC_INTERRUPT_MASK_REG_MASTER;
    } else {
        port = PIC_INTERRUPT_MASK_REG_SLAVE;
        irq_num -= 8;
    }
    value = in_port_byte(port) & ~(1 << irq_num);
    out_port_byte(port, value);        
}

void pic_remap_irq(void) {
	uint8_t mask_m;
	uint8_t mask_s;
	
	// Save the masks
	mask_m = pic_receive_data_master();
	mask_s = pic_receive_data_slave();
	
	// Initialise the master and slave PIC's (ICW1)
	pic_send_command_master(PIC_ICW1_INITIALISATION | PIC_ICW1_ICW4);
	pic_send_command_slave(PIC_ICW1_INITIALISATION | PIC_ICW1_ICW4);
	
	// Tell the master and slave PIC's the new IRQ offsets (ICW2)
	pic_send_data_master(PIC_MASTER_REMAP_OFFSET);
	pic_send_data_slave(PIC_SLAVE_REMAP_OFFSET);
	
	// Tell the master and slave PIC's which IRQ lines they are to pass interrupts over (ICW3)
	pic_send_data_master(PIC_ICW3_MASTER_IRQ_MAP_TO_SLAVE);
	pic_send_data_slave(PIC_ICW3_SLAVE_IRQ_MAP_TO_MASTER);
	
	// Tell the master and slave PIC's to be set in 80x86 mode (ICW4)
	pic_send_data_master(PIC_ICW4_80x86_MODE);
	pic_send_data_slave(PIC_ICW4_80x86_MODE);
	
	// Restore masks
	pic_send_data_master(mask_m);
	pic_send_data_slave(mask_s);
}
