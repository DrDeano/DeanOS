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

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

static bool daylight_savings;

static char prev_command_buffer[10][64] = {0};
static int prev_command_buffer_end = 0;
static int prev_command_buffer_index = 0;

static char command_buffer[64] = {0};
int command_buffer_index;

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
	__asm__ __volatile__ ("hlt");
	while (1) {
		__asm__ __volatile__ ("hlt");
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
		"clear"
	};

	const int num_commands = 7;
	
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
		} else if (strcmp(command_buffer, "uptime") == 0) {
			unsigned int ticks = get_pit_ticks();
			unsigned int div = get_pit_divisor();
			unsigned int sec = (ticks / div) % 60;
			unsigned int min = (ticks / (div * 60)) % 60;
			unsigned int hr = (ticks / (div * 60 * 60)) % 24;
			unsigned int day = (ticks / (div * 60 * 60 * 24)) % 365;
			kprintf("%d ticks: day:hr:min:sec %3d:%02d:%02d:%02d\n", ticks, day, hr, min, sec);
		} else if(strcmp(command_buffer, "eg") == 0) {
			kprintf("Soph is a butt\n");
		} else if (strcmp(command_buffer, "time") == 0) {
			display_time();
		} else if (strcmp(command_buffer, "clear") == 0) {
			clear();
		} else if (strcmp(command_buffer, "butt") == 0) {
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
		} else {
			if(command_buffer[0]) {
				kprintf("%s: Command not found\n", command_buffer);
			}
		}
	}
}

noreturn void kernel_main(void) {
	// Get the parameters from the bootloader. The cursor position
	boot_params * params = get_boot_params(ADDRESS);
	
	/* Initialize terminal interface */
	terminal_initialise(params);
	
	gdt_init();
	
	idt_init();
	
	isr_init();
	
	irq_init();
	
	pit_install();
	
	keyboard_init();
	
	rtc_init();
	
	interrupt_enable();
	
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
