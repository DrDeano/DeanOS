/**
 * \file keyboard.h
 * \brief Functions, definitions and structures for setting up the keyboard driver.
 */
#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

/**
 * \brief The port address for reading the input buffer from the keyboard encoder.
 */
#define KEYBOARD_ENCODER_READ_INPUT_BUFFER			0x60

/**
 * \brief The port address for sending a command to the keyboard encoder.
 */
#define KEYBOARD_ENCODER_SEND_COMMAND				0x60

/**
 * \brief The port address for reading the status register of the keyboard controller.
 */
#define KEYBOARD_CONTROLLER_READ_STATUS_REGISTER	0x64

/**
 * \brief The port address for sending a command to the keyboard controller.
 */
#define KEYBOARD_CONTROLLER_SEND_COMMAND			0x64

/**
 * \brief The keyboard status registers masks.
 */
enum keyboard_status_register {
	KEYBOARD_STATUS_REGISTER_OUTPUT_BUFFER_MASK		= 0x01,	/**< xxxxxxx1 | Whether the output buffer is full. If set, then the buffer is full so can be read. If not set, then the buffer isn't full and shouldn't be read. */
	KEYBOARD_STATUS_REGISTER_INPUT_BUFFER_MASK		= 0x02,	/**< xxxxxx1x | Whether the input buffer is full. If set, then the buffer is full so don't write anything yet. If not set, then  the buffer isn't full and can be written to. */
	KEYBOARD_STATUS_REGISTER_SYSTEM_FLAGS_MASK		= 0x04,	/**< xxxxx1xx | The system flag that is set when a successful keyboard controller self test is completed. Is unset after a power on reset. */
	KEYBOARD_STATUS_REGISTER_COMMAND_DATA_MASK		= 0x08,	/**< xxxx1xxx | A flag saying whether the input buffer was data (via port 0x60) or a command (via port 0x64). If set, then was date, else is a command. */
	KEYBOARD_STATUS_REGISTER_KEYBOARD_LOCK_MASK		= 0x10,	/**< xxx1xxxx | Set if the keyboard is unlocked. If unset, then the keyboard is locked. */
	KEYBOARD_STATUS_REGISTER_AUX_OUTPUT_BUFFER_MASK	= 0x20,	/**< xx1xxxxx | Whether the auxiliary output buffer is full. This also depends on the mode the controller is operating in. For PS/2 systems: if set, then is mouse data if you can read from \ref KEYBOARD_ENCODER_READ_INPUT_BUFFER. If unset, then determines if read form port 0x60 is valid if value is 0. For AT systems: If set the OK flag. If unset, then time out of transmission from controller to keyboard. May indicate that no keyboard is present. */
	KEYBOARD_STATUS_REGISTER_TIMEOUT_MASK			= 0x40,	/**< x1xxxxxx | If set, then a time out occurred. If not set, then is a OK flag. For a PS/2 system, it is a general time out, for a AT system, it is a time out on transmission from keyboard to keyboard controller, possibly parity error. */
	KEYBOARD_STATUS_REGISTER_PARITY_ERROR_MASK		= 0x80	/**< 1xxxxxxx | If set, then there was a parity error with the last byte. If not set, then no error. */
};

/**
 * \brief The collection of all (I think) keys on a keyboard. Including multimedia keys.
 */
enum {
	KEYBOARD_KEY_A = 0x00,
	KEYBOARD_KEY_B,
	KEYBOARD_KEY_C,
	KEYBOARD_KEY_D,
	KEYBOARD_KEY_E,
	KEYBOARD_KEY_F,
	KEYBOARD_KEY_G,
	KEYBOARD_KEY_H,
	KEYBOARD_KEY_I,
	KEYBOARD_KEY_J,
	KEYBOARD_KEY_K,
	KEYBOARD_KEY_L,
	KEYBOARD_KEY_M,
	KEYBOARD_KEY_N,
	KEYBOARD_KEY_O,
	KEYBOARD_KEY_P,
	KEYBOARD_KEY_Q,
	KEYBOARD_KEY_R,
	KEYBOARD_KEY_S,
	KEYBOARD_KEY_T,
	KEYBOARD_KEY_U,
	KEYBOARD_KEY_V,
	KEYBOARD_KEY_W,
	KEYBOARD_KEY_X,
	KEYBOARD_KEY_Y,
	KEYBOARD_KEY_Z,
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
	
