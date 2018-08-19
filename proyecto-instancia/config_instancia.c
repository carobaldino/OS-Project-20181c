#include "config_instancia.h"


void levantar_configFile(){
	conf_instancia = config_create("instancia.config");
	config_info.ip_coordinador = malloc(IP_SIZE);
	strcpy(config_info.ip_coordinador , config_get_string_value(conf_instancia, "ip_coordinador" ));
	config_info.puerto_coordinador = config_get_int_value(conf_instancia,"puerto_coordinador");
	config_info.algortimo_reemplazo = detectar_algoritmo_reemplazo(config_get_string_value(conf_instancia, "algoritmo_reemplazo"));
	if(!nombre_por_parametro){
		config_info.nombre_instancia = strdup(config_get_string_value(conf_instancia, "nombre_instancia" ));
		config_info.punto_montaje = strdup(config_get_string_value(conf_instancia, "punto_montaje"));
	}
	config_info.intervalo_dump_seg = config_get_int_value(conf_instancia,"intervalo_dump_seg");
	config_destroy(conf_instancia);
}

t_algoritmo_reemplazo detectar_algoritmo_reemplazo (char* algor_a_insertar) {
	if(strcmp( algor_a_insertar, "CIRC")==0) {
		return CIRC;
	}else
	if(strcmp( algor_a_insertar, "LRU")==0) {
		return LRU;
	}else
	if(strcmp( algor_a_insertar, "BSU")==0) {
		return BSU;
	}else{
		printf("No se detecto ningun algoritmo");
		exit(0);
	}
}




