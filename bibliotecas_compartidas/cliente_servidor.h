#ifndef CLIENTE_SERVIDOR_H
#define CLIENTE_SERVIDOR_H

//Includes
#include <commons/config.h>
#include <readline/readline.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
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
#include <pthread.h>
#include "protocolo.h"

//Mensajes de error
#define ERROR_CREAR_SOCKET "Crear socket con socket()"
#define ERROR_RESEVAR_PUERTO "No se pudo reservar puerto con bind()"
#define ERROR_ESCUCHAR_PUERTO "No se pudo escuchar puerto con listen()"
#define ERROR_ACEPTAR_CLIENTE "No se pudo aceptar cliente con accept()"
#define ERROR_CONECTAR_SERVIDOR "No se pudo conectar al servidor"
#define Error_enviar_mensaje "No se pudo enviar mensaje a traves del socket"
#define Fallo_recibir_mensaje "No se pudo recibir mensaje a traves del socket"

//Valores hardcodeados
#define TIEMPO_SLEEP 2
#define VALOR_EXIT 1 //lo pongo aca porque no entiendo que hace y se hace algo y despues hay que cambiar es mas facil
#define MAX_CLIENTES_ENCOLADOS 128 //por qué ? No hay por qué

/*Prototipos*/
int iniciar_servidor(int puerto);
void crear_socket(int *socket);
void reservar_puerto(int socket, int puerto);
void escuchar_puerto(int socket, int maximo_cliente_encolados);
int conectar_a_servidor(char* ip, int puerto, t_cliente cliente);
void conectar_socket(int socket_cliente_, int puerto, char *ip);

//Atencion de clientes
void charlar(int *socket_origen);
//Otras
void print_error(char *mensaje);
//mocks
char* tipo_de_cliente(t_cliente cliente);

#endif /* SOCKET_CLIENTE_H_ */
