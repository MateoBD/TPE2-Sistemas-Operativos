#include<sounds.h>
#include<libasm.h>
#include<libc.h>
#define SYS_PLAY_SOUND 12
#define SYS_STOP_SOUND 13
#define BUFFER_SIZE 20

Sound musicBuffer[BUFFER_SIZE]={0};
static int musicBufferIndexForAdd=0;
static int musicBufferIndexForConsume=0;
static int ticksStart=0;
static int ticksNow=0;
static int soundTicks=0;

void playSound(uint32_t frequency){
    sys_call(SYS_PLAY_SOUND, frequency, 0, 0, 0);
}
void stopSound(void){
    sys_call(SYS_STOP_SOUND, 0, 0, 0, 0);
}
void playSoundForTicks(uint32_t nFrequence, uint32_t ticks){
    playSound(nFrequence);
    sleep(ticks);
    stopSound();
}
void setSound(Sound s){
    musicBuffer[musicBufferIndexForAdd]=s;
    musicBufferIndexForAdd++;
    musicBufferIndexForAdd%=BUFFER_SIZE;
}
static int hasNextSound(){
    return musicBufferIndexForAdd!=musicBufferIndexForConsume;
}
static int soundTime(){
    return ticksNow-ticksStart<=soundTicks;
}

void actulizeSound(){
    ticksNow=getTicks();
    if(hasNextSound() && !soundTime()){
        playSound(musicBuffer[musicBufferIndexForConsume].frequency);
        soundTicks=musicBuffer[musicBufferIndexForConsume].ticks;
        ticksStart=getTicks();
        musicBufferIndexForConsume++;
        musicBufferIndexForConsume%=BUFFER_SIZE;
        return;
    }
    stopSound();
}