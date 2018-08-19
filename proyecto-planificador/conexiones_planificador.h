#ifndef CONEXIONES_PLANIFICADOR_H_
#define CONEXIONES_PLANIFICADOR_H_

#include "../bibliotecas_compartidas/cliente_servidor.h"
#include "../bibliotecas_compartidas/protocolo.h"
#include "listas.h"
#include "finalizar.h"
#include <stdio.h>
#include <stdlib.h>

/*Variables globales*/
int socket_coordinador;
int socket_escucha;

/*Declaraciones*/
void aceptar_clientes_planificador();
void cerrar_hilo_conexiones();
#endif /* CONEXIONES_PLANIFICADOR_H_ */
