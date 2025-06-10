#ifndef _PCB_QUEUE_ADT_H_
#define _PCB_QUEUE_ADT_H_

#include <stdint.h>

typedef struct PCBQueueCDT *PCBQueueADT;

/**
 * @brief Crea una nueva cola de PCBs
 * @return Un puntero a la nueva cola, o NULL si no se pudo crear
 */
PCBQueueADT new_PCBQueueADT();

/**
 * @brief Añade un proceso a la cola
 * @param self Puntero a la cola
 * @param process Puntero al proceso a encolar
 * @return 0 si se encoló correctamente, -1 si la cola está llena
 */
int enqueue_process(PCBQueueADT self, void *const process);

/**
 * @brief Extrae un proceso de la cola
 * @param self Puntero a la cola
 * @return Puntero al proceso desencolado, o NULL si la cola está vacía
 */
void * dequeue_process(PCBQueueADT self);

#endif /* PCB_QUEUE_ADT_H */