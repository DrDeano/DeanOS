#include <keyboard.h>
#include <ps2.h>
#include <pic.h>
#include <regs_t.h>
#include <stdio.h>
#include <irq.h>
#include <portio.h>

#include <stdbool.h>

/**
 *  A structure for storing the key, its ASCII value, (if has one), and the upper case ASCII value
 * (if has one). If hasn't got an upper case ASCII value, then is the same as the lower case value.
 */
typedef struct {
    const uint8_t key;
    const unsigned char ascii;
    const unsigned char shift_ascii;
} key_mapping;

/**
 *  The last key that was pressed. Then is reset to KEYBOARD_KEY_UNKNOWN once accessed.
 */
static unsigned char last_key_press;

/**
 *  Is the shift key pressed.
 */
static bool shift_pressed;

/**
 *  Is the control key pressed.
 */
static bool ctrl_pressed;

/**
 *  Is the alt key pressed.
 */
static bool alt_pressed;

/**
 *  Is the caps lock on.
 */
static bool caps_lock_toggle;

/**
 *  Is the number lock on.
 */
static bool num_lock_toggle;

/**
 *  Is the scroll lock on.
 */
static bool scroll_lock_toggle;

/**
 *  Is the key being pressed part of the extended range of scan codes.
 */
static bool is_extended;

/**
 *  The key map of set 1 of scan codes.
 */
