#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define VALOR_EXIT 1
#define FALLO_ENVIAR_MENSAJE "No se pudo enviar mensaje a traves del socket"
#define LARGO_MAXIMO_CLAVE 40

//HEADER
typedef enum{
	CONEXION, //Un cliente informa a un servidor que se ha conectado. Payload: Algun t_cliente
	FALLO_AL_RECIBIR,//Indica que un mensaje no se recibio correctamente en prot_recibir_mensaje
	DESCONEXION, //Indica que un cliente se ha desconectado (usado para instancias)
	FALLO_AL_ENVIAR,//Todavia no aplicado. Analogo al de arriba

	EJECUTAR_SCRIPTS, //Orden del planificador al ESI para que parsee y ejecute script
	EJECUCION_CORRECTA, //Respuesta del ESI al Planificador que la operacion se ejecuto bien
	EJECUCION_FALLO, //Respuesta del ESI al Planificador que la operacion se ejecuto bien
	RESULTADO_OPERACION_CORRECTA, //Respuesta del coordinador al ESI
	RESULTADO_OPERACION_FALLO, //Respuesta del coordinador al ESI
	SCRITP_FINALIZADO, //Mensaje del ESI al planificador luego de haber leido y parseado el script.

	/*Solicitudes del Coordinador al soporte del Planificador*/
	SOLICITUD_CLAVE_LIBRE, //Payload string de la clave
	SOLICITUD_CLAVE_BLOQUEADA_POR_ESI, //Payload string de la clave
	BLOQUEAR_ESI,//Orden del Coordinador para que bloquee el esi ejecutando. Payload: clave bloqueante

	/*Respuestas del Coordinador al soporte del Planificador*/
	RESPUESTA_CLAVE_LIBRE, //Planificador responde al coordinador que la clave esta libre.
	RESPUESTA_CLAVE_BLOQUEADA, ////Planificador responde al coordinador que la clave ya esta bloqueada.
	RESPUESTA_CLAVE_BLOQUEADA_POR_ESI_OK, //Respuesta del Planificador al Coordinador que indica que la clave se encuentra bloqueada por el ESI que la quiere persistir
	RESPUESTA_CLAVE_BLOQUEADA_POR_ESI_FALLO, //Respuesta del Planificador al Coordinador que indica que la clave NO se encuentra bloqueada por el ESI que la quiere persistir

	/*Para el GET*/
	SOLICITAR_CLAVE, //Payload: string de la clave
	CLAVE_LIBRE, //Respuesta del coordinador al esi
	CLAVE_YA_BLOQUEADA, //Respuesta del coordinador al esi

	/*Para el SET*/
	ASIGNAR_VALOR, //Mensaje del ESI al Coordinador. Payload: clave + valor

	/*Para es STORE*/
	PERSISTIR_CLAVE, //Payload: string de la clave

	/*Ordenes Coordinador - Planificador*/
	ABORTAR_ESI, //Sin payload. Bloquea ESI en ejecucion
	DESBLOQUEAR_CLAVE_DEL_ESI_EJECUTANDO, //Payload: string de la clave
	BLOQUEAR_CLAVE_PARA_ESI_EJECUTANDO, //Payload: string de la clave
	ORDEN_EJECUTADA, //Respuesta para que el coordinador sepa que ya hizo lo que se pidio

	/*Para Coordinador*/
	ENVIA_CONFIGURACIONES,

	/*Para Instancia*/
	GUARDAR_CLAVE,
	CLAVE_GUARDADA,
	CLAVE_NO_GUARDADA,
	SETEAR_VALOR,
	VALOR_SETEADO,
	VALOR_NO_SETEADO,
	STOREAR_CLAVE,
	CLAVE_STOREADA,
	CLAVE_NO_STOREADA,

	/*Para ESI*/
	NOMBRE,

	/*Planificador*/
	PIDE_STATUS,
	ENVIA_STATUS,

	/*Para dump*/
	DUMP_INICIADO,
	DUMP_TERMINADO
}t_header;

typedef struct{
	t_header head;
	size_t tamanio_total;
	void* payload;
}t_prot_mensaje;

//Para t_clientes
typedef enum{
	INSTANCIA,
	PLANIFICADOR,
	ESI
}t_cliente;

/**
* @NAME: prot_recibir_mensaje
* @DESC: retorna un mensaje recibido en el socket.
* 		 El mensaje tiene la siguiente forma: HEADER + PAYLOAD
* 		 Quien recibe el mensaje debe castearlo asi:
* 		 t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(un_socket);
* 		 t_header header_recibido = mensaje_recibido->head;
* 		 t_algo payload = *(t_algo*) mensaje_recibido->payload;
*
* @PARAMS:
* 		socket_origen - el nombre lo dice...
*/
t_prot_mensaje* prot_recibir_mensaje(int socket_origen);

/**
* @NAME: mensaje_error_al_recibir
* @DESC: retorno de un prot_recibir_mensaje si hay un error al recibir
*/
t_prot_mensaje* mensaje_error_al_recibir();

/**
* @NAME: mensaje_desconexion_al_recibir
* @DESC: retorno de un prot_recibir_mensaje si hay desconexion
*/
t_prot_mensaje* mensaje_desconexion_al_recibir();

/**
* @NAME: prot_enviar_mensaje
* @DESC: funcion SUPER GENERICA que resume todas las anteriores
* 		 Basicamente, le pones el socket destino, el HEADER que queres mandar, el sizeof del payload, el puntero al payload (osea, lo pasas con &) y te hace todo el trabajo solo
*		 Ejemplo: quiero mandar un int, con el header NUMERO -supongamos que existe
*		 int a = 8
*		 prot_enviar_mensaje( destino, NUMERO, sizeof(int), &a);
*		 LISTOOOO
* @PARAMS
* 		socket_destino - el nombre lo dice...
* 		header - el header
* 		tamanio_payload - el sizeof(tipo_de_lo_que_quiero_enviar)
* 		payload - &cosa_que_quiero_enviar
*/
void prot_enviar_mensaje(int socket_destino,t_header header , size_t tamanio_payload, void* payload);

//Hace los free
void prot_destruir_mensaje(t_prot_mensaje* victima);

/*Bien, la idea de esto es que cuando se haga un set sobre una clave
 * se deberia tratar a la misma como una sola estructura para que su manejo sea mas
 * sencillao: solo se enviaria un mensaje que tiene las cadenas (clave y valor) y sus
 * tamaños para hacer el parseo mas sencillo
 *
 * Buffer: tamanio_clave + tamanio_valor + clave + valor
 * */
void* prot_armar_payload_con_clave_valor(char* clave, char* valor);

/*Para cuando el payload es un string simple.
 * Lo lee y retorna el char* al mensaje
 * Hace el malloc dentro*/
char* leer_string_de_mensaje(t_prot_mensaje* mensaje);

#endif /* PROTOCOLO_H_ */
