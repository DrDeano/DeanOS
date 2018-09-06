/**
 *  \file kernel_main.c
 *  \brief The main kernel
 */
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <stdio.h>
#include <string.h>

#include <tty.h>
#include <boot.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <interrupt.h>
#include <pit.h>
#include <keyboard.h>
#include <main.h>
#include <rtc.h>
#include <speaker.h>
#include <pmm.h>
#include <paging.h>
#include <floppy.h>

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

static bool daylight_savings;

static char prev_command_buffer[10][64] = {0};
static int prev_command_buffer_end = 0;
static int prev_command_buffer_index = 0;

static char command_buffer[64] = {0};
static int command_buffer_index;

noreturn void panic(const char * format, ...) {
	// Disable interrupts.
	interrupt_disable();

    // Get args.
    va_list args;
	va_start(args, format);

	// Show panic.
	kprintf("\nPANIC:\n");
	kvprintf(format, args);
	kprintf("\n\nHalted");

	va_end(args);

	// Halt forever.
	while(1) {
		__asm__ __volatile__ ("sti");
		__asm__ __volatile__ ("hlt");
		__asm__ __volatile__ ("cli");
	}
	__builtin_unreachable();
}

/**
 *  \brief Brief description
 *
 *  \return Return description
 *
 *  \details More details
 *
 *  \todo Set daylight_savings correctly
 */
static void human_clock_init(void) {
	daylight_savings = true;
}

