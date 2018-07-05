#ifndef INCLUDE_ISR_H
#define INCLUDE_ISR_H

#include <stdint.h>
#include <regs_t.h>

#define ISR_TOTAL 32

// Exceptions.
// https://wiki.osdev.org/Exceptions
enum {
    EXCEPTION_DIVIDE_BY_ZERO            = 0,
    EXCEPTION_DEBUG                     = 1,
    EXCEPTION_NON_MASKABLE_INTERRUPT    = 2,
    EXCEPTION_BREAKPOINT                = 3,
    EXCEPTION_OVERFLOW                  = 4,
    EXCEPTION_BOUND_RANGE_EXCEEDED      = 5,
    EXCEPTION_INVALID_OPCODE            = 6,
    EXCEPTION_DEVICE_NOT_AVAILABLE      = 7,
    EXCEPTION_DOUBLE_FAULT              = 8,
    EXCEPTION_COPROCESSOR_SEG_OVERRUN   = 9,
    EXCEPTION_INVALID_TSS               = 10,
    EXCEPTION_SEGMENT_NOT_PRESENT       = 11,
    EXCEPTION_STACK_SEGMENT_FAULT       = 12,
    EXCEPTION_GENERAL_PROTECTION_FAULT  = 13,
    EXCEPTION_PAGE_FAULT                = 14,
    EXCEPTION_X87_FLOAT_POINT           = 16,
    EXCEPTION_ALIGNMENT_CHECK           = 17,
    EXCEPTION_MACHINE_CHECK             = 18,
    EXCEPTION_SIMD_FLOAT_POINT          = 19,
    EXCEPTION_VIRTUALIZATION            = 20,
    EXCEPTION_SECURITY                  = 30
};

// Exception handler type
typedef void (*isr_handler)(regs_t * regs);

void isr_install_handler(uint8_t isr_num, isr_handler handler);
void isr_uninstall_handler(uint8_t isr_num);

void isr_init();

#endif /* INCLUDE_ISR_H */