	KEYBOARD_KEY_ENTER,
	KEYBOARD_KEY_ESC,
	KEYBOARD_KEY_BACKSPACE,
	KEYBOARD_KEY_TAB,
	KEYBOARD_KEY_SPACE,
	KEYBOARD_KEY_GRAVE,
	KEYBOARD_KEY_HYPEN,
	KEYBOARD_KEY_EQUALS,
	KEYBOARD_KEY_OPEN_BRACKET,
	KEYBOARD_KEY_CLOSE_BRACKET,
	KEYBOARD_KEY_BACKSLASH,
	KEYBOARD_KEY_SEMICOLON,
	KEYBOARD_KEY_APOSTROPHE,
	KEYBOARD_KEY_COMMA,
	KEYBOARD_KEY_FULL_STOP,
	KEYBOARD_KEY_SLASH,
	//KEYBOARD_KEY_HASH,
	
	KEYBOARD_KEY_NUM_SLASH,
	KEYBOARD_KEY_NUM_STAR,
	KEYBOARD_KEY_NUM_MINUS,
	KEYBOARD_KEY_NUM_PLUS,
	KEYBOARD_KEY_NUM_ENTER,
	KEYBOARD_KEY_NUM_1,
	KEYBOARD_KEY_NUM_2,
	KEYBOARD_KEY_NUM_3,
	KEYBOARD_KEY_NUM_4,
	KEYBOARD_KEY_NUM_5,
	KEYBOARD_KEY_NUM_6,
	KEYBOARD_KEY_NUM_7,
	KEYBOARD_KEY_NUM_8,
	KEYBOARD_KEY_NUM_9,
	KEYBOARD_KEY_NUM_0,
	KEYBOARD_KEY_NUM_FULL_STOP,
	
	KEYBOARD_KEY_ARROW_UP,
	KEYBOARD_KEY_ARROW_LEFT,
	KEYBOARD_KEY_ARROW_DOWN,
	KEYBOARD_KEY_ARROW_RIGHT,
	
	KEYBOARD_KEY_INSERT,
	KEYBOARD_KEY_HOME,
	KEYBOARD_KEY_PAGE_UP,
	KEYBOARD_KEY_DELETE,
	KEYBOARD_KEY_END,
	KEYBOARD_KEY_PAGE_DOWN,
	
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
	KEYBOARD_KEY_F11,
	KEYBOARD_KEY_F12,
	
	KEYBOARD_KEY_NUM_LOCK,
	KEYBOARD_KEY_CAPS_LOCK,
	KEYBOARD_KEY_SCROLL_LOCK,
	
	KEYBOARD_KEY_LEFT_CTRL,
	KEYBOARD_KEY_LEFT_SHIFT,
	KEYBOARD_KEY_LEFT_ALT,
	KEYBOARD_KEY_RIGHT_CTRL,
	KEYBOARD_KEY_RIGHT_SHIFT,
	KEYBOARD_KEY_RIGHT_ALT,
	KEYBOARD_KEY_SYSREQ,
	
	// Extended codes
	KEYBOARD_KEY_MULTIMEDIA_PREVIOUS,
	KEYBOARD_KEY_MULTIMEDIA_NEXT,
	KEYBOARD_KEY_MULTIMEDIA_MUTE,
	KEYBOARD_KEY_MULTIMEDIA_CALCULATOR,
	KEYBOARD_KEY_MULTIMEDIA_PLAY,
	KEYBOARD_KEY_MULTIMEDIA_STOP,
	KEYBOARD_KEY_MULTIMEDIA_VOL_DOWN,
	KEYBOARD_KEY_MULTIMEDIA_VOL_UP,
	