unsigned char keymap[256] = {
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_ESC,
	KEYBOARD_KEY_1,
	KEYBOARD_KEY_2,
	KEYBOARD_KEY_3,
	KEYBOARD_KEY_4,
	KEYBOARD_KEY_5,
	KEYBOARD_KEY_6,
	KEYBOARD_KEY_7,
	KEYBOARD_KEY_8,
	KEYBOARD_KEY_9,
	KEYBOARD_KEY_0,
	KEYBOARD_KEY_HYPEN,
	KEYBOARD_KEY_EQUALS,
	KEYBOARD_KEY_BACKSPACE,
	
	KEYBOARD_KEY_TAB,
	KEYBOARD_KEY_Q,
	KEYBOARD_KEY_W,
	KEYBOARD_KEY_E,
	KEYBOARD_KEY_R,
	KEYBOARD_KEY_T,
	KEYBOARD_KEY_Y,
	KEYBOARD_KEY_U,
	KEYBOARD_KEY_I,
	KEYBOARD_KEY_O,
	KEYBOARD_KEY_P,
	KEYBOARD_KEY_OPEN_BRACKET,
	KEYBOARD_KEY_CLOSE_BRACKET,
	KEYBOARD_KEY_ENTER,
	
	KEYBOARD_KEY_LEFT_CTRL,
	
	KEYBOARD_KEY_A,
	KEYBOARD_KEY_S,
	KEYBOARD_KEY_D,
	KEYBOARD_KEY_F,
	KEYBOARD_KEY_G,
	KEYBOARD_KEY_H,
	KEYBOARD_KEY_J,
	KEYBOARD_KEY_K,
	KEYBOARD_KEY_L,
	KEYBOARD_KEY_SEMICOLON,
	KEYBOARD_KEY_APOSTROPHE,
	KEYBOARD_KEY_GRAVE,
	
	KEYBOARD_KEY_LEFT_SHIFT,
	KEYBOARD_KEY_BACKSLASH,
	KEYBOARD_KEY_Z,
	KEYBOARD_KEY_X,
	KEYBOARD_KEY_C,
	KEYBOARD_KEY_V,
	KEYBOARD_KEY_B,
	KEYBOARD_KEY_N,
	KEYBOARD_KEY_M,
	KEYBOARD_KEY_COMMA,
	KEYBOARD_KEY_FULL_STOP,
	KEYBOARD_KEY_SLASH,
	KEYBOARD_KEY_RIGHT_SHIFT,
	
	KEYBOARD_KEY_NUM_STAR,
	KEYBOARD_KEY_LEFT_ALT,
	KEYBOARD_KEY_SPACE,
	KEYBOARD_KEY_CAPS_LOCK,
	KEYBOARD_KEY_F1,
	KEYBOARD_KEY_F2,
	KEYBOARD_KEY_F3,
	KEYBOARD_KEY_F4,
	KEYBOARD_KEY_F5,
	KEYBOARD_KEY_F6,
	KEYBOARD_KEY_F7,
	KEYBOARD_KEY_F8,
	KEYBOARD_KEY_F9,
	KEYBOARD_KEY_F10,
	KEYBOARD_KEY_NUM_LOCK,
	KEYBOARD_KEY_SCROLL_LOCK,
	
	KEYBOARD_KEY_NUM_7,
	KEYBOARD_KEY_NUM_8,
	KEYBOARD_KEY_NUM_9,
	KEYBOARD_KEY_NUM_MINUS,
	KEYBOARD_KEY_NUM_4,
	KEYBOARD_KEY_NUM_5,
	KEYBOARD_KEY_NUM_6,
	KEYBOARD_KEY_NUM_PLUS,
	KEYBOARD_KEY_NUM_1,
	KEYBOARD_KEY_NUM_2,
	KEYBOARD_KEY_NUM_3,
	KEYBOARD_KEY_NUM_0,
	KEYBOARD_KEY_NUM_FULL_STOP,
	KEYBOARD_KEY_SYSREQ,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_F11,
	KEYBOARD_KEY_F12,									// 0x58
	
	// Extended section
	KEYBOARD_KEY_UNKNOWN,				// 0xE0 0x00
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,								// 0x60
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,								// 0x68
	
	KEYBOARD_KEY_MULTIMEDIA_PREVIOUS,	// 0xE0 0x10
	
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,								// 0x70
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_MULTIMEDIA_NEXT,
	
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_NUM_ENTER,
	KEYBOARD_KEY_RIGHT_CTRL,
	
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_MULTIMEDIA_MUTE,		// 0xE0 0x20
	KEYBOARD_KEY_MULTIMEDIA_CALCULATOR,
	KEYBOARD_KEY_MULTIMEDIA_PLAY,
	
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_MULTIMEDIA_STOP,
	
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,								// 0x80
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_MULTIMEDIA_VOL_DOWN,
	
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_MULTIMEDIA_VOL_UP,		// 0xE0 0x30
	
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_MULTIMEDIA_WWW_HOME,
	
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_NUM_SLASH,
	
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,								// 0x90
	
	KEYBOARD_KEY_RIGHT_ALT,
	
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,				// 0xE0 0x40
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_HOME,									// 0xA0
	KEYBOARD_KEY_ARROW_UP,
	KEYBOARD_KEY_PAGE_UP,
	
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_ARROW_LEFT,
	
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_ARROW_RIGHT,
	
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_END,
	KEYBOARD_KEY_ARROW_DOWN,			// 0xE0 0x50
	KEYBOARD_KEY_PAGE_DOWN,
	KEYBOARD_KEY_INSERT,
	KEYBOARD_KEY_DELETE,
	
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,								// 0xB0
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_LEFT_GUI,
	KEYBOARD_KEY_RIGHT_GUI,
	KEYBOARD_KEY_APPS,
	KEYBOARD_KEY_POWER,
	KEYBOARD_KEY_SLEEP,
	
	KEYBOARD_KEY_UNKNOWN,				// 0xE0 0x60
	KEYBOARD_KEY_UNKNOWN,
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_WAKE,
	
	KEYBOARD_KEY_UNKNOWN,
	
	KEYBOARD_KEY_MULTIMEDIA_WWW_SEARCH,
	KEYBOARD_KEY_MULTIMEDIA_WWW_FAVORITES,
	KEYBOARD_KEY_MULTIMEDIA_WWW_REFRESH,				// 0xC0
	KEYBOARD_KEY_MULTIMEDIA_WWW_STOP,
	KEYBOARD_KEY_MULTIMEDIA_WWW_FORWARD,
	KEYBOARD_KEY_MULTIMEDIA_WWW_BACK,
	KEYBOARD_KEY_MULTIMEDIA_MY_COMPUTER,
	KEYBOARD_KEY_MULTIMEDIA_EMAIL,
	KEYBOARD_KEY_MULTIMEDIA_MEDIA_SELECT	// 0xE0 0x6D
};

/**
 *  The array of scan code to ASCII key map.
 */
