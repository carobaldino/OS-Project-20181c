#ifndef ALGORITMOS_DISTRIBUCION_H_
#define ALGORITMOS_DISTRIBUCION_H_

#include "coordinador.h"
#include "config_coordinador.h"
#include "conexiones_coordinador.h"
#include <math.h>

#define CANTIDAD_DE_LETRAS ('z'-'a' + 1)

int prox_instancia_a_seleccionar;
pthread_mutex_t mutex_instancia;
t_list* registro_claves;

bool aplicar_algoritmo_distribucion(char* clave);
bool distribuirPorEL(char* clave);
bool guardarEnInstancia(char* clave, t_instancia* instancia);
bool distribuirPorLSU(char* clave);
bool distribuirPorKE(char* clave);;
bool ordenar_por_espacio_restante(void* instancia_void_1, void* instancia_void_2);
t_instancia* seleccionar_instancia_para_distribuir_clave(char* clave);
#endif
