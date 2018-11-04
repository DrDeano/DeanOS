#include <speaker.h>
#include <portio.h>
#include <pit.h>

#include <stdint.h>

/**
 * \todo Return error code from PIT
 */
static void speaker_start(uint32_t frequency) {
	pit_setup_counter(frequency, PIT_OCW_SELECT_COUNTER_2, PIT_OCW_MODE_SQUARE_WAVE_GENERATOR | PIT_OCW_BINARY_COUNT_BINARY);

	uint8_t temp = in_port_byte(SPEAKER_PORT_ADDRESS);
	if(temp != (temp | 0x03)) {
		out_port_byte(SPEAKER_PORT_ADDRESS, temp | 0x03);
	}
}

static void speaker_stop(void) {
	uint8_t temp = in_port_byte(SPEAKER_PORT_ADDRESS) & 0xFC;
	out_port_byte(SPEAKER_PORT_ADDRESS, temp);
}

void speaker_star_wars(void) {
	beep(440,500);
	pit_wait(250);
	beep(440,500);
	pit_wait(250);
	beep(440,500);
	pit_wait(250);
	beep(349,350);
	pit_wait(250);
	beep(523,150);
	pit_wait(250);
	beep(440,500);
	pit_wait(250);
	beep(349,350);
	pit_wait(250);
	beep(523,150);
	pit_wait(250);
	beep(440,1000);
	pit_wait(250);
	beep(659,500);
	pit_wait(250);
	beep(659,500);
	pit_wait(250);
	beep(659,500);
	pit_wait(250);
	beep(698,350);
	pit_wait(250);
	beep(523,150);
	pit_wait(250);
	beep(415,500);
	pit_wait(250);
	beep(349,350);
	pit_wait(250);
	beep(523,150);
	pit_wait(250);
	beep(440,1000);
}

void speaker_happy_birthday(void) {
	beep(264, 125);
	pit_wait(250);
	beep(264, 125);
	pit_wait(125);
	beep(297, 500);
	pit_wait(125);
	beep(264, 500);
	pit_wait(125);
	beep(352, 500);
	pit_wait(125);
	beep(330, 1000);
	pit_wait(250);
	beep(264, 125);
	pit_wait(250);
	beep(264, 125);
	pit_wait(125);
	beep(297, 500);
	pit_wait(125);
	beep(264, 500);
	pit_wait(125);
	beep(396, 500);
	pit_wait(125);
	beep(352, 1000);
	pit_wait(250);
	beep(264, 125);
	pit_wait(250);
	beep(264, 125);
	pit_wait(125);
	beep(264, 500);
	pit_wait(125);
	beep(440, 500);
	pit_wait(125);
	beep(352, 250);
	pit_wait(125);
	beep(352, 125);
	pit_wait(125);
	beep(330, 500);
	pit_wait(125);
	beep(297, 1000);
	pit_wait(250);
	beep(466, 125);
	pit_wait(250);
	beep(466, 125);
	pit_wait(125);
	beep(440, 500);
	pit_wait(125);
	beep(352, 500);
	pit_wait(125);
	beep(396, 500);
	pit_wait(125);
	beep(352, 1000);
}

void beep(uint32_t frequency, uint32_t play_duration) {
	if(frequency == 0) {
		return;
	}
	speaker_start(frequency);
	pit_wait(play_duration);
	speaker_stop();
}