	KEYBOARD_KEY_LEFT_GUI,
	KEYBOARD_KEY_RIGHT_GUI,
	KEYBOARD_KEY_APPS,
	
	KEYBOARD_KEY_POWER,
	KEYBOARD_KEY_SLEEP,
	KEYBOARD_KEY_WAKE,
	
	KEYBOARD_KEY_MULTIMEDIA_WWW_HOME,
	KEYBOARD_KEY_MULTIMEDIA_WWW_SEARCH,
	KEYBOARD_KEY_MULTIMEDIA_WWW_FAVORITES,
	KEYBOARD_KEY_MULTIMEDIA_WWW_REFRESH,
	KEYBOARD_KEY_MULTIMEDIA_WWW_STOP,
	KEYBOARD_KEY_MULTIMEDIA_WWW_FORWARD,
	KEYBOARD_KEY_MULTIMEDIA_WWW_BACK,
	
	KEYBOARD_KEY_MULTIMEDIA_MY_COMPUTER,
	KEYBOARD_KEY_MULTIMEDIA_EMAIL,
	KEYBOARD_KEY_MULTIMEDIA_MEDIA_SELECT,
	
	KEYBOARD_KEY_UNKNOWN
};

// ------------------
// Keyboard commands.
// ------------------

/**
 * \brief The command for setting the keyboard status lights for the scroll, number and caps lock.
 * Bit 0: Scroll lock (0: off, 1:on).
 * Bit 1: Number lock (0: off, 1:on).
 * Bit 2: Caps   lock (0: off, 1:on).
 */
#define KEYBOARD_ENCODER_COMMAND_SET_LED				0xED

/**
 * \brief This echo's back the same command, 0xEE to \ref KEYBOARD_ENCODER_READ_INPUT_BUFFER.
 * Usually used for testing.
 */
#define KEYBOARD_ENCODER_COMMAND_ECHO					0xEE

/**
 * \brief The command for setting a scan code set. The next byte written will be the scan code set.
 * This must be written to \ref KEYBOARD_ENCODER_SEND_COMMAND. The format for the scan code set are
 * as follows:\n
 * Bit 0: Returns the current scan code set in the input buffer, \ref KEYBOARD_ENCODER_READ_INPUT_BUFFER.\n
 * Bit 1: Sets scan code set 1.\n
 * Bit 2: Sets scan code set 2.\n
 * Bit 3: Sets scan code set 3.\n
 * All other bits must be zero.
 */
#define KEYBOARD_ENCODER_COMMAND_SET_SCAN_CODE_SET		0xF0

// -------------------
// Set scan code bits.
// -------------------

/**
 * \brief The bit for getting the current scan code set.
 */
#define GET_CURRENT_SCAN_CODE_SET	0x01	// 00000001

/**
 * \brief The bit for setting scan code set 1.
 */
#define SET_SCAN_CODE_SET_1			0x02	// 00000010

/**
 * \brief The bit for setting scan code set 2.
 */
#define SET_SCAN_CODE_SET_2			0x04	// 00000100

/**
 * \brief The bit for setting scan code set 3.
 */
#define SET_SCAN_CODE_SET_3			0x08	// 00001000

// -------------------

/**
 * \brief The command for reading the next 2 bytes from \ref KEYBOARD_ENCODER_READ_INPUT_BUFFER as
 * the keyboard ID 
 */
#define KEYBOARD_ENCODER_COMMAND_READ_KEYBOARD_ID		0xF2

/**
 * \brief The command for setting the autorepeat delay and repeat rate. Next byte written to \ref
 * KEYBOARD_ENCODER_SEND_COMMAND has the following format:\n
 * Bits 0 - 4: Repeat rate: 00000 (0x00) - 30 char/sec, 11111 (0x1F) - 2 char/sec.\n
 * Bits 5 - 6: Repeat delay: 00 - 0.25sec, 01 - 0.5sec, 10 - 0.75sec, 11 - 1sec.
 * Other bits must be zero.
 */
