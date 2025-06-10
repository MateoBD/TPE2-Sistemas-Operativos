#ifndef _PROCESSES_H_
#define _PROCESSES_H_

#include <stdint.h>
#include <syscalls.h>

#define MAX_PROCESSES 1024
typedef uint16_t pid_t;

extern void call_int_20();

/*
 * @brief Inicializa los procesos y el scheduler
 */
int init_processes();

/*
 * @brief Verifica si los procesos han sido inicializados
 * @return 1 si están inicializados, 0 si no
 */
int processes_initialized();

/*
 * @brief Obtiene el proceso idle
 * @return Puntero al proceso idle
 */
void *get_idle_process_stack();

/*
 * @brief Scheduler principal
 * @param current_stack Puntero al stack del proceso actual
 * @return Puntero al stack del siguiente proceso a ejecutar
 */
void set_next_process(void *current_stack);

/*
 * @brief Scheduler principal
 * @param current_stack Puntero al stack del proceso actual
 * @return Puntero al stack del siguiente proceso a ejecutar
 */
void *get_next_process();

/*
 * @brief Libera los procesos terminados
 * Esta función libera la memoria de los procesos que han terminado su ejecución.
 */
void free_terminated_processes(void);

/**
 * @brief Crea un nuevo proceso
 * @param name Nombre del proceso
 * @param entryPoint Punto de entrada (función main)
 * @param argc Cantidad de argumentos
 * @param argv Vector de argumentos
 * @param fds Vector de file descriptors
 * @return PID del nuevo proceso o -1 si hubo error
 */
pid_t create_process(const char *name, void *entryPoint, int argc, char **argv, uint16_t *fds, uint8_t is_foreground);

/**
 * @brief Termina un proceso
 * @param pid PID del proceso a terminar
 * @return 0 si se terminó correctamente, -1 si hubo error
 */
int kill_process(uint32_t pid);

/**
 * @brief Mata el proceso en primer plano
 * @return 0 si se mató correctamente, -1 si hubo error
 */
int kill_foreground_process();

/**
 * @brief Obtiene el PID del proceso actual
 * @return PID del proceso en ejecución
 */
pid_t get_current_pid();

/**
 * @brief Verifica si el proceso actual es de primer plano
 * @return 1 si es de primer plano, 0 si no
 */
int is_foreground(pid_t pid);

/**
 * @brief Espera a que un proceso termine
 * @param pid PID del proceso a esperar
 */
void wait_process(pid_t pid, int8_t *status);

/**
 * @brief Obtiene la prioridad del proceso actual
 * @return Prioridad del proceso en ejecución
 */
uint8_t get_current_priority(void);

/**
 * @brief Obtiene los file descriptors del proceso actual
 * @param fds Puntero a un array donde se guardarán los file descriptors
 * @return 0 si se obtuvieron correctamente, -1 si hubo error
 */
int get_current_fds(uint16_t *fds);

/**
 * @brief Cambia la prioridad de un proceso
 * @param pid PID del proceso
 * @param newPriority Nueva prioridad
 * @return 0 si se cambió correctamente, -1 si hubo error
 */
int change_priority(uint32_t pid, uint8_t newPriority);

/**
 * @brief Establece el estado de salida del proceso actual
 * @param status Estado de salida del proceso
 */
void set_exit_status(int8_t status);

/**
 * @brief Bloquea el proceso actual
 * @param pid PID del proceso a bloquear
 * @return 0 si se bloqueó correctamente, -1 si hubo error
 */
int block_process(pid_t pid);

/**
 * @brief Desbloquea un proceso
 * @param pid PID del proceso a desbloquear
 * @return 0 si se desbloqueó correctamente, -1 si hubo error
 */
int wake_up_process(pid_t pid);

/**
 * @brief Verifica si un proceso está bloqueado
 * @param pid PID del proceso
 * @return 1 si está bloqueado, 0 si no
 */
int32_t is_sleeping(pid_t pid);

/**
 * @brief Cambia el estado de un proceso a durmiendo
 * @param pid PID del proceso
 * @return 0 si se durmió correctamente, -1 si hubo error
 */
int32_t sleep_process(pid_t pid);

/**
 * @brief Despierta un proceso que estaba durmiendo
 * @param pid PID del proceso
 * @return 0 si se despertó correctamente, -1 si hubo error
 */
int32_t unsleep_process(pid_t pid);

/**
 * @brief Cambia el estado de un proceso a bloqueado o desbloqueado, o viceversa
 * @param pid PID del proceso
 * @return 0 si se cambió correctamente, -1 si hubo error
 */
int toggle_block_process(pid_t pid);

/**
 * @brief Verifica si hay procesos en ejecución
 * @return 1 si hay procesos en ejecución, 0 si no
 */
int has_running_processes();

/**
 * @brief Verifica si el sistema está en ejecución
 * @return 1 si el sistema está corriendo, 0 si no
 */
int is_system_running();

/**
 * @brief Detiene el sistema
 * Esta función detiene todos los procesos y libera los recursos del sistema.
 */
void stop_system();

/**
 * @brief Obtiene información de todos los procesos activos
 * @param process_array Array donde se guardará la información de los procesos
 * @param max_processes Tamaño máximo del array
 * @return Número de procesos encontrados, -1 si hubo error
 */
int get_processes_info(ProcessInfo *process_array, int max_processes);

#endif /* PROCESSES_H */