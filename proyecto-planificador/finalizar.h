#ifndef FINALIZAR_H_
#define FINALIZAR_H_

#include <stdio.h>
#include <stdlib.h>
#include "conexiones_planificador.h"
#include <signal.h>

pthread_t hilo_interfaz_coordinador_planificador;
pthread_t hilo_planificacion;
pthread_t hilo_Aceptar_Clientes;
pthread_t hilo_consola;

bool cerrando;
bool modo_debug;

void finalizar(char* mensaje_de_cierre);
void handler_ctrl_c(int);

#endif /* FINALIZAR_H_ */
