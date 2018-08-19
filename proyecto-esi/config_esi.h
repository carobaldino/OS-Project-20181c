#ifndef CONFIG_ESI_H_
#define CONFIG_ESI_H_

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

//Tamaño de la ip
#define IP_SIZE (sizeof(char) * (strlen("123.000.000.001") + 1))

/**Estructuras**/
typedef struct{
	char* coordinador_ip;
	int coordinador_puerto;
	char* planificador_ip;
	int planificador_puerto;
}t_info;

/**Variables**/
t_config *conf_esi;
t_info config_info;

/**Prototipos**/
void levantar_configFile();

#endif /* CONFIG_ESI_H_ */
