#ifndef CONEXIONES_COORDINADOR_H_
#define CONEXIONES_COORDINADOR_H_

#include "../bibliotecas_compartidas/cliente_servidor.h"
#include "../bibliotecas_compartidas/protocolo.h"
#include "config_coordinador.h"
#include <commons/collections/list.h>
#include <semaphore.h>

int socket_planificador;
int socket_escucha;

typedef struct {
	int espacio_memoria_total;
	int espacio_utilizado;
	int socket_instancia;
	char* nombre;
	sem_t* productor_consumidor_de_esta_instancia;
	t_prot_mensaje* mensaje_instancia;
}t_instancia;

typedef struct{
	char* clave;
	char* valor;
	t_instancia* instancia;
}t_registro_claves;

#include "algoritmos_distribucion.h" //La pongo aca porque si la pongo arriba no compile Wtf

t_list* lista_instancias;

/*Atendiendo planificador por hilo*/
t_prot_mensaje* mensaje_planificador;
pthread_mutex_t mutex_mensaje_planificador;
pthread_mutex_t mutex_registro;
sem_t productor_consumidor_mensajes_planificador;

void aceptar_clientes_coordinador();
void escuchar_instancia(int* socket_instancia);
void enviar_configTabla_a_instancia(int socket_instancia);
void escuchar_esi(int* socket_esi);
void escuchar_planificador();

bool solicitar_clave(int socket_esi, t_prot_mensaje* mensaje_del_esi);
bool asignar_valor(int socket_esi, t_prot_mensaje* mensaje_del_esi);
bool persistir_clave(int socket_esi, t_prot_mensaje* mensaje_del_esi);
bool set_valor(char* clave, char* valor);
bool store_clave(char* clave);
void status(char* clave);

void printear_instancia(void* instancia);
t_registro_claves* buscar_clave_en_registro(char* clave);
#endif /* CONEXIONES_COORDINADOR_H_ */