static const key_mapping key_to_ascii_map[] = {
    // Letters.
    [KEYBOARD_KEY_A] = { KEYBOARD_KEY_A, 'a', 'A' },
    [KEYBOARD_KEY_B] = { KEYBOARD_KEY_B, 'b', 'B' },
    [KEYBOARD_KEY_C] = { KEYBOARD_KEY_C, 'c', 'C' },
    [KEYBOARD_KEY_D] = { KEYBOARD_KEY_D, 'd', 'D' },
    [KEYBOARD_KEY_E] = { KEYBOARD_KEY_E, 'e', 'E' },
    [KEYBOARD_KEY_F] = { KEYBOARD_KEY_F, 'f', 'F' },
    [KEYBOARD_KEY_G] = { KEYBOARD_KEY_G, 'g', 'G' },
    [KEYBOARD_KEY_H] = { KEYBOARD_KEY_H, 'h', 'H' },
    [KEYBOARD_KEY_I] = { KEYBOARD_KEY_I, 'i', 'I' },
    [KEYBOARD_KEY_J] = { KEYBOARD_KEY_J, 'j', 'J' },
    [KEYBOARD_KEY_K] = { KEYBOARD_KEY_K, 'k', 'K' },
    [KEYBOARD_KEY_L] = { KEYBOARD_KEY_L, 'l', 'L' },
    [KEYBOARD_KEY_M] = { KEYBOARD_KEY_M, 'm', 'M' },
    [KEYBOARD_KEY_N] = { KEYBOARD_KEY_N, 'n', 'N' },
    [KEYBOARD_KEY_O] = { KEYBOARD_KEY_O, 'o', 'O' },
    [KEYBOARD_KEY_P] = { KEYBOARD_KEY_P, 'p', 'P' },
    [KEYBOARD_KEY_Q] = { KEYBOARD_KEY_Q, 'q', 'Q' },
    [KEYBOARD_KEY_R] = { KEYBOARD_KEY_R, 'r', 'R' },
    [KEYBOARD_KEY_S] = { KEYBOARD_KEY_S, 's', 'S' },
    [KEYBOARD_KEY_T] = { KEYBOARD_KEY_T, 't', 'T' },
    [KEYBOARD_KEY_U] = { KEYBOARD_KEY_U, 'u', 'U' },
    [KEYBOARD_KEY_V] = { KEYBOARD_KEY_V, 'v', 'V' },
    [KEYBOARD_KEY_W] = { KEYBOARD_KEY_W, 'w', 'W' },
    [KEYBOARD_KEY_X] = { KEYBOARD_KEY_X, 'x', 'X' },
    [KEYBOARD_KEY_Y] = { KEYBOARD_KEY_Y, 'y', 'Y' },
    [KEYBOARD_KEY_Z] = { KEYBOARD_KEY_Z, 'z', 'Z' },

    // Numbers.
    [KEYBOARD_KEY_1] = { KEYBOARD_KEY_1, '1', '!' },
    [KEYBOARD_KEY_2] = { KEYBOARD_KEY_2, '2', '@' },
    [KEYBOARD_KEY_3] = { KEYBOARD_KEY_3, '3', '#'/*'£'*/ },
    [KEYBOARD_KEY_4] = { KEYBOARD_KEY_4, '4', '$' },
    [KEYBOARD_KEY_5] = { KEYBOARD_KEY_5, '5', '%' },
    [KEYBOARD_KEY_6] = { KEYBOARD_KEY_6, '6', '^' },
    [KEYBOARD_KEY_7] = { KEYBOARD_KEY_7, '7', '&' },
    [KEYBOARD_KEY_8] = { KEYBOARD_KEY_8, '8', '*' },
    [KEYBOARD_KEY_9] = { KEYBOARD_KEY_9, '9', '(' },
    [KEYBOARD_KEY_0] = { KEYBOARD_KEY_0, '0', ')' },

    [KEYBOARD_KEY_ENTER]			= { KEYBOARD_KEY_ENTER,			'\n', '\n' },
    [KEYBOARD_KEY_ESC]				= { KEYBOARD_KEY_ESC,			'\e', '\e' },
    [KEYBOARD_KEY_BACKSPACE]		= { KEYBOARD_KEY_BACKSPACE,		'\b', '\b' },
    [KEYBOARD_KEY_TAB]				= { KEYBOARD_KEY_TAB,			'\t', '\t' },
    [KEYBOARD_KEY_SPACE]			= { KEYBOARD_KEY_SPACE,			' ',  ' ' },
    [KEYBOARD_KEY_GRAVE]			= { KEYBOARD_KEY_GRAVE,			'`',  '~'/*'¬'*/ },
    [KEYBOARD_KEY_HYPEN]			= { KEYBOARD_KEY_HYPEN,			'-',  '_' },
    [KEYBOARD_KEY_EQUALS]			= { KEYBOARD_KEY_EQUALS,		'=',  '+' },
    [KEYBOARD_KEY_OPEN_BRACKET]		= { KEYBOARD_KEY_OPEN_BRACKET,	'[',  '{' },
    [KEYBOARD_KEY_CLOSE_BRACKET]	= { KEYBOARD_KEY_CLOSE_BRACKET,	']',  '}' },
    [KEYBOARD_KEY_BACKSLASH]		= { KEYBOARD_KEY_BACKSLASH,		'\\', '|' },
    [KEYBOARD_KEY_SEMICOLON]		= { KEYBOARD_KEY_SEMICOLON,		';',  ':' },
    [KEYBOARD_KEY_APOSTROPHE]		= { KEYBOARD_KEY_APOSTROPHE,	'\'', '"' },
    [KEYBOARD_KEY_COMMA]			= { KEYBOARD_KEY_COMMA,			',',  '<' },
    [KEYBOARD_KEY_FULL_STOP]		= { KEYBOARD_KEY_FULL_STOP,		'.',  '>' },
    [KEYBOARD_KEY_SLASH]			= { KEYBOARD_KEY_SLASH,			'/',  '?' },

    // Num pad.
    [KEYBOARD_KEY_NUM_SLASH]		= { KEYBOARD_KEY_NUM_SLASH,		'/', '/' },
    [KEYBOARD_KEY_NUM_STAR]			= { KEYBOARD_KEY_NUM_STAR,		'*', '*' },
    [KEYBOARD_KEY_NUM_MINUS]		= { KEYBOARD_KEY_NUM_MINUS,		'-', '-' },
    [KEYBOARD_KEY_NUM_PLUS]			= { KEYBOARD_KEY_NUM_PLUS,		'+', '+' },
    [KEYBOARD_KEY_NUM_ENTER]		= { KEYBOARD_KEY_NUM_ENTER,		'\n', '\n' },
    [KEYBOARD_KEY_NUM_1]			= { KEYBOARD_KEY_NUM_1,			'E', '1' },
    [KEYBOARD_KEY_NUM_2]			= { KEYBOARD_KEY_NUM_2,			'D', '2' },
    [KEYBOARD_KEY_NUM_3]			= { KEYBOARD_KEY_NUM_3,			'P', '3' },
    [KEYBOARD_KEY_NUM_4]			= { KEYBOARD_KEY_NUM_4,			'L', '4' },
    [KEYBOARD_KEY_NUM_5]			= { KEYBOARD_KEY_NUM_5,			  0, '5' },
    [KEYBOARD_KEY_NUM_6]			= { KEYBOARD_KEY_NUM_6,			'R', '6' },
    [KEYBOARD_KEY_NUM_7]			= { KEYBOARD_KEY_NUM_7,			'H', '7' },
    [KEYBOARD_KEY_NUM_8]			= { KEYBOARD_KEY_NUM_8,			'U', '8' },
    [KEYBOARD_KEY_NUM_9]			= { KEYBOARD_KEY_NUM_9,			'P', '9' },
    [KEYBOARD_KEY_NUM_0]			= { KEYBOARD_KEY_NUM_0,			'I', '0' },
    [KEYBOARD_KEY_NUM_FULL_STOP]	= { KEYBOARD_KEY_NUM_FULL_STOP, 'D', '.' },
};

