#include "config_coordinador.h"

void levantar_configFile()
{
	conf_coordinador = config_create("coordinador.config");
	config_info.puerto_escucha = config_get_int_value(conf_coordinador,"puerto_escucha");
	tabla_instancias.cantidad_entradas = config_get_int_value(conf_coordinador,"cantidad_entradas");
	tabla_instancias.tamanio_entradas_bytes = config_get_int_value(conf_coordinador,"tamanio_entrada_bytes");
	config_info.retardo_miliseg = config_get_int_value(conf_coordinador,"retardo_miliseg");
	config_info.algoritmo_distribucion = detectar_algoritmo_distribucion(config_get_string_value(conf_coordinador, "algoritmo_distribucion"));
	config_destroy(conf_coordinador);
}

t_algoritmo_coordinador detectar_algoritmo_distribucion (char* algor_a_insertar) {
	if(strcmp( algor_a_insertar, "LSU")==0) {
		return LSU;
	}
	if(strcmp( algor_a_insertar, "KE")==0) {
		return KE;
	}
	if(strcmp( algor_a_insertar, "EL")==0) {
		return EL;
	}
	else {
		printf("No se detecto ningun algoritmo");
		exit(0);
	}
}

