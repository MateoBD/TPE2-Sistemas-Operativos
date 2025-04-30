#include <gnasound.h>
#include <libasm.h>
#include <gnalib.h>

#define SYS_PLAY_SOUND 12
#define SYS_STOP_SOUND 13
#define BUFFER_SIZE 20

Sound music_buffer[BUFFER_SIZE] = {0};
static int music_buffer_index_for_add = 0;
static int music_buffer_index_for_consume = 0;
static int ticks_start = 0;
static int ticks_now = 0;
static int sound_ticks = 0;

void play_sound(uint32_t frequency)
{
    sys_call(SYS_PLAY_SOUND, frequency, 0, 0, 0);
}

void stop_sound(void)
{
    sys_call(SYS_STOP_SOUND, 0, 0, 0, 0);
}

void play_sound_for_ticks(uint32_t frequency, uint32_t ticks)
{
    play_sound(frequency);
    sleep(ticks);
    stop_sound();
}

void set_sound(Sound s)
{
    music_buffer[music_buffer_index_for_add] = s;
    music_buffer_index_for_add++;
    music_buffer_index_for_add %= BUFFER_SIZE;
}

static int has_next_sound()
{
    return music_buffer_index_for_add != music_buffer_index_for_consume;
}

static int sound_time()
{
    return ticks_now - ticks_start <= sound_ticks;
}

void actualize_sound()
{
    ticks_now = get_ticks();
    if (has_next_sound() && !sound_time())
    {
        play_sound(music_buffer[music_buffer_index_for_consume].frequency);
        sound_ticks = music_buffer[music_buffer_index_for_consume].ticks;
        ticks_start = get_ticks();
        music_buffer_index_for_consume++;
        music_buffer_index_for_consume %= BUFFER_SIZE;
        return;
    }
    stop_sound();
}