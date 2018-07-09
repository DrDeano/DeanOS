#include <speaker.h>
#include <portio.h>
#include <pit.h>

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
        pit_wait(wait_time * 18);
        out_port_byte(0x61, tempB);
		pit_wait(1 * 18);
   }
} 

void speaker_init() {
	//out_port_byte(0x61, 3 | (unsigned char) (1000<<2) );
	//speaker_set(1000);   
    //beep(2,2);
}
