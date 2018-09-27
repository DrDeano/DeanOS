/**
 *  \file speaker.h
 *  \brief Functions and definitions for using the PC speakers.
 */
#ifndef INCLUDE_SPEAKER_H
#define INCLUDE_SPEAKER_H

#include <stdint.h>

/**
 *  \brief The port addresses of the speaker.
 */
#define SPEAKER_PORT_ADDRESS	0x61

/**
 *  \brief Play the happy birthday song.
 */
void speaker_happy_birthday(void);

/**
 *  \brief Play the star was song.
 */
void speaker_star_wars(void);

/**
 *  \brief Play a sound with a frequency and duration with the PC speakers.
 *
 *  \param [in] frequency The frequency to play the sound.
 *  \param [in] play_duration The length of the beep in milliseconds.
 */
void beep(uint32_t frequency, uint32_t play_duration);

#endif /* INCLUDE_SPEAKER_H */
