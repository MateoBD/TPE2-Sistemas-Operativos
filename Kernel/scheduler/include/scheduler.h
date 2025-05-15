#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>

/**
 * @brief Inicializa el scheduler
 */
int init_scheduler();

/**
 * @brief Crea un nuevo proceso
 * @param name Nombre del proceso
 * @param entryPoint Punto de entrada (función main)
 * @param priority Prioridad del proceso
 * @param argc Cantidad de argumentos
 * @param argv Vector de argumentos
 * @return PID del nuevo proceso o -1 si hubo error
 */
int create_process(void * entryPoint, uint8_t priority, int argc, char** argv);

/**
 * @brief Termina un proceso
 * @param pid PID del proceso a terminar
 * @return 0 si se terminó correctamente, -1 si hubo error
 */
int kill_process(uint32_t pid);

/**
 * @brief Obtiene el PID del proceso actual
 * @return PID del proceso en ejecución
 */
uint32_t get_current_pid();

/**
 * @brief Obtiene la prioridad del proceso actual
 * @return Prioridad del proceso en ejecución
 */
uint8_t get_current_priority();

/**
 * @brief Cambia la prioridad de un proceso
 * @param pid PID del proceso
 * @param newPriority Nueva prioridad
 * @return 0 si se cambió correctamente, -1 si hubo error
 */
int change_priority(uint32_t pid, uint8_t newPriority);

#endif /* SCHEDULER_H */
