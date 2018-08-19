#include "interfaz_planificador_coordinador.h"

void soporte(){
	while(!cerrando){
		solicitud_coordinador = prot_recibir_mensaje(socket_coordinador);
		if(modo_debug){
			printf("[Soporte]: Llego solicitud del Coordinador\n");
		}
		switch(solicitud_coordinador->head){

			case SOLICITUD_CLAVE_LIBRE:{
				char* clave_solicitada = leer_string_de_mensaje(solicitud_coordinador);
				if(modo_debug){
					printf("[Soporte]: Consulta si la clave <%s> esta libre\n", clave_solicitada);
				}
				//Busco la clave entre las bloqueadas
				bool es_clave_buscada(void* clave){
					return strcmp( (char*) clave, clave_solicitada) == 0;
				}
				if( !list_any_satisfy(claves_bloqueadas, &es_clave_buscada) ){
					//Si esta libre se le dice al coordianador que esta libre
					prot_enviar_mensaje(socket_coordinador, RESPUESTA_CLAVE_LIBRE, 0, NULL);
					if(modo_debug){
						printf("[Soporte]: La clave esta libre: <%s>. Le respondo al Coordinador\n", clave_solicitada);
					}
				}else{
					//Si esta bloqueada se avisa de esto al plani
					prot_enviar_mensaje(socket_coordinador, RESPUESTA_CLAVE_BLOQUEADA, 0, NULL);
					if(modo_debug){
						printf("[Soporte]: La clave esta bloqueada: <%s>. Le respondo al Coordinador\n", clave_solicitada);
					}
				}
				free(clave_solicitada);
				break;
			}


			case BLOQUEAR_ESI:{
				char* clave_bloqueante = leer_string_de_mensaje(solicitud_coordinador);
				if(modo_debug){
					printf("[Soporte]: Orden de bloquear ESI: <%d> con la clave <%s>\n", esi_a_ejecutar->ID, clave_bloqueante);
				}
				ejecutando_a_bloqueado(esi_a_ejecutar, clave_bloqueante);
				if(modo_debug){
					printf("[Soporte]: ESI: <%d> bloqueado con la clave <%s>\n", esi_a_ejecutar->ID, clave_bloqueante);
				}
				prot_enviar_mensaje(socket_coordinador, ORDEN_EJECUTADA, 0, NULL);
				if(modo_debug){
					printf("[Soporte]: Aviso que se ejecuto la orden de bloqueo\n");
				}
				free(clave_bloqueante);
				break;
			}

			case BLOQUEAR_CLAVE_PARA_ESI_EJECUTANDO:{
				char* clave = leer_string_de_mensaje(solicitud_coordinador);
				if(modo_debug){
					printf("[Soporte]: Orden de bloquear clave <%s> para es ESI <%d>\n", clave ,esi_a_ejecutar->ID);
				}
				bloquear_clave_para_esi_ejecutando(clave);
				prot_enviar_mensaje(socket_coordinador, ORDEN_EJECUTADA, 0, NULL);
				if(modo_debug){
					printf("[Soporte]: Aviso que se ejecuto la orden\n");
				}
				free(clave);
				break;
			}

			case SOLICITUD_CLAVE_BLOQUEADA_POR_ESI:{
				char* clave = leer_string_de_mensaje(solicitud_coordinador);
				if(modo_debug){
					printf("[Soporte]: Consulta si la clave <%s> esta bloqueada por ESI <%d>\n", clave, esi_a_ejecutar->ID);
				}
				bool es_clave_buscada(void* clave_de_lista){
					return strcmp( ((char*)clave_de_lista) , clave ) == 0;
				}
				if(list_any_satisfy(esi_a_ejecutar->claves_bloqueadas, &es_clave_buscada)){
					if(modo_debug){
						printf("[Soporte]: Respondo que la clave <%s> esta bloqueada por ESI <%d>\n", clave, esi_a_ejecutar->ID);
					}
					prot_enviar_mensaje(socket_coordinador, RESPUESTA_CLAVE_BLOQUEADA_POR_ESI_OK, 0, NULL);
				}else{
					if(modo_debug){
						printf("[Soporte]: Respondo que la clave <%s> NO esta bloqueada por ESI <%d>\n", clave, esi_a_ejecutar->ID);
					}
					prot_enviar_mensaje(socket_coordinador, RESPUESTA_CLAVE_BLOQUEADA_POR_ESI_FALLO, 0, NULL);
				}
				free(clave);
				break;
			}

			case DESBLOQUEAR_CLAVE_DEL_ESI_EJECUTANDO:{
				/*Esto es para el STORE. Se supone que se hace STORE sobre el ESI que esta ejecutando*/
				char* clave = leer_string_de_mensaje(solicitud_coordinador);
				if(modo_debug){
					printf("[Soporte]: Orden de desbloquear la clave <%s> del ESI <%d>\n", clave, esi_a_ejecutar->ID );
				}
				desbloquear_clave_de_esi(clave, esi_a_ejecutar);
				desbloquear_esi_bloqueados_por_clave(clave);
				if(modo_debug){
					printf("[Soporte]: Aviso que se ejecuto la orden\n");
				}
				prot_enviar_mensaje(socket_coordinador, ORDEN_EJECUTADA, 0, NULL);
				free(clave);
				break;
			}

			case FALLO_AL_RECIBIR:{
				prot_destruir_mensaje(solicitud_coordinador);
				if(modo_debug){
					printf("[Soporte]: Fallo al recibir mensaje del Coordinador\n");
				}
				finalizar("Error al recibir mensaje del Coordinador");
				break;
			}
				break;

			case ENVIA_STATUS:{
				size_t bytes_leidos = 0;

				size_t largo_instancia_acutal = 0;
				size_t largo_valor = 0;
				size_t largo_instancia_en_la_que_iria = 0;

				char* instancia_acutal;
				char* valor;
				char* instancia_en_la_que_iria;

				memcpy(&largo_instancia_acutal , solicitud_coordinador->payload + bytes_leidos, sizeof(size_t));
				bytes_leidos += sizeof(size_t);

				memcpy(&largo_valor , solicitud_coordinador->payload + bytes_leidos, sizeof(size_t));
				bytes_leidos += sizeof(size_t);

				memcpy(&largo_instancia_en_la_que_iria , solicitud_coordinador->payload + bytes_leidos, sizeof(size_t));
				bytes_leidos += sizeof(size_t);

				instancia_acutal = (char*) malloc(largo_instancia_acutal + 1);
				valor = (char*) malloc(largo_valor +1);
				instancia_en_la_que_iria = (char*) malloc(largo_instancia_en_la_que_iria + 1);

				memcpy(instancia_acutal , solicitud_coordinador->payload + bytes_leidos, largo_instancia_acutal);
				bytes_leidos += largo_instancia_acutal;

				memcpy(valor , solicitud_coordinador->payload + bytes_leidos, largo_valor);
				bytes_leidos += largo_valor;

				memcpy(instancia_en_la_que_iria , solicitud_coordinador->payload + bytes_leidos, largo_instancia_en_la_que_iria);

				instancia_acutal[largo_instancia_acutal] = '\0';
				valor[largo_valor] = '\0';
				instancia_en_la_que_iria[largo_instancia_en_la_que_iria] = '\0';

				prot_destruir_mensaje(solicitud_coordinador);

				printf("\tInstancia actual: <%s>\n", instancia_acutal);
				printf("\tValor: <%s>\n", valor);
				printf("\tInstancia en la que iria actualmente: <%s>\n\n", instancia_en_la_que_iria);
				free(instancia_acutal);
				free(instancia_en_la_que_iria);
				free(valor);
				sem_post(&producto_consumidor_status);
				break;
			}

			case DESCONEXION:{
				prot_destruir_mensaje(solicitud_coordinador);
				if(modo_debug){
					printf("[Soporte]: Desconexion del Coordinador\n");
				}
				finalizar("El Coordinador se desconecto");
				break;
			}
				break;
			default:
				if(modo_debug){
					printf("No se reconoce solicitud del Coordinador");
				}
				break;
		}
		prot_destruir_mensaje(solicitud_coordinador);
	}//Fin del While
	pthread_exit(0);
}
