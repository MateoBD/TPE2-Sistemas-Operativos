#ifndef _GNAUNI_H_
#define _GNAUNI_H_

#include <stdint.h>

/**
 * @file gnauni.h
 * @brief Header file for GNA userland library functions.
 *
 * This header provides the interface for creating processes, managing semaphores,
 * and other userland functionalities in the GNA operating system.
 */

/**
 * @brief Crea un nuevo proceso
 * @param name Nombre del proceso
 * @param function Función que se ejecutará en el nuevo proceso
 * @param argc Cantidad de argumentos
 * @param argv Vector de argumentos
 * @param fds Vector de file descriptors (stdin, stdout)
 * @param is_foreground Indica si el proceso es de primer plano (1) o fondo (0)
 * @return PID del nuevo proceso o -1 si hubo error
 */
uint64_t create_process(const char *name, void *function, int argc, char **argv, uint16_t *fds, char is_foreground);

/**
 * @brief Obtiene el PID del proceso actual
 * @return PID del proceso en ejecución
 */
uint32_t get_pid(void);

/**
 * @brief Cambia la prioridad de un proceso
 * @param pid PID del proceso
 * @param newPriority Nueva prioridad
 * @return 0 si se cambió correctamente, -1 si hubo error
 */
int nice(uint32_t pid, uint8_t newPriority);

/**
 * @brief Espera a que un proceso termine
 * @param pid PID del proceso a esperar
 * @param status Puntero donde se guardará el estado de salida del proceso
 * @return 0 si se esperó correctamente, -1 si hubo error
 */
int wait(uint32_t pid, int8_t *status);

/**
 * @brief Crea un nuevo semáforo con un valor inicial
 * @param initial_value Valor inicial del semáforo
 * @return Identificador del semáforo creado o -1 si hubo error
 */
int32_t sem_init(uint32_t initial_value);

/**
 * @brief Destruye un semáforo
 * @param sem Identificador del semáforo a destruir
 * @return 0 si se destruyó correctamente, -1 si hubo error
 */
int32_t sem_close(uint32_t sem);

/**
 * @brief Realiza una operación wait en el semáforo
 * @param sem Identificador del semáforo
 * @return 0 si se ejecutó correctamente, -1 si hubo error
 */
int32_t sem_wait(uint32_t sem);

/**
 * @brief Realiza una operación post en el semáforo
 * @param sem Identificador del semáforo
 * @return 0 si se ejecutó correctamente, -1 si hubo error
 */
int32_t sem_post(uint32_t sem);

/**
 * @brief Obtiene el valor actual de un semáforo
 * @param sem Identificador del semáforo
 * @return Valor actual del semáforo o -1 si hubo error
 */
int32_t sem_getvalue(uint32_t sem);

/**
 * @brief Termina un proceso dado su PID
 * @param pid PID del proceso a terminar
 * @return 0 si se terminó correctamente, -1 si hubo error
 */
int32_t kill(uint32_t pid);

/**
 * @brief Bloquea un proceso dado su PID
 * @param pid PID del proceso a bloquear
 * @return 0 si se bloqueó correctamente, -1 si hubo error
 */
int32_t block(uint32_t pid);

/**
 * @brief Desbloquea un proceso dado su PID
 * @param pid PID del proceso a desbloquear
 * @return 0 si se desbloqueó correctamente, -1 si hubo error
 */
int32_t unblock(uint32_t pid);

/**
 * @brief Bloquea el proceso actual hasta que se desbloquee
 * @return 0 si se bloqueó correctamente, -1 si hubo error
 */
int32_t sched_yield(void);

#endif /* _GNAUNI_H_ */