static void display_time(void) {
	static char * str_day[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	rtc_date_time_t date;
	get_time(&date);
	// DD-MM-YYYY hh:mm:ss
	kprintf("%s %02d-%02d-%04d %02d:%02d:%02d\n", str_day[date.day_of_week], date.day, date.month, date.year, date.hour, date.minute, date.second);
}

static void add_command(char * cmd) {
	memcpy(prev_command_buffer[prev_command_buffer_end], cmd, strlen(cmd) + 1);
	prev_command_buffer_end = (prev_command_buffer_end + 1) % 10;
	prev_command_buffer_index = prev_command_buffer_end;
}

static char * get_prev_cmd(void) {
	int count = 10;
	int save_index = prev_command_buffer_index;
	if(prev_command_buffer_index == 0) {
		prev_command_buffer_index = 10;
	}

	prev_command_buffer_index--;
	//save_index = prev_command_buffer_index;

	while((prev_command_buffer[prev_command_buffer_index][0] == '\0') && count > 0) {
		if(prev_command_buffer_index == prev_command_buffer_end) {
			prev_command_buffer_index = save_index;
			return prev_command_buffer[prev_command_buffer_index];
		}

		prev_command_buffer_index--;
		if(prev_command_buffer_index == 0) {
			prev_command_buffer_index = 10;
		}
		count--;
	}

	if(prev_command_buffer_index == prev_command_buffer_end) {
		prev_command_buffer_index = save_index;
		return prev_command_buffer[prev_command_buffer_index];
	}

	return prev_command_buffer[prev_command_buffer_index];
}

static void zero_cmd_buffer(void) {
	memset(command_buffer, 0, sizeof(command_buffer));
	command_buffer_index = 0;
}

static void move_left(void) {
	if(command_buffer_index) {
		move_cursor_left();
		command_buffer_index--;
	}
}

static void move_right(void) {
	if(command_buffer[command_buffer_index] != '\0') {
		move_cursor_right();
		command_buffer_index++;
	}
}

static void clear_line(void) {
	//int temp = command_buffer_index;

	// Start from the end
	for(unsigned int i = command_buffer_index; i < strlen(command_buffer); i++) {
		move_right();
		command_buffer_index--;
	}

	for(unsigned int i = 0; i < strlen(command_buffer); i++) {
		kputchar('\b');
		kputchar('\0');
		kputchar('\b');
	}

	// Move back to where started
	//command_buffer_index = temp;
}

static void add_char_to_cmd(char c) {
	// CHECK OVER FLOW
	clear_line();

	if(c == '\b') {
		if(command_buffer_index) {
			memmove(command_buffer + command_buffer_index - 1, command_buffer + command_buffer_index, strlen(command_buffer + command_buffer_index));

			command_buffer_index--;

			// Set last item to null
			command_buffer[strlen(command_buffer) - 1] = '\0';
		}

		goto print;
	} else if(c == '\t') {

		if(command_buffer_index + 4 > 62) {
			goto print;
		}

		if(command_buffer[command_buffer_index] != '\0') {
			memmove(command_buffer + command_buffer_index + 4, command_buffer + command_buffer_index, strlen(command_buffer + command_buffer_index));
		}
		command_buffer[command_buffer_index++] = ' ';
		command_buffer[command_buffer_index++] = ' ';
		command_buffer[command_buffer_index++] = ' ';
		command_buffer[command_buffer_index++] = ' ';
		goto print;
	}

	if(command_buffer_index + 1 > 62) {
		goto print;
	}

	// If half way into a word
	if(command_buffer[command_buffer_index] != '\0') {
		// Move all chars to right of the index one to the right
		memmove(command_buffer + command_buffer_index + 1, command_buffer + command_buffer_index, strlen(command_buffer + command_buffer_index));
	}

	// Add the char
	command_buffer[command_buffer_index++] = c;

	print:

	kprintf("%s", command_buffer);

	for(unsigned int i = command_buffer_index; i < strlen(command_buffer); i++) {
		command_buffer_index++;
		move_left();
	}
}

static char * get_next_cmd(void) {
	if(prev_command_buffer_index == prev_command_buffer_end) {
		return prev_command_buffer[prev_command_buffer_index];
	}

	if(prev_command_buffer_index == 10) {
		prev_command_buffer_index = 0;
	}

	prev_command_buffer_index++;
	return prev_command_buffer[prev_command_buffer_index];
}

static void kernel_task(void) {
	char * prev_cmd;

	const char * list_of_commands[] = {
		"help",
		"hello",
		"butt",
		"eg",
		"time",
		"uptime",
		"clear",
		"read"
	};

	const int num_commands = 8;

	char ch;
	while(true) {
		zero_cmd_buffer();
		kprintf("terminal:>");
		while(command_buffer_index < 62) {
			unsigned char key = get_last_key_press();

			while(key == KEYBOARD_KEY_UNKNOWN) {
				__asm__ __volatile__ ("sti");
				__asm__ __volatile__ ("hlt");
				__asm__ __volatile__ ("cli");
				key = get_last_key_press();
			}
			switch (key) {
				case KEYBOARD_KEY_ENTER:
					//kputchar('\n');
					break;

				case KEYBOARD_KEY_ESC:
					//kprintf("ESC\n");
					break;

				case KEYBOARD_KEY_F1:
					//kprintf("F1\n");
					memcpy(command_buffer, "help", 5);
					//command_buffer_index += 5;
					key = KEYBOARD_KEY_ENTER;
					break;

				case KEYBOARD_KEY_F2:
					//kprintf("F2\n");
					break;

				case KEYBOARD_KEY_F3:
					//kprintf("F3\n");
					break;

				case KEYBOARD_KEY_F4:
					//kprintf("F4\n");
					break;

				case KEYBOARD_KEY_F5:
					//kprintf("F5\n");
					break;

				case KEYBOARD_KEY_F6:
					//kprintf("F6\n");
					break;

				case KEYBOARD_KEY_F7:
					//kprintf("F7\n");
					break;

				case KEYBOARD_KEY_F8:
					//kprintf("F8\n");
					break;

				case KEYBOARD_KEY_F9:
					//kprintf("F9\n");
					break;

				case KEYBOARD_KEY_F10:
					//kprintf("F10\n");
					break;

				case KEYBOARD_KEY_F11:
					//kprintf("F11\n");
					break;

				case KEYBOARD_KEY_F12:
					//kprintf("F12\n");
					break;

				case KEYBOARD_KEY_NUM_LOCK:
					//kprintf("NUM\n");
					break;

				case KEYBOARD_KEY_CAPS_LOCK:
					//kprintf("CAPS\n");
					break;

				case KEYBOARD_KEY_SCROLL_LOCK:
					//kprintf("SCROLL\n");
					break;

				case KEYBOARD_KEY_LEFT_CTRL:
					//kprintf("LCTRL\n");
					break;

				case KEYBOARD_KEY_LEFT_SHIFT:
					//kprintf("LSHIFT\n");
					break;

				case KEYBOARD_KEY_LEFT_ALT:
					//kprintf("LALT\n");
					break;

				case KEYBOARD_KEY_RIGHT_CTRL:
					//kprintf("RCTRL\n");
					break;

				case KEYBOARD_KEY_RIGHT_SHIFT:
					//kprintf("RSHIFT\n");
					break;

				case KEYBOARD_KEY_RIGHT_ALT:
					//kprintf("RALT\n");
					break;

				case KEYBOARD_KEY_SYSREQ:
					//kprintf("SYSREQ\n");
					break;

				case KEYBOARD_KEY_INSERT:
					//kprintf("INSERT\n");
					break;

				case KEYBOARD_KEY_HOME:
					//kprintf("HOME\n");
					break;

				case KEYBOARD_KEY_PAGE_UP:
					//kprintf("PUP\n");
					break;

				case KEYBOARD_KEY_DELETE:
					//kprintf("DELETE\n");
					break;

				case KEYBOARD_KEY_END:
					//kprintf("END\n");
					break;

				case KEYBOARD_KEY_PAGE_DOWN:
					//kprintf("PDOWN\n");
					break;

				case KEYBOARD_KEY_ARROW_UP:
					//kprintf("AUP\n");
					clear_line();

					zero_cmd_buffer();

					prev_cmd = get_prev_cmd();

					kprintf("%s", prev_cmd);
					for(unsigned int i = 0; i < strlen(prev_cmd); i++) {
						command_buffer[command_buffer_index++] = prev_cmd[i];
					}
					//memcpy(command_buffer, prev_cmd, strlen(prev_cmd) + 1); // Includes the null terminator
					//command_buffer_index = strlen(prev_cmd) + 1;
					break;

				case KEYBOARD_KEY_ARROW_DOWN:
					//kprintf("ADOWN\n");
					clear_line();

					zero_cmd_buffer();

					prev_cmd = get_next_cmd();

					kprintf("%s", prev_cmd);
					for(unsigned int i = 0; i < strlen(prev_cmd); i++) {
						command_buffer[command_buffer_index++] = prev_cmd[i];
					}
					break;

				case KEYBOARD_KEY_ARROW_LEFT:
					//kprintf("ALEFT\n");
					move_left();
					break;

				case KEYBOARD_KEY_ARROW_RIGHT:
					//kprintf("ARIGHT\n");
					move_right();
					break;

				case KEYBOARD_KEY_BACKSPACE:
					if (command_buffer_index > 0) {
						/* kputchar('\b');
						kputchar('\0');
						kputchar('\b');
						//command_buffer_index--;
						command_buffer[command_buffer_index--] = '\0'; */
						add_char_to_cmd('\b');
					}

					break;

				case KEYBOARD_KEY_TAB:
					/* kputchar('\t');
					command_buffer[command_buffer_index++] = ' ';
					command_buffer[command_buffer_index++] = ' ';
					command_buffer[command_buffer_index++] = ' ';
					command_buffer[command_buffer_index++] = ' '; */
					add_char_to_cmd('\t');
					break;

				default:
					ch = key_to_ascii(key);
					if(ch) {
						/* kputchar(ch);

						command_buffer[command_buffer_index++] = ch; */
						add_char_to_cmd(ch);
					}
					break;
			}

			if(key == KEYBOARD_KEY_ENTER) {
				kputchar('\n');
				break;
			}
		}
		//command_buffer[command_buffer_index] = '\0';
		add_command(command_buffer);

		if(strcmp(command_buffer, "help") == 0) {
			kprintf("List of commands:\n");
			for(int i = 0; i < num_commands; i++) {
				kprintf("\t- %s\n", list_of_commands[i]);
			}
		} else if(strcmp(command_buffer, "hello") == 0) {
			kprintf("Hello there\n");
		} else if(strcmp(command_buffer, "uptime") == 0) {
			unsigned int ticks = get_pit_ticks();
			unsigned int freq = get_pit_frequency();
			unsigned int sec = (ticks / freq) % 60;
			unsigned int min = (ticks / (freq * 60)) % 60;
			unsigned int hr = (ticks / (freq * 60 * 60)) % 24;
			unsigned int day = (ticks / (freq * 60 * 60 * 24)) % 365;
			kprintf("%u ticks: day:hr:min:sec %3d:%02d:%02d:%02d\n", ticks, day, hr, min, sec);
		} else if(strcmp(command_buffer, "eg") == 0) {
			kprintf("Soph is a butt\n");
		} else if(strcmp(command_buffer, "time") == 0) {
			display_time();
		} else if(strcmp(command_buffer, "clear") == 0) {
			clear();
		} else if(strcmp(command_buffer, "butt") == 0) {
			kprintf("  \\           /   \n");
			kprintf("   \\         /    \n");
			kprintf("    )       (     \n");
			kprintf("  .`         `.   \n");
			kprintf(".'             `. \n");
			kprintf(":        |       :\n");
			kprintf("'.      .'.     .'\n");
			kprintf("  \\`'''`\\ /`'''`/ \n");
			kprintf("   \\     |     /  \n");
			kprintf("    |    |    |   \n");
		} else if(strcmp(command_buffer, "read") == 0) {
			uint8_t * sector = floppy_read_sector(1);
			if(sector) {
				for(int i = 1; i <= 128; i++) {
					kprintf("0x%2X ", (uint32_t) sector[i - 1]);
					if(i % 15 == 0 && i != 1) {
						kprintf("\n");
					}
				}
				kprintf("\n");
			} else {
				kprintf("Error reading\n");
			}
		} else {
			if(command_buffer[0]) {
				kprintf("%s: Command not found\n", command_buffer);
			}
		}
	}
}

void pmm_test(void) {
	uint32_t * p1 = (uint32_t *) pmm_alloc_block();
	kprintf("p1 allocated at 0x%0x\n", p1);

	uint32_t * p2 = (uint32_t *) pmm_alloc_blocks(2);
	kprintf("Allocated 2 blocks for p2 at 0x%0x\n", p2);

	uint32_t * p3 = (uint32_t *) pmm_alloc_block();
	kprintf("Allocated 1 blocks for p3 at 0x%0x\n", p3);

	pmm_free_blocks(p2, 2);
	kprintf("Unallocated p2 to free block 2\n");

	uint32_t * p4 = (uint32_t *) pmm_alloc_blocks(3);
	kprintf("Allocated 3 blocks for p4 at 0x%0x\n", p4);

	p2 = (uint32_t *) pmm_alloc_blocks(2);
	kprintf("Re-allocated 2 blocks for p2 at 0x%0x\n", p2);

	uint32_t * p5 = (uint32_t *) pmm_alloc_block();
	kprintf("Allocated 1 blocks for p5 at 0x%0x\n", p5);

	pmm_free_blocks(p4, 3);
	kprintf("Unallocated p4 to free block 3\n");

	uint32_t * p6 = (uint32_t *) pmm_alloc_blocks(4);
	kprintf("Allocated 4 blocks for p6 at 0x%0x\n", p6);

	pmm_free_block(p1);
	p1 = (uint32_t *) pmm_alloc_block();
	kprintf("Unallocated p1 to free block 1. p1 is reallocated to 0x%0x\n", p1);

	pmm_free_block(p1);
	pmm_free_blocks(p2, 2);
	pmm_free_block(p3);
	pmm_free_block(p5);
	pmm_free_blocks(p6, 4);
}

void paging_test(void) {
	// This should page fault as not mapped
	int * x = (int *) 0x4000000; // 1GB
	int y = *x;

	kprintf("y: %d\n", y);
}

/**
 *  \brief The kernel main entry point that initiates everything.
 */
noreturn void kernel_main(void) {
	// Get the parameters from the bootloader. The cursor position
	boot_params params;

	char * str_type[] = {
		"Available",
		"Reserved",
		"ACPI Reclaim",
		"ACPI NVS",
		"Bad memory"
	};

	get_boot_params(&params);

	/* Initialize terminal interface */
	terminal_initialise(&params);

	if(params.sig == 0) {
		panic("Unable to get parameters from bootloader\n");
	}

	kprintf("Kernel size: %u bytes\n", params.kernel_size);

	gdt_init();

	idt_init();

	isr_init();

	irq_init();

	pit_install();

	keyboard_init();

	interrupt_enable();

	// Add on the first MB.
	// mem_lower is the number of KB between 1MB and 16MB.
	// mem_upper is the number of 64KB blocks above 16MB, so need to multiply by 64 to get the number of KB.
	uint32_t mem_size = 1024 + params.memory_lower + (params.memory_upper * 64);

	uint32_t mem_map_len = params.memory_map_length + 0;

	memory_map_entry_t * mem_map = (memory_map_entry_t *) params.memory_map;

	kprintf("%uKB (%uMB), lower: %uKB (%uMB), upper: %u 64KB blocks (%uMB)\n", mem_size, mem_size / 1024, params.memory_lower, params.memory_lower / 1024, params.memory_upper, params.memory_upper / 16);

	kprintf("Memory map addr: 0x%08p. Memory map length: %u\n", mem_map, mem_map_len);

	// Place the memory bit map after DMA buffer
	pmm_init(mem_size, (uint32_t *) 0x4000);

	// Initiating memory regions
	for(uint32_t i = 0; i < mem_map_len; i++) {
		// Sanity check if type is above 5
		if(mem_map[i].type > 5) {
			mem_map[i].type = 1;		// If so, mark it reserved
		}

		// If start address is zero when not the first memory map, then at end so break. Shouldn't need to as got the number of the memory map entries.
		if(i > 0 && mem_map[i].base_addr_lower == 0 && mem_map[i].base_addr_upper == 0) {
			break;
		}

		kprintf("%u: Start addr: 0x%08X%08X Len: 0x%08X%08X Type: %u-%s\n", i, mem_map[i].base_addr_upper, mem_map[i].base_addr_lower, mem_map[i].length_upper, mem_map[i].length_lower, mem_map[i].type, str_type[mem_map[i].type - 1]);

		// If type is 1 (available), then initiate the region so can be allocated
		if(mem_map[i].type == 1) {
			pmm_init_region(mem_map[i].base_addr_lower, mem_map[i].length_lower);
		}
	}

	// Uninitialise the floppy DMA buffer
	pmm_uninit_region(0x1000, 0x3000);

	// Uninitialise the kernel stack region
	pmm_uninit_region(0x24000, 0x7AC00);

	// Uninitialise the kernel memory region
	pmm_uninit_region(0x100000, params.kernel_size);

	kprintf("Total number of blocks: %u. Used blocks: %u. Free blocks: %u\n", pmm_get_max_blocks(), pmm_get_used_blocks(), pmm_get_free_blocks());

	//pmm_test();

	paging_init();

	//paging_test();

	rtc_init();

	floppy_set_working_drive(0);

	floppy_init();

	human_clock_init();

	speaker_init();

    kernel_task();

	while(1) {
		__asm__ __volatile__ ("sti");
		__asm__ __volatile__ ("hlt");
		__asm__ __volatile__ ("cli");
	}
	__builtin_unreachable();
}
