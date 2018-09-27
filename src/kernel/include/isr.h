/**
 *  \file isr.h
 *  \brief Functions, definitions and structures for setting up the Interrupt Service Routines.
 */
#ifndef INCLUDE_ISR_H
#define INCLUDE_ISR_H

#include <stdint.h>
#include <regs_t.h>

/**
 *  \brief The total number of ISR's
 */
#define ISR_TOTAL 32

/**
 *  \brief A list of exceptions that can be generated that need to be handled.
 */
enum exceptions {
    EXCEPTION_DIVIDE_BY_ZERO				= 0,	/**< Divide By Zero exception. */
    EXCEPTION_SINGLE_STEP_DEBUG				= 1,	/**< Single Step (Debugger) exception. */
    EXCEPTION_NON_MASKABLE_INTERRUPT		= 2,	/**< Non Maskable Interrupt exception. */
    EXCEPTION_BREAKPOINT_DEBUG				= 3,	/**< Breakpoint (Debugger) exception. */
    EXCEPTION_OVERFLOW						= 4,	/**< Overflow exception. */
    EXCEPTION_BOUND_RANGE_EXCEEDED			= 5,	/**< Bound Range Exceeded exception. */
    EXCEPTION_INVALID_OPCODE				= 6,	/**< Invalid Opcode exception. */
    EXCEPTION_DEVICE_NOT_AVAILABLE			= 7,	/**< No Coprocessor, Device Not Available exception. */
    EXCEPTION_DOUBLE_FAULT					= 8,	/**< Double Fault exception. */
    EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN	= 9,	/**< Coprocessor Segment Overrun exception. */
    EXCEPTION_INVALID_TASK_STATE_SEGMENT	= 10,	/**< Invalid Task State Segment (TSS) exception. */
    EXCEPTION_SEGMENT_NOT_PRESENT			= 11,	/**< Segment Not Present exception. */
    EXCEPTION_STACK_SEGMENT_FAULT			= 12,	/**< Stack Segment Overrun exception. */
    EXCEPTION_GENERAL_PROTECTION_FAULT		= 13,	/**< General Protection Fault exception. */
    EXCEPTION_PAGE_FAULT					= 14,	/**< Page Fault exception. */
    EXCEPTION_X87_FLOAT_POINT				= 16,	/**< x87 FPU Floating Point Error exception. */
    EXCEPTION_ALIGNMENT_CHECK				= 17,	/**< Alignment Check exception. */
    EXCEPTION_MACHINE_CHECK					= 18,	/**< Machine Check exception. */
    EXCEPTION_SIMD_FLOAT_POINT				= 19,	/**< SIMD Floating Point exception. */
    EXCEPTION_VIRTUALIZATION				= 20,	/**< Virtualisation exception. */
    EXCEPTION_SECURITY						= 30	/**< Security exception. */
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
 *  \brief Initiates the ISR's by adding to the IDT the exception handlers for each ISR.
 */
void isr_init(void);

#endif /* INCLUDE_ISR_H */
