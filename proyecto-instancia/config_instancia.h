#ifndef CONFIG_INSTANCIA_H_
#define CONFIG_INSTANCIA_H_

#include "estructuras-comunes.h"
#include <commons/config.h>

// variables simbolicas
#define IP_SIZE (sizeof(char) * (strlen("123.000.000.001") + 1))

t_config *conf_instancia;

//Prototipo
void levantar_configFile();
t_algoritmo_reemplazo detectar_algoritmo_reemplazo(char*);

#endif /* CONFIG_INSTANCIA_H_ */
