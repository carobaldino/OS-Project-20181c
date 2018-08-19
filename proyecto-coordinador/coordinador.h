#ifndef COORDINADOR_H_
#define COORDINADOR_H_

#define CANT_INSTANCIAS 10

#include "config_coordinador.h"
#include "../bibliotecas_compartidas/cliente_servidor.h"
#include <pthread.h>
#include <commons/log.h>
#include "../bibliotecas_compartidas/protocolo.h"
#include "conexiones_coordinador.h"
#include "algoritmos_distribucion.h"
#include <semaphore.h>
#include <commons/log.h>

//Variables globables
pthread_t hilo_Aceptar_Clientes;
t_log* logger;

#endif
