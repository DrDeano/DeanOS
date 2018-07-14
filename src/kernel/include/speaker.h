#ifndef INCLUDE_SPEAKER_H
#define INCLUDE_SPEAKER_H

#define PIT_CHANNEL_2	0x42
#define PIT_CMD_REG		0x43

void speaker_set(int hz);
void beep(unsigned int wait_time, unsigned int times);
void speaker_init();

#endif /* INCLUDE_SPEAKER_H */
