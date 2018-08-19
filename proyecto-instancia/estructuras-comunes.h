#ifndef ESTRUCTURAS_COMUNES_H_
#define ESTRUCTURAS_COMUNES_H_

#include "../bibliotecas_compartidas/protocolo.h"
#include "../bibliotecas_compartidas/cliente_servidor.h"
#include <commons/collections/list.h>
#include <commons/txt.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/stat.h>

//Estructuras
typedef enum{
	CIRC,
	LRU,
	BSU,
}t_algoritmo_reemplazo;

typedef struct{
	char* ip_coordinador;
	int puerto_coordinador;
	t_algoritmo_reemplazo algortimo_reemplazo;
	char* punto_montaje;
	char* nombre_instancia;
	int intervalo_dump_seg;
}t_info;

typedef struct {
	int cant_entrada;
	int tam_entrada;
} datos_armado_tabla;

typedef struct {
	char* clave;
	int entrada_inicial;
	int tamanio_valor;
} t_tabla_entradas;

typedef struct{
	void* inicio_bloque;
	bool ocupado;
}t_bitmap_bloques;

//Variables globales usadas en varios files
bool nombre_por_parametro;
bool modo_debug;
bool ejecucion_pausada;//Determian si se debe continuar al fin de ejecutar una isntruccion
t_list* tabla_entradas;
t_bitmap_bloques* bitmap;
int socket_coordinador;
datos_armado_tabla datos_tabla;
t_info config_info;
bool pausado;
pthread_mutex_t mutex_pausar_continuar;
pthread_mutex_t mutex_dump_compactar;
#endif /* ESTRUCTURAS_COMUNES_H_ */