/**
 *  \brief Set the keyboard caps, num and scroll lock lights depending whether the keys have been
 *  pressed/toggled.
 *  
 *  \param [in] scroll_lock Whether the scroll lock is on.
 *  \param [in] num_lock    Whether the number lock is on.
 *  \param [in] caps_lock   Whether the caps lock is on.
 */
static void set_keyboard_lights(bool scroll_lock, bool num_lock, bool caps_lock) {
	uint8_t data = (scroll_lock << 3) | (num_lock << 2) | caps_lock;
	ps2_send_command(KEYBOARD_SET_LED);
	ps2_send_command(data);
}

/**
 *  \brief The IRQ handler for the keyboard that reads the scan code from the keyboards buffer and
 *  translates it into a key press or release. A held down key will generate repeated interrupts.
 *  
 *  \param [in] regs The register of the handler.
 */
void keyboard_handler(regs_t * regs) {
	(void) regs;				// Registers aren't needed.
	
	unsigned char scancode;
	unsigned char key_pressed;

    // Read from the keyboard's data buffer
	scancode = in_port_byte(0x60);
	
	// Handle extended codes for home, insert, end, ...
	// https://wiki.osdev.org/PS/2_Keyboard
	if (scancode == 0xe0 || scancode == 0xe1) {
		is_extended = true;
		return;
	}
	
	if (is_extended) {
		key_pressed = keymap[(scancode & ~0x80) + 0x59];
	} else {
		key_pressed = keymap[(scancode & ~0x80)];
	}
	
	is_extended = false;

    // If the top bit of the byte we read from the keyboard is set, then the key has just been released
	if (scancode & 0x80) {
        switch (key_pressed) {
			case KEYBOARD_KEY_LEFT_SHIFT:
			case KEYBOARD_KEY_RIGHT_SHIFT:
				shift_pressed = false;
				break;
			
			case KEYBOARD_KEY_LEFT_CTRL:
			case KEYBOARD_KEY_RIGHT_CTRL:
				ctrl_pressed = false;
				break;
			
			case KEYBOARD_KEY_LEFT_ALT:
			case KEYBOARD_KEY_RIGHT_ALT:
				alt_pressed = false;
				break;
			
			default:
				break;
		}
	} else {
		switch (key_pressed) {
			case KEYBOARD_KEY_SCROLL_LOCK:
				scroll_lock_toggle = !scroll_lock_toggle;
				set_keyboard_lights(scroll_lock_toggle, num_lock_toggle, caps_lock_toggle);
				break;
			
			case KEYBOARD_KEY_NUM_LOCK:
				num_lock_toggle = !num_lock_toggle;
				set_keyboard_lights(scroll_lock_toggle, num_lock_toggle, caps_lock_toggle);
				break;
			
			case KEYBOARD_KEY_CAPS_LOCK:
				caps_lock_toggle = !caps_lock_toggle;
				set_keyboard_lights(scroll_lock_toggle, num_lock_toggle, caps_lock_toggle);
				break;
			
			case KEYBOARD_KEY_LEFT_SHIFT:
			case KEYBOARD_KEY_RIGHT_SHIFT:
				shift_pressed = true;
				break;
			
			case KEYBOARD_KEY_LEFT_CTRL:
			case KEYBOARD_KEY_RIGHT_CTRL:
				ctrl_pressed = true;
				break;
			
			case KEYBOARD_KEY_LEFT_ALT:
			case KEYBOARD_KEY_RIGHT_ALT:
				alt_pressed = true;
				break;
		}
		
		last_key_press = key_pressed;
	}
}

char key_to_ascii(unsigned char key) {
	// Has the key got a ASCII value
	if(key > KEYBOARD_KEY_NUM_FULL_STOP) {
		return 0;
	}
	
	// If shift key is held, then return the upper ASCII value
	if(shift_pressed) {
		return key_to_ascii_map[key].shift_ascii;
	}
	
	// Else return the lower case ASCII value.
	return key_to_ascii_map[key].ascii;
}

unsigned char get_last_key_press() {
	unsigned char ret = last_key_press;
	last_key_press = KEYBOARD_KEY_UNKNOWN;
	return ret;
}

void keyboard_init() {
	// Installs 'keyboard_handler' to IRQ1
	last_key_press = KEYBOARD_KEY_UNKNOWN;
	
	shift_pressed = false;
	ctrl_pressed = false;
	alt_pressed = false;

	caps_lock_toggle = false;
	num_lock_toggle = false;
	scroll_lock_toggle = false;
	
	is_extended = false;
	
	irq_install_handler(PIC_IRQ_KEYBOARD, keyboard_handler);
}
