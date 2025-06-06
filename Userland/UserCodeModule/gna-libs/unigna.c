/*
 * Ejemplo de uso de semáforos:
 *
 * // Crear un semáforo con valor inicial 1 (mutex)
 * int32_t mutex = sem_open(1);
 * if (mutex == -1) {
 *     // Error al crear semáforo
 *     return;
 * }
 *
 * // Entrar en sección crítica
 * sem_wait(mutex);
 * // ... código de sección crítica ...
 * sem_post(mutex);
 *
 * // Obtener valor actual
 * int32_t value = sem_getvalue(mutex);
 *
 * // Destruir semáforo cuando ya no se necesite
 * sem_close(mutex);
 */

#include <unigna.h>
#include <libasm.h>
#include <stdint.h>

int32_t sem_init(uint32_t initial_value)
{
    return (int32_t)sys_call(SYS_SEM_OPEN, initial_value, 0, 0, 0, 0, 0);
}

int32_t sem_close(uint32_t sem)
{
    return (int32_t)sys_call(SYS_SEM_CLOSE, sem, 0, 0, 0, 0, 0);
}

int32_t sem_wait(uint32_t sem)
{
    return (int32_t)sys_call(SYS_SEM_WAIT, sem, 0, 0, 0, 0, 0);
}

int32_t sem_post(uint32_t sem)
{
    return (int32_t)sys_call(SYS_SEM_POST, sem, 0, 0, 0, 0, 0);
}

int32_t sem_getvalue(uint32_t sem)
{
    return (int32_t)sys_call(SYS_SEM_GETVALUE, sem, 0, 0, 0, 0, 0);
}