#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <kernel_task.h>
#include <tty.h>
#include <rtc.h>
#include <floppy.h>
#include <speaker.h>
#include <keyboard.h>
#include <pit.h>

static char prev_command_buffer[10][64] = {0};	/**<  */
static int prev_command_buffer_end = 0;			/**<  */
static int prev_command_buffer_index = 0;		/**<  */

static void display_time(void) {
	static char * str_day[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	rtc_date_time_t date;
	tty_get_time(&date);
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

static void zero_cmd_buffer(char * command_buffer, int * command_buffer_index) {
	memset(command_buffer, 0, 64);
	*command_buffer_index = 0;
}

static void move_left(int * command_buffer_index) {
	if(*command_buffer_index) {
		tty_move_cursor_left();
		(*command_buffer_index)--;
	}
}

static void move_right(char * command_buffer, int * command_buffer_index) {
	if(command_buffer[*command_buffer_index] != '\0') {
		tty_move_cursor_right();
		(*command_buffer_index)++;
	}
}

static void clear_line(char * command_buffer, int * command_buffer_index) {
	//int temp = command_buffer_index;
	
	// Start from the end
	for(unsigned int i = *command_buffer_index; i < strlen(command_buffer); i++) {
		move_right(command_buffer, command_buffer_index);
		(*command_buffer_index)--;
	}
	
	// Clear the line
	for(unsigned int i = 0; i < strlen(command_buffer); i++) {
		kputchar('\b');
		kputchar('\0');
		kputchar('\b');
	}
	
	// Move back to where started
	//command_buffer_index = temp;
}

static void add_char_to_cmd(char c, char * command_buffer, int * command_buffer_index) {
	// CHECK OVER FLOW
	clear_line(command_buffer, command_buffer_index);
	
	if(c == '\b') {
		if(*command_buffer_index) {
			memmove(command_buffer + *command_buffer_index - 1, command_buffer + *command_buffer_index, strlen(command_buffer + *command_buffer_index));
			
			(*command_buffer_index)--;
			
			// Set last item to null
			command_buffer[strlen(command_buffer) - 1] = '\0';
		}
		
		goto print;
	} else if(c == '\t') {
		
		if(*command_buffer_index + 4 > 62) {
			goto print;
		}
		
		if(command_buffer[*command_buffer_index] != '\0') {
			memmove(command_buffer + *command_buffer_index + 4, command_buffer + *command_buffer_index, strlen(command_buffer + *command_buffer_index));
		}
		command_buffer[(*command_buffer_index)++] = ' ';
		command_buffer[(*command_buffer_index)++] = ' ';
		command_buffer[(*command_buffer_index)++] = ' ';
		command_buffer[(*command_buffer_index)++] = ' ';
		goto print;
	}
	
	// if(*command_buffer_index + 1 > 62)
	if(*command_buffer_index > 61) {
		goto print;
	}
	
	// If half way into a word
	if(command_buffer[*command_buffer_index] != '\0') {
		// Move all chars to right of the index one to the right
		memmove(command_buffer + *command_buffer_index + 1, command_buffer + *command_buffer_index, strlen(command_buffer + *command_buffer_index));
	}
	
	// Add the char
	command_buffer[(*command_buffer_index)++] = c;
	
	print:
	
	kprintf("%s", command_buffer);
	
	for(unsigned int i = *command_buffer_index; i < strlen(command_buffer); i++) {
		(*command_buffer_index)++;
		move_left(command_buffer_index);
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

static void get_command(char * command_buffer) {
	char ch;
	char * prev_cmd;
	bool end_of_cmd = false;
	char * help = "help";
	
	//char command_buffer[64] = {0};
	int command_buffer_index = 0;
	
	while(command_buffer_index < 62 && !end_of_cmd) {
			// unsigned char key = get_key();
			unsigned char key = wait_for_key_press();
			switch (key) {
				case KEYBOARD_KEY_ENTER:
					end_of_cmd = true;
					break;
					
				case KEYBOARD_KEY_ESC:
					//kprintf("ESC\n");
					break;
					
				case KEYBOARD_KEY_F1:
					zero_cmd_buffer(command_buffer, &command_buffer_index);
					clear_line(command_buffer, &command_buffer_index);
					for(int i = 0; i < 5; i++) {
						add_char_to_cmd(help[i], command_buffer, &command_buffer_index);
					}
					end_of_cmd = true;
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
					tty_page_up();
					break;
					
				case KEYBOARD_KEY_DELETE:
					//kprintf("DELETE\n");
					break;
					
				case KEYBOARD_KEY_END:
					//kprintf("END\n");
					break;
					
				case KEYBOARD_KEY_PAGE_DOWN:
					tty_page_down();
					break;
					
				case KEYBOARD_KEY_ARROW_UP:
					clear_line(command_buffer, &command_buffer_index);
					zero_cmd_buffer(command_buffer, &command_buffer_index);
					
					prev_cmd = get_prev_cmd();
					
					kprintf("%s", prev_cmd);
					for(unsigned int i = 0; i < strlen(prev_cmd); i++) {
						command_buffer[command_buffer_index++] = prev_cmd[i];
					}
					break;
					
				case KEYBOARD_KEY_ARROW_DOWN:
					clear_line(command_buffer, &command_buffer_index);
					zero_cmd_buffer(command_buffer, &command_buffer_index);
					
					prev_cmd = get_next_cmd();
					
					kprintf("%s", prev_cmd);
					for(unsigned int i = 0; i < strlen(prev_cmd); i++) {
						command_buffer[command_buffer_index++] = prev_cmd[i];
					}
					break;
					
				case KEYBOARD_KEY_ARROW_LEFT:
					move_left(&command_buffer_index);
					break;
					
				case KEYBOARD_KEY_ARROW_RIGHT:
					move_right(command_buffer, &command_buffer_index);
					break;
					
				case KEYBOARD_KEY_BACKSPACE:
					if (command_buffer_index > 0) {
						add_char_to_cmd('\b', command_buffer, &command_buffer_index);
					}
					break;
					
				case KEYBOARD_KEY_TAB:
					add_char_to_cmd('\t', command_buffer, &command_buffer_index);
					break;
					
				default:
					ch = key_to_ascii(key);
					if(ch) {
						add_char_to_cmd(ch, command_buffer, &command_buffer_index);
					}
					break;
			}
		}
		
		//end_of_cmd = false;
		kputchar('\n');
}

void kernel_task(void) {
	const int num_commands = 9;
	const char * list_of_commands[] = {
		"help",
		"hello",
		"butt",
		"eg",
		"time",
		"uptime",
		"clear",
		"read",
		"beep"
	};
	
	char command_buffer[64] = {0};
	int command_buffer_index = 0;
	
	kprintf("kernel task started\n");
	
	while(true) {
		zero_cmd_buffer(command_buffer, &command_buffer_index);
		kprintf("terminal:>");
		
		get_command(command_buffer);
		
		add_command(command_buffer);
		
		if(strcmp(command_buffer, "help") == 0) {
			kprintf("List of commands:\n");
			for(int i = 0; i < num_commands; i++) {
				kprintf("\t- %s\n", list_of_commands[i]);
			}
		} else if(strcmp(command_buffer, "hello") == 0) {
			kprintf("Hello there\n");
		} else if(strcmp(command_buffer, "uptime") == 0) {
			unsigned int ticks = pit_get_ticks();
			unsigned int freq = pit_get_frequency();
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
			tty_clear();
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
			kprintf("Enter the logical bock address to read from: ");
			char lba[64] = {0};
			get_command(lba);
			kprintf("string number get: %s\n", lba);
			uint32_t lba_int = atoi(lba);
			kprintf("lba number: %d\n", lba_int);
			uint8_t * sector = floppy_read_sector(lba_int);
			if(sector) {
				for(int i = 1; i <= 512; i++) {
					kprintf("0x%2X ", (uint32_t) sector[i - 1]);
					if(i % 15 == 0 && i != 1) {
						kprintf("\n");
					}
				}
				kprintf("\n");
			} else {
				kprintf("Error reading\n");
			}
		} else if(strcmp(command_buffer, "beep") == 0) {
			beep(400, 150);
			// speaker_happy_birthday();
			// speaker_star_wars();
		} else {
			if(command_buffer[0]) {
				kprintf("%s: Command not found\n", command_buffer);
			}
		}
	}
}

