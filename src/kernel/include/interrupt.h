#ifndef INCLUDE_INTERRUPT_H
#define INCLUDE_INTERRUPT_H

static inline void interrupt_enable() {
	__asm__ __volatile__ ("sti");
}

static inline void interrupt_disable() {
	__asm__ __volatile__ ("cli");
}

#endif /* INCLUDE_INTERRUPT_H */
