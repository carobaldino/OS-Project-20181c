#include "operaciones.h"


void OPERACION_GET(){
	printf("GET\tclave: <%s>\n", proxima_operacion.argumentos.GET.clave);
	/*Un GET es una operacion que bloquea una clave
	 * es necesario enviar un mensaje al coordinador para bloquear una clave.
	 * El coordinador, a su vez, se la debe pedir al planificador. Si la misma
	 * encuentra bloqueada, debe mandar el ESI a bloqueados*/

	//1- Pedirle el clave la coordinador
	prot_enviar_mensaje(socket_coordinador, SOLICITAR_CLAVE, strlen(proxima_operacion.argumentos.GET.clave), proxima_operacion.argumentos.GET.clave );

	//2- El coordinador debe retornarme si la clave esta libre/bloqueada o si no se pudo
	resultado_operacion = prot_recibir_mensaje(socket_coordinador);
	switch(resultado_operacion->head){
		case CLAVE_LIBRE:
			//3.1 Si salio bien entonces ya no hay operacion pendiente
			printf("La clave <%s> se bloqueo\n", proxima_operacion.argumentos.GET.clave);
			hay_operacion_pendiente = false;
			prot_destruir_mensaje(resultado_operacion);
			destruir_operacion(proxima_operacion);
			prot_enviar_mensaje(socket_planificador, EJECUCION_CORRECTA, 0, NULL);
			break;

		case CLAVE_YA_BLOQUEADA:
			//3.2 Si salio mal entonces el planificador me bloquea y yo tengo una operacion pendiente
			printf("La clave <%s> ya fue bloqueda\n", proxima_operacion.argumentos.GET.clave);
			hay_operacion_pendiente = true;
			prot_destruir_mensaje(resultado_operacion);
			prot_enviar_mensaje(socket_planificador, EJECUCION_CORRECTA, 0, NULL);
			break;

		case RESULTADO_OPERACION_FALLO:
			printf("El Coordinador no pudo ejecutar la operacion. Abortanto ESI...\n");
			terminar_operacion(EJECUCION_FALLO);
			break;

		case FALLO_AL_RECIBIR: case DESCONEXION:
			printf("Fallo al recibir el resultado del Coordinador. Abortando ESI...\n");
			terminar_operacion(EJECUCION_FALLO);
			break;

		default:
			printf("Respuesta del Coordinador no reconocida. Abortando ESI...\n");
			terminar_operacion(EJECUCION_FALLO);
			break;
		}
}

void OPERACION_SET(){
	printf("SET\tclave: <%s>\tvalor: <%s>\n", proxima_operacion.argumentos.SET.clave, proxima_operacion.argumentos.SET.valor);
	/*Un SET es una operacion que le da un valor a una clave
	 * La clave debio haber sido bloqueada previamente por este mismo ESI */

	//1- Enviar al Coordinador la instruccion de SET con clave + valor
	//El payload de este mensaje tiene un tipo tamanio_clave + tamanio_valor + clave + valor
	size_t tamanio_payload = strlen(proxima_operacion.argumentos.SET.clave) + strlen(proxima_operacion.argumentos.SET.valor)  + sizeof(size_t)*2;
	void* payload = prot_armar_payload_con_clave_valor(proxima_operacion.argumentos.SET.clave, proxima_operacion.argumentos.SET.valor);

	prot_enviar_mensaje(socket_coordinador, ASIGNAR_VALOR, tamanio_payload, payload);
	free(payload);
	//Arme estas dos variables mas porque meter todo eso como parametro se me hacia complicado de leer

	//2- El Coordinador responde si la asignacion se realizo de forma correcta, en caso de que no se aborta el esi
	resultado_operacion = prot_recibir_mensaje(socket_coordinador);

	switch(resultado_operacion->head){
		case RESULTADO_OPERACION_CORRECTA:
			printf("se pudo asignor el valor <%s> a la clave <%s> correctamente\n",proxima_operacion.argumentos.SET.valor, proxima_operacion.argumentos.SET.clave );
			hay_operacion_pendiente = false;
			terminar_operacion(EJECUCION_CORRECTA);
			break;

		case RESULTADO_OPERACION_FALLO:
			printf("No se pudo asignar el valor <%s> a la clave <%s>. Abortando esi...\n",proxima_operacion.argumentos.SET.valor, proxima_operacion.argumentos.SET.clave );
			terminar_operacion(EJECUCION_FALLO);
			break;

		case FALLO_AL_RECIBIR: case DESCONEXION:
			printf("Fallo al recibir el resultado del Planificador. Abortando ESI...\n");
			terminar_operacion(EJECUCION_FALLO);
			break;

		default:
			printf("Respuesta del Planificador no reconocida. Abortando ESI...\n");
			terminar_operacion(EJECUCION_FALLO);
			break;
		}
}

void OPERACION_STORE(){
	printf("STORE\tclave: <%s>\n", proxima_operacion.argumentos.STORE.clave);
	/*Un STORE es un operacion que persiste una clave
	 * La clave debio haber sido bloqueada previamente por este mismo ESI
	 * Para verificar esto el cooirdinador le consulta al planificador
	 * Si no la posee se debe abortar el ESI y avisar al usuario
	 */

	//2- Enviar al coordinador la orden de persistir valor
	prot_enviar_mensaje(socket_coordinador, PERSISTIR_CLAVE, strlen(proxima_operacion.argumentos.STORE.clave), proxima_operacion.argumentos.STORE.clave );

	//3- El Coordinador retornara un mensaje si se pudo persistir el valor o hubo un error
	resultado_operacion = prot_recibir_mensaje(socket_coordinador);

	switch(resultado_operacion->head){
	case RESULTADO_OPERACION_CORRECTA:
		printf("Clave <%s> persistida\n", proxima_operacion.argumentos.STORE.clave);
		hay_operacion_pendiente = false;
		terminar_operacion(EJECUCION_CORRECTA);
		break;

	case RESULTADO_OPERACION_FALLO:
		printf("No se pudo persistir la clave <%s>\n", proxima_operacion.argumentos.STORE.clave);
		terminar_operacion(EJECUCION_FALLO);
		break;

	case FALLO_AL_RECIBIR: case DESCONEXION:
		printf("Fallo al recibir el resultado del Coordinador. Abortando ESI...\n");
		terminar_operacion(EJECUCION_FALLO);
		break;

	default :
		printf("Respuesta del Coordinador no reconocida. Abortando ESI...\n");
		terminar_operacion(EJECUCION_FALLO);
		break;
	}
}

void terminar_operacion(t_header resultado){
	prot_enviar_mensaje(socket_planificador, resultado, 0, NULL);
	prot_destruir_mensaje(resultado_operacion);
	destruir_operacion(proxima_operacion);
	if(resultado == EJECUCION_FALLO){
		finalizar(FINALIZAR_CON_ERROR);
	}
}

void finalizar(int retorno){
	close(socket_coordinador);
	close(socket_planificador);
	free(linea);
	fclose(script_a_pasear);
	exit(retorno);
}
