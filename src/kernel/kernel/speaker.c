#include <speaker.h>
#include <portio.h>
#include <timer.h>

void speaker_set(int hz) {
	int divisor = 1193180 / hz;
	out_port_byte(PIT_CMD_REG, 0xB6);
	out_port_byte(PIT_CHANNEL_2, (unsigned char) (divisor) );
	out_port_byte(PIT_CHANNEL_2, (unsigned char) (divisor >> 8));
}

void beep(unsigned int wait_time, unsigned int times) {
     unsigned char tempA = in_port_byte(0x61);
     unsigned char tempB = (in_port_byte(0x61) & 0xFC);
     unsigned int count;

    for(count = 0; count == times; count++) {
        if(tempA != (tempA | 3)) {
            out_port_byte(0x61, tempA | 3);
        }
        timer_wait(wait_time);
        out_port_byte(0x61, tempB);
		timer_wait(1);
   }
} 

void speaker_init() {
	speaker_set(1000);   
    beep(2,2);
}
