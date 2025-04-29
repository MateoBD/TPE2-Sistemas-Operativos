//
//  random.c
//  random
//
//
#include <random.h>
#include <libc.h>

static unsigned int seed = 3723141;

int random() {
    time* t = getTime();
    seed = seed * 1103515245 + 12345 * (t->sec);
    seed += (t->min);
    seed = (unsigned int)(seed * 7 / 65536) % 32768;
    return seed;
}

int randInt(int izq, int der) {
    if ( izq > der) {
        int aux = izq;
        izq = der;
        der = aux;
    }
    int diff = der - izq +1;
    return (random()%diff)+izq;
}