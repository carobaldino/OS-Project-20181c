#include "config_planificador.h"

void levantar_configFile(){
	conf_planificador = config_create("planificador.config");
	config_info.ip_coordinador = (char*) malloc(IP_SIZE);
	strcpy(config_info.ip_coordinador , config_get_string_value(conf_planificador, "ip_coordinador" ));
	config_info.puerto_escucha = config_get_int_value(conf_planificador, "puerto_escucha");
	config_info.puerto_coordinador =  config_get_int_value(conf_planificador, "puerto_coordinador");
	config_info.estimacion_inicial = config_get_int_value(conf_planificador, "estimacion_inicial");
	config_info.alfa = config_get_int_value(conf_planificador, "alfa_inicial");
	config_info.algoritmo = detectar_algoritmo_planificacion(config_get_string_value(conf_planificador, "algoritmo_planificacion"));
	config_info.claves_inicialmente_bloqueadas = config_get_array_value(conf_planificador,"claves_inicialmente_bloqueadas");
	config_info.cantidad_claves_inicialmente_bloqueadas = config_get_int_value(conf_planificador, "cantidad_claves_inicialmente_bloqueadas");
	config_destroy(conf_planificador);
}

t_algoritmo_planificador detectar_algoritmo_planificacion(char* algoritmo){
	if(strcmp(algoritmo, "HRRN") == 0){
		return HRRN;
	}
	if(strcmp(algoritmo, "SJF_CD") == 0){
		return SJF_CD;
	}
	if(strcmp(algoritmo, "SJF_SD") == 0){
		return SJF_SD;
	}
	printf("No se reconoce algoritmo de planifiacion, se elije FIFO\n");
	free(algoritmo);
	return FIFO;
}
