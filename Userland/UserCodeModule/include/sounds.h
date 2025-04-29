#ifndef SOUNDS_H
#define SOUNDS_H

#include <stdint.h>
//notes frequencies
#define C0 16
#define D0 18
#define E0 21
#define F0 23
#define G0 26
#define A0 29
#define B0 31
#define C1 33
#define D1 37
#define E1 41
#define F1 46
#define G1 52
#define A1 58
#define B1 65
#define C2 69
#define D2 78
#define E2 87
#define F2 98
#define G2 104
#define A2 117
#define B2 131
#define C3 139
#define D3 156
#define E3 175
#define F3 196
#define G3 220
#define A3 247
#define B3 277
#define C4 261
#define D4 294
#define E4 329
#define F4 349
#define G4 392
#define A4 440
#define B4 493
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784
#define A5 880
#define B5 987
#define C6 1046
#define D6 1175
#define E6 1319
#define F6 1397
#define G6 1568
#define A6 1760
#define B6 1976
#define C7 2093
#define D7 2349
#define E7 2637
#define F7 2794
#define G7 3136
#define A7 3520
#define B7 3951
#define C8 4186
#define D8 4699
#define E8 5274
#define F8 5588
#define G8 6272
#define A8 7040
#define B8 7902

typedef struct{
    uint32_t frequency;
    uint32_t ticks;
}Sound;

/**
 * @brief Plays a sound at the specified frequency.
 *
 * @param frequency The frequency of the sound to be played.
 */
void playSound(uint32_t frequency);

/**
 * @brief Stops the currently playing sound.
 * if nothing is playing, this function does nothing.
 */
void stopSound(void);
/**
 * @brief Plays a sound at the specified frequency for a given number of ticks.
 *
 * @param nFrequence The frequency of the sound to be played.
 * @param ticks The duration in ticks for which the sound should be played.
 */
void playSoundForTicks(uint32_t nFrequence, uint32_t ticks);

/**
 * @brief Adds a sound to the queue for reproduction.
 *
 * @param s The sound to be added.
 */
void setSound(Sound s);
/**
 * @brief Updates the sound system to play the next sound or stop the current sound.
 */
void actulizeSound();

#endif // SOUNDS_H