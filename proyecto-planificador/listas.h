
#ifndef LISTAS_H_
#define LISTAS_H_

//Includes
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "config_planificador.h"
#include "../bibliotecas_compartidas/protocolo.h"
#include <string.h>
#include <semaphore.h>
#include <stdbool.h>
#include "finalizar.h"

typedef struct{
	//Parte en comun
	double estimacion_actual;
	double estimacion_anterior;
	int real_anterior;

	//Para SJF-CD
	double rafaga_actaual;
	bool desalojado;

	//PARA HRRN
	unsigned long tiempo_ultima_llegada_a_listos;
	double responsive_ratio;

	//
	bool ya_estimado;
}t_estimacion;

typedef struct{
	int socket;
	int ID;
	t_list* claves_bloqueadas;
	char* clave_bloqueante;
	t_estimacion estimacion;
	bool terminado;
	bool bloqueado;
	char* nombre;
}t_esi;

typedef struct{
	char* nombre;
	int ID;
}t_esi_terminado;

#include "algoritmos_planificacion.h"

//Listas

t_list* listos;
t_list* bloqueados;
t_list* ejecutando;
t_list* terminados;
t_list* claves_bloqueadas;

//Variable globlal
unsigned int ultimo_id;
sem_t contador_esis;
sem_t producto_consumidor_status;
bool llego_esi_nuevo;
pthread_mutex_t mutex_listas;
pthread_mutex_t mutex_pausar_continuar;
t_esi* esi_a_ejecutar;
bool pausado;
/**
* @NAME: inicializar_listas
* @DESC: hace el list_create de las listas del planificador
*/
void inicializar_listas();

/**
* @NAME: agregar_nuevo_esi
* @DESC: Crea un nuevo t_esi y lo enlista en nuevos -- lista por defecto... es bastante logico
* @PARAMS:
* 		socket - socket a traves del cual se va comunicar el ESI y el Planificador
*/
void agregar_nuevo_esi(int socket, char* nombre);

/**
* @NAME: mover_esi
* @DESC: mueve el esi de una lista a otra
* @PARAMS:
* 		esi - t_esi a mover
* 		origen - lista de origen del esi
* 		destino - lista destino del esi
*/
void mover_esi(t_esi* esi, t_list* origen, t_list* destino);

/**
* @NAME: listo_a_ejecutando
* @DESC: hace mover_esi de listo a ejecutando
* @PARAMS:
*		esi - t_esi a mover
*/
void listo_a_ejecutando(t_esi* esi);

/**
* @NAME: ejecutando_a_bloqueado
* @DESC: hace mover_esi de ejecutando a bloqueado
* @PARAMS:
*		esi - t_esi a mover
*/
void ejecutando_a_bloqueado(t_esi* esi, char* clave_bloqueante);

/**
* @NAME: bloqueado_a_listo
* @DESC: hace mover_esi de bloqueado a listo
* @PARAMS:
*		esi - t_esi a mover
*/
void bloqueado_a_listo(t_esi* esi);

void ejecutando_a_listo(t_esi* esi);

#include "consola.h"

void bloquear_clave_para_esi_ejecutando(char* clave);
void desbloquear_clave_de_esi(char* clave, t_esi* esi);
void eliminar_clave_de_lista_de_claves_bloqueadas(char* clave);
void desbloquear_esi_bloqueados_por_clave(char* clave_a_desbloquear);
void listo_a_bloqueado(t_esi* esi, char* clave_bloqueante);

/*Manda un ESI a terminados, liberando las claves y cerrando
 * los sockets.*/
void terminar_esi(t_esi* victima, t_list* lista_origen);

#endif /* LISTAS_H_ */
