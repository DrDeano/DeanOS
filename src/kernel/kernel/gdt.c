#include <gdt.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static gdt_entry_t gdt_entries[GDT_ENTRIES];
static gdt_ptr_t gdt_ptr;

static tss_t tss;

inline static void _gdt_load(gdt_ptr_t * ptr, uintptr_t offset) {
	__asm__ __volatile__ ("lgdt [eax]" : : "a" (ptr));
	__asm__ __volatile__ ("mov ds, bx" : : "b" (offset));
	__asm__ __volatile__ ("mov es, bx" : : "b" (offset));
	__asm__ __volatile__ ("mov fs, bx" : : "b" (offset));
	__asm__ __volatile__ ("mov gs, bx" : : "b" (offset));
	__asm__ __volatile__ ("mov ss, bx" : : "b" (offset));
	__asm__ __volatile__ ("jmp 0x08:.flush");
	__asm__ __volatile__ (".flush:");
}

inline static void _tss_load(uintptr_t offset) {
	__asm__ __volatile__ ("ltr ax" : : "a" (offset));
}

static void gdt_set_entry(size_t index, bool is_code, bool is_user_mode) {
	// Set up base address. Aways 0
    gdt_entries[index].base_low = 0;
	gdt_entries[index].base_high = 0;
	
	// Set up limits. Aways 0xFFFFF
	gdt_entries[index].limit_low = 0xFFFF;
	gdt_entries[index].limit_high = 0xF;
	
	// Set up access bits
	gdt_entries[index].writeable = true;
	gdt_entries[index].direction_conforming = false;
	gdt_entries[index].executable = is_code;
	gdt_entries[index].descriptor_bit = true;
	gdt_entries[index].privilege = is_user_mode ? GDT_PRIVILEGE_USER : GDT_PRIVILEGE_KERNEL;
	gdt_entries[index].present = true;
	
	// Set up flags
	gdt_entries[index].reserved_zero = 0;
	gdt_entries[index].is_64bits = false;
	gdt_entries[index].is_32bits = true;
	gdt_entries[index].is_limit_4K = true;
}

static void tss_set_entry(uint32_t base, uint32_t limit) {
	// Set up base address
    gdt_entries[GDT_TSS_INDEX].base_low = (base & 0xFFFFFF);
	gdt_entries[GDT_TSS_INDEX].base_high = (base >> 24) & 0xFF;
	
	// Set up limits
	gdt_entries[GDT_TSS_INDEX].limit_low = (limit & 0xFFFF);
	gdt_entries[GDT_TSS_INDEX].limit_high = (limit >> 16) & 0x0F;
	
	// Set up access bits
	gdt_entries[GDT_TSS_INDEX].accessed = true;
	gdt_entries[GDT_TSS_INDEX].writeable = false;
	gdt_entries[GDT_TSS_INDEX].direction_conforming = false;
	gdt_entries[GDT_TSS_INDEX].executable = true;
	gdt_entries[GDT_TSS_INDEX].descriptor_bit = false;
	gdt_entries[GDT_TSS_INDEX].privilege = GDT_PRIVILEGE_USER;
	gdt_entries[GDT_TSS_INDEX].present = true;
	
	// Set up flags
	gdt_entries[GDT_TSS_INDEX].reserved_zero = 0;
	gdt_entries[GDT_TSS_INDEX].is_64bits = false;
	gdt_entries[GDT_TSS_INDEX].is_32bits = false;
	gdt_entries[GDT_TSS_INDEX].is_limit_4K = false;
}

static void gdt_setup() {
	// NULL segment
	gdt_entries[GDT_NULL_INDEX] = (gdt_entry_t) { };
	
	// Set code and data segments for kernel
	gdt_set_entry(GDT_KERNEL_CODE_INDEX, true, false);
	gdt_set_entry(GDT_KERNEL_DATA_INDEX, false, false);
	
	// Set code and data segments for user
	gdt_set_entry(GDT_USER_CODE_INDEX, true, true);
	gdt_set_entry(GDT_USER_DATA_INDEX, false, true);
	
	// Setup TSS
	tss_set_entry((uint32_t) &tss, sizeof(tss_t) - 1);
}

static void gdt_load() {
	// Create the GDT table
	gdt_ptr.num_of_entries = (sizeof(gdt_entry_t) * GDT_ENTRIES) - 1;
	gdt_ptr.gdt_location = gdt_entries;
	
	_gdt_load(&gdt_ptr, GDT_KERNEL_DATA_OFFSET);
}

static void tss_load() {
	_tss_load(GDT_TSS_OFFSET);
}

static void tss_setup() {
	tss.SS0 = GDT_KERNEL_DATA_OFFSET;
	tss.ESP0 = 0;
	tss.IO_Permissions_Base_Offset = (uint16_t) sizeof(tss_t);
	//tss.ES = GDT_KERNEL_DATA_OFFSET;
	//tss.CS = GDT_KERNEL_CODE_OFFSET;
	//tss.FS = 0x13; // kernel data segment
	//tss.GS = 0x13; // kernel data segment
}

void gdt_init() {
	// Zero out the TSS
	memset(&tss, 0, sizeof(tss_t));
	
	// Initialise the stack segment
	tss_setup();
	
	kprintf("Initialising GDT at 0x%p\n", &gdt_entries);
	gdt_setup();
	
	// Load the gdt table
	gdt_load();
	
	// Load the tss
	tss_load();
}
