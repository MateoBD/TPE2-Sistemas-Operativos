#ifndef PC_SPEAKER_DRIVER_H
#define PC_SPEAKER_DRIVER_H

#include <stdint.h>

void play_sound(uint32_t n_frequency);
void stop_sound();

#endif // PC_SPEAKER_DRIVER_H