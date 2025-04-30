#ifndef _GNASOUND_H_
#define _GNASOUND_H_

#include <stdint.h>

enum note_frequencies
{
    C0 = 16,
    D0 = 18,
    E0 = 21,
    F0 = 23,
    G0 = 26,
    A0 = 29,
    B0 = 31,
    C1 = 33,
    D1 = 37,
    E1 = 41,
    F1 = 46,
    G1 = 52,
    A1 = 58,
    B1 = 65,
    C2 = 69,
    D2 = 78,
    E2 = 87,
    F2 = 98,
    G2 = 104,
    A2 = 117,
    B2 = 131,
    C3 = 139,
    D3 = 156,
    E3 = 175,
    F3 = 196,
    G3 = 220,
    A3 = 247,
    B3 = 277,
    C4 = 261,
    D4 = 294,
    E4 = 329,
    F4 = 349,
    G4 = 392,
    A4 = 440,
    B4 = 493,
    C5 = 523,
    D5 = 587,
    E5 = 659,
    F5 = 698,
    G5 = 784,
    A5 = 880,
    B5 = 987,
    C6 = 1046,
    D6 = 1175,
    E6 = 1319,
    F6 = 1397,
    G6 = 1568,
    A6 = 1760,
    B6 = 1976,
    C7 = 2093,
    D7 = 2349,
    E7 = 2637,
    F7 = 2794,
    G7 = 3136,
    A7 = 3520,
    B7 = 3951,
    C8 = 4186,
    D8 = 4699,
    E8 = 5274,
    F8 = 5588,
    G8 = 6272,
    A8 = 7040,
    B8 = 7902
};

typedef struct
{
    uint32_t frequency;
    uint32_t ticks;
} Sound;

/**
 * @brief Plays a sound at the specified frequency.
 *
 * @param frequency The frequency of the sound to be played.
 */
void play_sound(uint32_t frequency);

/**
 * @brief Stops the currently playing sound.
 * if nothing is playing, this function does nothing.
 */
void stop_sound(void);

/**
 * @brief Plays a sound at the specified frequency for a given number of ticks.
 *
 * @param frequency The frequency of the sound to be played.
 * @param ticks The duration in ticks for which the sound should be played.
 */
void play_sound_for_ticks(uint32_t frequency, uint32_t ticks);

/**
 * @brief Adds a sound to the queue for reproduction.
 *
 * @param s The sound to be added.
 */
void set_sound(Sound s);

/**
 * @brief Updates the sound system to play the next sound or stop the current sound.
 */
void actualize_sound();

#endif // SOUNDS_H