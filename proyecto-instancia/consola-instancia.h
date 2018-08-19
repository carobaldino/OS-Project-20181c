#ifndef CONSOLA_INSTANCIA_H_
#define CONSOLA_INSTANCIA_H_

#include "estructuras-comunes.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>

pthread_t hilo_consola;

void consola();
void estado();
void debug_on();
void debug_off();
void imprimir_entrada(void* entrada_en_tabla);
void pausar();
void continuar();
void imprimir_storage(void* entrada_en_tabla);
#endif /* CONSOLA_INSTANCIA_H_ */
