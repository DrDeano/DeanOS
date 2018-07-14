/**
 *  \file interrupt.h
 *  \brief Inline assembly for enabling and disabling interrupts.
 */
#ifndef INCLUDE_INTERRUPT_H
#define INCLUDE_INTERRUPT_H

/**
 *  \brief Enable interrupts.
 */
static inline void interrupt_enable(void) {
	__asm__ __volatile__ ("sti");
}

/**
 *  \brief Disable interrupts.
 */
static inline void interrupt_disable(void) {
	__asm__ __volatile__ ("cli");
}

#endif /* INCLUDE_INTERRUPT_H */