#define KEYBOARD_ENCODER_COMMAND_SET_AUTOREPEAT			0xF3

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_ENABLE_KEYBOARD		0xF4

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_RESET_POWER_ON_CONDITION_WAIT_ENABLE	0xF5

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_RESET_POWER_ON_CONDITION_BEGIN			0xF6

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_SET_ALL_KEYS_TO_AUTOREPEAT				0xF7

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_SET_ALL_KEYS_TO_SEND_MAKE_BREAK_CODES	0xF8

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_SET_ALL_KEYS_TO_MAKE_CODES				0xF9

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_SET_ALL_KEYS_TO_AUTOREPEAT_AND_SEND_MAKE_BREAK_CODES	0xFA

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_SET_A_KEY_TO_AUTOREPEAT				0xFB

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_SET_A_KEY_TO_SEND_MAKE_BREAK_CODES		0xFC

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_SET_A_KEY_TO_BREAK_CODES				0xFD

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_RESEND_LAST_RESTULT					0xFE

/**
 * \brief The command for 
 */
#define KEYBOARD_ENCODER_COMMAND_RESET_KEYBOARD_SELF_TEST				0xFF

// ----------------------
// Response/return codes.
// ----------------------

/**
 * \brief The response code for if the internal buffer overruns.
 */
#define KEYBOARD_ENCODER_RESPONSE_BUFFER_OVERRUN				0x00

// 0x01 - 0x58 0x81 - 0xD8 are the scan codes.

/**
 * \brief The response code for 
 */
#define KEYBOARD_ENCODER_RESPONSE_KEYBOARD_ID					0x83AB

/**
 * \brief The response code for 
 */
#define KEYBOARD_ENCODER_RESPONSE_SELF_TEST_PASSED				0x55

/**
 * \brief The response code for 
 */
#define KEYBOARD_ENCODER_RESPONSE_BASIC_ASSURANCE_TEST			0xAA

/**
 * \brief The response code for 
 */
#define KEYBOARD_ENCODER_RESPONSE_ECHO							0xEE

// 0xF0 prefix for some scan codes. Doesn't apply to PS/2.

/**
 * \brief The response code for 
 */
#define KEYBOARD_ENCODER_RESPONSE_ACKNONWLEDGE					0xFA

/**
 * \brief The response code for 
 */
#define KEYBOARD_ENCODER_RESPONSE_BASIC_ASSURANCE_TEST_FAILED	0xFC

/**
 * \brief The response code for 
 */
#define KEYBOARD_ENCODER_RESPONSE_DIAGONSTIC_FAILURE			0xFD

/**
 * \brief The response code for 
 */
#define KEYBOARD_ENCODER_RESPONSE_RESEND_LAST_COMMAND			0xFE

/**
 * \brief The response code for 
 */
#define KEYBOARD_ENCODER_RESPONSE_KET_ERROR						0xFF

// -----------------------------
// Keyboard controller commands.
// -----------------------------

// 0x00 - 0x3F: Read from controller RAM.

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_READ_COMMNAD				0x20

// 0x40 - 0x7F: Write to controller RAM.

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_WRITE_COMMAND				0x60

// 0x90 - 0x93: Synaptic multiplexer prefix

// 0x90 - 0x9F: Write port.

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_READ_COPYRIGHT				0xA0

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_READ_FIRMWARE_VERSION		0xA1

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_CHANGE_SPEED_1				0xA2

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_CHANGE_SPEED_2				0xA3

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_CHECK_PASSWORD_INSTALLED	0xA4

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_LOAD_PASSWORD				0xA5

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_CHECK_PASSWORD				0xA6

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_DISABLE_MOUSE_PORT			0xA7

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_ENABLE_MOUSE_PORT			0xA8

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_TEST_MOUSE_PORT				0xA9

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_SELF_TEST					0xAA

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_INTERFACE_TEST				0xAB

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_DIAGNOSTIC_DUMP				0xAC

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_DISABLE_KEYBOARD			0xAD

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_ENABLE_KEYBOARD				0xAE

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_READ_KEYBOARD_VERSION		0xAF

// 0xB0 - 0xB5: Reset controller line

