#ifndef DUMP_H_
#define DUMP_H_

#include "estructuras-comunes.h"

pthread_t HiloDump;

void dump();
void persistir_entrada(void* entrada);
#endif /* DUMP_H_ */
