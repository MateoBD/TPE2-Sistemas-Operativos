#ifndef _SEMAPHORES_H_
#define _SEMAPHORES_H_

#include <stdint.h>

#define MAX_SEMAPHORES 512

/**
 * @brief Crea un nuevo semáforo con un valor inicial
 * @param initial_value Valor inicial del semáforo
 * @return Identificador del semáforo creado o -1 si hubo error
 */
int32_t create_sem(uint32_t initial_value);

/**
 * @brief Realiza una operación wait en el semáforo
 * @param sem Identificador del semáforo
 */
void sem_wait(uint32_t sem);

/**
 * @brief Realiza una operación post en el semáforo
 * @param sem Identificador del semáforo
 */
void sem_post(uint32_t sem);

/**
 * @brief Destruye un semáforo
 * @param sem Identificador del semáforo a destruir
 */
void destroy_sem(uint32_t sem);

/**
 * @brief Obtiene el valor actual de un semáforo
 * @param sem Identificador del semáforo
 * @return Valor actual del semáforo o 0 si no existe
 */
uint32_t get_sem_value(uint32_t sem);

#endif /* _SEMAPHORES_H_ */