// 0xB6 - 0xBD: Set controller line

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_READ_INPUT_PORT				0xC0

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_CONTINUOUS_INPUT_PORT_POLL_LOW	0xC1

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_CONTINUOUS_INPUT_PORT_POLL_HIGH	0xC2

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_UNBLOCK_CONTOLLER_LINE_P22_P23	0xC8

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_BLOCK_CONTOLLER_LINE_P22_P23	0xC9

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_READ_CONTROLLER_MODE		0xCA

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_WRITE_CONTROLLER_MODE		0xCB

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_READ_OUTPUT_PORT			0xD0

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_WRITE_OUTPUT_PORT			0xD1

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_WRITE_OUTPUT_BUFFER			0xD2

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_WRITE_MOUSE_OUTPUT_BUFFER	0xD3

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_WRITE_TO_MOUSE				0xD4

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_DISABLE_A20					0xDD

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_ENABLE_A20					0xDF

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_READ_TEST_INPUT				0xE0

// 0xF0 - 0xFF: Pulse output bit.

/**
 * \brief The command for 
 */
#define KEYBOARD_CONTROLLER_COMMAND_SYSTEM_RESET				0xFE

/**
 * \brief Disable the keyboard.
 */
void disable_keyboard(void);

/**
 * \brief Enable the keyboard.
 */
void enable_keyboard(void);

/**
 * \todo Code this
 */
uint8_t keyboard_read_input_port(void);

/**
 * \todo Code this
 */
uint8_t keyboard_read_output_port(void);

/**
 * \todo Code this
 */
void keyboard_write_output_port(uint8_t port);

/**
 * \todo Code this
 */
uint8_t keyboard_read_test_input(void);

/**
 * \brief Resets the CPU by pulsing bit 0 of the controllers output port, pin P0. This resets the
 * system nicely. This may not work on all systems, so can test if is the program is still running
 * after calling this function.
 */
void keyboard_system_reset(void);

/**
 * \brief Preform the keyboard self test.
 * 
 * \return Returns true if the test passed, else returns false.
 */
bool keyboard_self_test(void);

/**
 * \brief Perform the keyboard interface test. If the test fails, then is recommended to disable
 * the keyboard and reset it and test it again. If fails again. then the keyboard could be
 * malfunctioned and not be used.
 * 
 * \return Returns true if the test passed, else returns false.
 */
bool keyboard_interface_test(void);

/**
 * \brief Read the status register from the keyboard controller.
 */
uint8_t keyboard_controller_read_status(void);

/**
 * \brief Send a command to the keyboard controller. This waits until the keyboard is ready to
 * receive the command then sends it.
 * 
 * \param [in] cmd The command to send.
 */
void keyboard_controller_send_command(const uint8_t cmd);

/**
 * \brief Read the input buffer of the keyboard encoder.
 * 
 * \return The data in the input buffer of the keyboard encoder.
 */
uint8_t kerybaord_encoder_read_input(void);

/**
 * \brief Send a command to the keyboard encoder. This first it sent to the keyboard controller so
 * will need to wait until it is ready to receive the command.
 * 
 * \param [in] cmd The command to send to the keyboard encoder.
 */
void kerybaord_encoder_send_command(const uint8_t cmd);

/**
 * \brief Given a keyboard key, return the ASCII value of the key.
 * 
 * \param [in] key The keyboard key to be translated.
 * \return The ASCII value of the keyboard key.
 */
char key_to_ascii(unsigned char key);

/**
 * \brief Get the last key that was pressed. This resets the last key pressed to \ref
 * KEYBOARD_KEY_UNKNOWN.
 * 
 * \return The last key that's was pressed.
 */
unsigned char get_last_key_press(void);

/**
 * \brief Waits for a key to be pressed. Waits until the key isn't \ref KEYBOARD_KEY_UNKNOWN.
 * 
 * \return The last key press that isn't \ref KEYBOARD_KEY_UNKNOWN.
 */
unsigned char wait_for_key_press(void);

/**
 * \brief Initiate the keyboard driver to accept key presses and handle them.
 */
void keyboard_init(void);

#endif /* INCLUDE_KEYBOARD_H */
