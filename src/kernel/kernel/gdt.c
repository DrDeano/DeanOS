#include <gdt.h>
#include <string.h>
#include <stdint.h>

static gdt_entry_t gdt_entries[GDT_ENTRIES];	/**< The GDT entry table of \ref GDT_ENTRIES entries. */
static gdt_ptr_t gdt_ptr;						/**< The GDT pointer that the CPU is loaded with that contains the base address of the GDT and the size. */
static tss_t tss;								/**< The task state segment entry. */

/**
 * \brief Inline assembly for loading the GDT and refreshing the code segment with the code
 * segment offset of the kernel as we are still in kernel land. Also loads the kernel data segment
 * into all the other segment registers.
 */
static inline void lgdt(void) {
	__asm__ __volatile__ ("lgdt [eax]" : : "a" (&gdt_ptr));					// Load the GDT into the CPU
	__asm__ __volatile__ ("mov bx, %0" : : "rN" (GDT_KERNEL_DATA_OFFSET));	// Load the kernel data segment, index into the GDT
	__asm__ __volatile__ ("mov ds, bx");
	__asm__ __volatile__ ("mov es, bx");
	__asm__ __volatile__ ("mov fs, bx");
	__asm__ __volatile__ ("mov gs, bx");
	__asm__ __volatile__ ("mov ss, bx");
	__asm__ __volatile__ ("jmp 0x08:.flush");								// Load the kernel code segment into the CS register
	__asm__ __volatile__ (".flush:");
}

/**
 * \brief Inline assembly for loading the TSS into the CPU.
 */
static inline void ltr(void) {
	__asm__ __volatile__ ("ltr ax" : : "a" (GDT_TSS_OFFSET));
}

/**
 * \brief Create an GDT entry into the table with values provided.
 * 
 * \param [in] index The index into the GDT table for the entry being created.
 * \param [in] is_code Whether the entry is a code or data segment.
 * \param [in] ring_level Which ring level the code or data segment is to be created for.
 */
static void gdt_set_entry(size_t index, bool is_code, uint8_t ring_level) {
	// Set up base address. Always 0
	gdt_entries[index].base_low = 0;
	gdt_entries[index].base_high = 0;
	
	// Set up limits. Always 0xFFFFF
	gdt_entries[index].limit_low = 0xFFFF;
	gdt_entries[index].limit_high = 0xF;
	
	// Set up access bits
	gdt_entries[index].writeable = true;
	gdt_entries[index].direction_conforming = false;
	gdt_entries[index].executable = is_code;
	gdt_entries[index].descriptor_bit = true;
	gdt_entries[index].privilege = (0x03 & ring_level);
	gdt_entries[index].present = true;
	
	// Set up flags
	gdt_entries[index].reserved_zero = 0;
	gdt_entries[index].is_64bits = false;
	gdt_entries[index].is_32bits = true;
	gdt_entries[index].is_limit_4K = true;
}

/**
 * \brief Create a GDT entry for the TSS.
 */
static void tss_set_entry(void) {
	// Set up base address
	gdt_entries[GDT_TSS_INDEX].base_low = (((uintptr_t) &tss) & 0xFFFFFF);
	gdt_entries[GDT_TSS_INDEX].base_high = (((uintptr_t) &tss) >> 24) & 0xFF;
	
	// Set up limits
	gdt_entries[GDT_TSS_INDEX].limit_low = ((sizeof(tss_t) - 1) & 0xFFFF);
	gdt_entries[GDT_TSS_INDEX].limit_high = ((sizeof(tss_t) - 1) >> 16) & 0x0F;
	
	// Set up access bits
	gdt_entries[GDT_TSS_INDEX].accessed = true;
	gdt_entries[GDT_TSS_INDEX].writeable = false;
	gdt_entries[GDT_TSS_INDEX].direction_conforming = false;
	gdt_entries[GDT_TSS_INDEX].executable = true;
	gdt_entries[GDT_TSS_INDEX].descriptor_bit = false;
	gdt_entries[GDT_TSS_INDEX].privilege = GDT_PRIVILEGE_RING_3;
	gdt_entries[GDT_TSS_INDEX].present = true;
	
	// Set up flags
	gdt_entries[GDT_TSS_INDEX].reserved_zero = 0;
	gdt_entries[GDT_TSS_INDEX].is_64bits = false;
	gdt_entries[GDT_TSS_INDEX].is_32bits = false;
	gdt_entries[GDT_TSS_INDEX].is_limit_4K = false;
}

/**
 * \brief Set up the full GDT table with a kernel code and data segment, user code and data
 * segment, and the TSS.
 */
static void gdt_setup(void) {
	// NULL segment
	gdt_entries[GDT_NULL_INDEX] = (gdt_entry_t) { };
	
	// Set code and data segments for kernel
	gdt_set_entry(GDT_KERNEL_CODE_INDEX, true, GDT_PRIVILEGE_RING_0);
	gdt_set_entry(GDT_KERNEL_DATA_INDEX, false, GDT_PRIVILEGE_RING_0);
	
	// Set code and data segments for user
	gdt_set_entry(GDT_USER_CODE_INDEX, true, GDT_PRIVILEGE_RING_3);
	gdt_set_entry(GDT_USER_DATA_INDEX, false, GDT_PRIVILEGE_RING_3);
	
	// Set up TSS
	tss_set_entry();
}

/**
 * \brief Load the GDT into the CPU by creating the GDT pointer then loading it.
 */
static void gdt_load(void) {
	// Create the GDT table
	gdt_ptr.num_of_entries = (sizeof(gdt_entry_t) * GDT_ENTRIES) - 1;
	gdt_ptr.gdt_location = gdt_entries;
	
	// Load the GDT
	lgdt();
}

/**
 * \brief Set up the TSS with the default values.
 */
static void tss_setup(void) {
	tss.SS0 = GDT_KERNEL_DATA_OFFSET;
	tss.ESP0 = 0;
	tss.IO_Permissions_Base_Offset = (uint16_t) sizeof(tss_t);
}

void gdt_init(void) {
	// Zero out the TSS
	memset(&tss, 0, sizeof(tss_t));
	
	// Initialise the stack segment
	tss_setup();
	
	gdt_setup();
	
	// Load the GST table
	gdt_load();
	
	// Load the TSS
	ltr();
}
