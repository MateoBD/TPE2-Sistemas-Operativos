#include <pcb-queueADT.h>
#include <stdint.h>
#include <memory-manager.h>
#include <stddef.h>

#define MAX_PROCESSES 1024

typedef struct PCBQueueCDT 
{
    void * queue[MAX_PROCESSES];
    uint32_t head;
    uint32_t tail;
    uint8_t size;
} PCBQueueCDT;

PCBQueueADT new_PCBQueueADT()
{
    PCBQueueADT toReturn = (PCBQueueADT) memory_alloc(memory_manager, sizeof(*toReturn));
    if (toReturn != NULL) 
    {
        toReturn->head = 0;
        toReturn->tail = 0;
        toReturn->size = 0;
    }
    return toReturn;
}

int enqueue_process(PCBQueueADT self, void * const process) 
{
    if (self->size == MAX_PROCESSES) {
        return -1;
    }
    self->queue[self->head++] = process;
    self->head %= MAX_PROCESSES;
    self->size++;
    return 0;
}

void * dequeue_process(PCBQueueADT self) 
{
    if (self->size == 0) {
        return NULL;
    }
    void * process = self->queue[self->tail++];
    self->tail %= MAX_PROCESSES;
    self->size--;
    return process;
}
