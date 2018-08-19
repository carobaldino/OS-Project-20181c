#include "config_esi.h"


void levantar_configFile()
{
	conf_esi = config_create("esi.config");
	config_info.coordinador_ip = malloc(IP_SIZE); //Porque una IP no tiene más de 17 (4*4+1)
	strcpy(config_info.coordinador_ip, config_get_string_value(conf_esi, "coordinador_ip"));
	config_info.planificador_ip = malloc(IP_SIZE); //Porque una IP no tiene más de 17 (4*4+1)
	strcpy(config_info.planificador_ip, config_get_string_value(conf_esi, "planificador_ip"));
	config_info.coordinador_puerto = config_get_int_value(conf_esi, "coordinador_puerto");
	config_info.planificador_puerto = config_get_int_value(conf_esi, "planificador_puerto");
	config_destroy(conf_esi);
}
