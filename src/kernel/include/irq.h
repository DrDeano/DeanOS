/**
 *  \file irq.h
 *  \brief The Interrupt Request handler functions for adding, removing and setting up the IRQ's.
 */
#ifndef INCLUDE_IRQ_H
#define INCLUDE_IRQ_H

#include <regs_t.h>

/**
 *  \def IRQ_TOTAL
 *  
 *  \brief The total number of IRQ's that can be handled
 */
#define IRQ_TOTAL	16

/**
 *  \typedef typedef void (*irq_handler)(regs_t * regs)
 *  \brief The type of a IRQ handler
 *  
 *  \param [in] regs Takes registers that were present when the handler was called.
 */
typedef void (*irq_handler)(regs_t * regs);

/**
 *  \brief Install a new IRQ handler for a given IRQ number.
 *  
 *  \param [in] irq_num The IRQ number that the new handler will be installed for.
 *  \param [in] handler The handler for the IRQ that will be run when interrupt is called.
 */
void irq_install_handler(int irq_num, irq_handler handler);

/**
 *  \brief Uninstall a IRQ handler for a given IRQ number.
 *  
 *  \param [in] irq_num The IRQ number for which the handler for the respective IRQ number will be
 *  uninstalled and be replaced with NULL.
 */
void irq_uninstall_handler(int irq_num);

/**
 *  \brief Initiate the IRQ's by adding to the IDT the interrupt handlers for each IRS.
 */
void irq_init(void);

#endif /* INCLUDE_IRQ_H */
