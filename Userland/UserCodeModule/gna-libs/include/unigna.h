#ifndef _UNIGNA_H_
#define _UNIGNA_H_

#include <stdint.h>

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

#endif /* _UNIGNA_H_ */