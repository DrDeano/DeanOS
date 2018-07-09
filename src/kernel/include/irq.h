#ifndef INCLUDE_IRQ_H
#define INCLUDE_IRQ_H

#include <regs_t.h>

#define IRQ_TOTAL	16

// IRQ handler type
typedef void (*irq_handler)(regs_t * regs);

void irq_install_handler(int irq, irq_handler handler);
void irq_uninstall_handler(int irq);
void irq_init();

#endif /* INCLUDE_IRQ_H */
