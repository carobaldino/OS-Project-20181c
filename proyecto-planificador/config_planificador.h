#ifndef CONFIG_PLANIFICADOR_H
#define CONFIG_PLANIFICADOR_H

#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "finalizar.h"

//Tamaño de la ip
#define IP_SIZE (sizeof(char) * (strlen("123.000.000.001") + 1))

/**Estructuras**/

typedef enum{
	FIFO,
	HRRN,
	SJF_CD,
	SJF_SD,
}t_algoritmo_planificador;

typedef struct {
	char *ip_coordinador;
	int puerto_coordinador;
	int puerto_escucha;
	t_algoritmo_planificador algoritmo;
	int estimacion_inicial;
	int alfa;
	char** claves_inicialmente_bloqueadas;
	int cantidad_claves_inicialmente_bloqueadas;
} t_info;

/**Globales**/

t_config *conf_planificador;
t_info config_info;

/**Declaraciones**/

void levantar_configFile();
t_algoritmo_planificador detectar_algoritmo_planificacion(char*);

#endif
