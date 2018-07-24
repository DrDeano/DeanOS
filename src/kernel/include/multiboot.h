#ifndef INCLUDE_MULTIBOOT_H
#define INCLUDE_MULTIBOOT_H

#include <stdint.h>

/**
 *  \brief The multiboot magic that must be present in EAX.
 */
#define MULTIBOOT_MAGIC		0x2BADB002

#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED		0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB			1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT		2

#define MULTIBOOT_MEMORY_TYPE_AVAILABLE			1
#define MULTIBOOT_MEMORY_TYPE_RESERVED			2
#define MULTIBOOT_MEMORY_TYPE_ACPI_RECLAIMABLE	3
#define MULTIBOOT_MEMORY_TYPE_ACPI_NVS			4
#define MULTIBOOT_MEMORY_TYPE_BADRAM			5

typedef struct {
	uint32_t base_addr_lower;
	uint32_t base_addr_upper;
	uint32_t length_lower;
	uint32_t length_upper;
	uint32_t type;
	uint32_t acpi;
} __attribute__((__packed__))  multiboot_mmap_entry_t;

typedef struct {
	uint32_t tabsize;
	uint32_t strsize;
	uint32_t addr;
	uint32_t reserved;
} __attribute__((__packed__)) multiboot_aout_symbol_table_t;

typedef struct {
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
} __attribute__((__packed__)) multiboot_elf_section_header_table_t;

typedef struct {
	uint32_t flags;				/**< Multiboot info version number */
	
	uint32_t mem_lower;			/**< Available memory from BIOS, lower */
	uint32_t mem_upper;			/**< Available memory from BIOS, upper */
	
	uint32_t boot_device;		/**< The drive number the "root" partition is on */
	
	uint32_t cmdline;			/**< Kernel command line */
	
	/* Boot-Module list */
	uint32_t mods_count;
	uint32_t mods_addr;
	
	union {
		multiboot_aout_symbol_table_t aout_sym;
		multiboot_elf_section_header_table_t elf_sec;
	} __attribute__((__packed__));
 
	/* Memory Mapping buffer */
	uint32_t mem_map_length;
	uint32_t mem_map_addr;
	
	/* Drive Info buffer */
	uint32_t drives_length;
	uint32_t drives_addr;
	
	/* ROM configuration table */
	uint32_t config_table;
	
	/* Boot Loader Name */
	uint32_t boot_loader_name;
	
	/* APM table */
	uint32_t apm_table;
	
	/* Video */
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
	
	uint64_t framebuffer_addr;
	uint32_t framebuffer_pitch;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	uint8_t framebuffer_bpp;
	
	uint8_t framebuffer_type;
	union {
		struct {
			uint32_t framebuffer_palette_addr;
			uint16_t framebuffer_palette_num_colours;
		} __attribute__((__packed__));
		struct {
			uint8_t framebuffer_red_field_position;
			uint8_t framebuffer_red_mask_size;
			uint8_t framebuffer_green_field_position;
			uint8_t framebuffer_green_mask_size;
			uint8_t framebuffer_blue_field_position;
			uint8_t framebuffer_blue_mask_size;
		} __attribute__((__packed__));
	} __attribute__((__packed__));
} __attribute__((__packed__)) multiboot_info_t;

#endif /* INCLUDE_MULTIBOOT_H */
