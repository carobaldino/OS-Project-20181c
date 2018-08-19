#ifndef ESI_H_
#define ESI_H_

//Includes
#include "../bibliotecas_compartidas/cliente_servidor.h"
#include "config_esi.h"
#include <parsi/parser.h>
#include "../bibliotecas_compartidas/protocolo.h"
#include <string.h>
#include "operaciones.h"
#include "signal.h"

//Constantes simbolicas
#define MAXIMO_TAMANIO_LINEA 100
#define FINALIZAR_CON_ERROR -1
#define FINALIZAR_CORRECTAMENTE 0

//Variables globales
int socket_coordinador;
int socket_planificador;
bool ejecutando_instrucciones;

//Para handlear ctrlc
void hander_ctrl_c();

#endif
