#ifndef random_h
#define random_h
#include <stdint.h>

/**
 * @brief Generates a random integer.
 * 
 * This function returns a random integer value. The range and 
 * distribution of the random values depend on the implementation.
 * 
 * @return A random integer.
 */
uint64_t random();

 /**
  * @brief Retorna un numero entero aleatorio en el rango [izq, der]
  */
 int randInt(int izq, int der);

#endif /* random_h */