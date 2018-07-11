/**
 *  \file isr.h
 *  \brief The Interrupt Service Routines for setting up exceptions
 */
#ifndef INCLUDE_ISR_H
#define INCLUDE_ISR_H

#include <stdint.h>
#include <regs_t.h>

/**
 *  \def ISR_TOTAL
 *  
 *  \brief The total number of ISR's
 */
#define ISR_TOTAL 32

/**
 *  A list of exceptions that can be generated that need to be handled.
 *  https://wiki.osdev.org/Exceptions
 */
enum {
    EXCEPTION_DIVIDE_BY_ZERO				= 0,
    EXCEPTION_SINGLE_STEP_DEBUG				= 1,
    EXCEPTION_NON_MASKABLE_INTERRUPT		= 2,
    EXCEPTION_BREAKPOINT_DEBUG				= 3,
    EXCEPTION_OVERFLOW						= 4,
    EXCEPTION_BOUND_RANGE_EXCEEDED			= 5,
    EXCEPTION_INVALID_OPCODE				= 6,
    EXCEPTION_DEVICE_NOT_AVAILABLE			= 7,
    EXCEPTION_DOUBLE_FAULT					= 8,
    EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN	= 9,
    EXCEPTION_INVALID_TASK_STATE_SEGMENT	= 10,
    EXCEPTION_SEGMENT_NOT_PRESENT			= 11,
    EXCEPTION_STACK_SEGMENT_FAULT			= 12,
    EXCEPTION_GENERAL_PROTECTION_FAULT		= 13,
    EXCEPTION_PAGE_FAULT					= 14,
    EXCEPTION_X87_FLOAT_POINT				= 16,
    EXCEPTION_ALIGNMENT_CHECK				= 17,
    EXCEPTION_MACHINE_CHECK					= 18,
    EXCEPTION_SIMD_FLOAT_POINT				= 19,
    EXCEPTION_VIRTUALIZATION				= 20,
    EXCEPTION_SECURITY						= 30
};

/**
 *  \typedef typedef void (*isr_handler)(regs_t * regs)
 *  \brief The type of a exception handler
 *  
 *  \param [in] regs Takes registers that were present when the handler was called.
 */
typedef void (*isr_handler)(regs_t * regs);

/**
 *  \brief Install a new exception handler for a given ISR number.
 *  
 *  \param [in] isr_num The ISR number that the new handler will be installed for.
 *  \param [in] handler The handler for the ISR that will be run when interrupt is called.
 */
void isr_install_handler(uint8_t isr_num, isr_handler handler);

/**
 *  \brief Uninstall a exception handler for a given ISR number.
 *  
 *  \param [in] isr_num The ISR number for which the handler for the respective ISR number will be
 *  uninstalled and be replaced with NULL.
 */
void isr_uninstall_handler(uint8_t isr_num);

/**
 *  \brief Initiate the ISR's by adding to the IDT the exception handlers for each IRS.
 */
void isr_init();

#endif /* INCLUDE_ISR_H */
