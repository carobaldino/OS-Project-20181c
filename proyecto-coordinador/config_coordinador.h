#ifndef CONFIG_COORDINADOR_H_
#define CONFIG_COORDINADOR_H_

#include <commons/config.h>
#include <readline/readline.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>


/**Estructuras**/
typedef enum{
	LSU,
	EL,
	KE,
}t_algoritmo_coordinador;

typedef struct{
	int puerto_escucha;
	int retardo_miliseg;
	t_algoritmo_coordinador algoritmo_distribucion;
}t_info;

	//YA QUE ES ALGO QUE USAN DOS PROCESOS DEBERIA ESTAR EN LAS BIBLIOTECAS COMPARTIDAS
typedef struct{
	int cantidad_entradas;
	int tamanio_entradas_bytes;
}t_tabla_instancias;

/**Variables**/
t_config *conf_coordinador;
t_info config_info;
t_tabla_instancias tabla_instancias;

/**Prototipos**/
void levantar_configFile();
t_algoritmo_coordinador detectar_algoritmo_distribucion (char* algor_a_insertar);




#endif /* CONFIG_COORDINADOR_H_ */
