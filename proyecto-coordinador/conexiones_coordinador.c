#include "conexiones_coordinador.h"

void aceptar_clientes_coordinador(){
	int socket_cliente;
	struct sockaddr_in direccion_cliente;
	unsigned int tamanio_direccion = sizeof(direccion_cliente);
	//Burocracia...

	//Se aceptan clientes cuando los haya
	// accept es una funcion bloqueante, si no hay ningun cliente esperando ser atendido, se queda esperando a que venga uno.
	while(  (socket_cliente = accept(socket_escucha, (void*) &direccion_cliente, &tamanio_direccion)) > 0){
		//Se recibe un mensaje del cliente
		t_prot_mensaje* mensaje_del_cliente = prot_recibir_mensaje(socket_cliente);
		//Se -castea- el mensaje. Sabemos que va a enviar un cliente por eso ponemos t_cliente
		t_header header_recibido = mensaje_del_cliente->head;
		t_cliente cliente_recibido = *(t_cliente*) mensaje_del_cliente->payload;

		switch(cliente_recibido){
		case ESI:{
			log_info(logger, "[Conexiones]: Recibi un ESI en mi socket: %d",socket_cliente);
			pthread_t RecibirMensajesEsi;
			/*Duplico la variable que tiene el valor del socket del cliente*/
			int* esi = (int*) malloc (sizeof(int));
			*esi = socket_cliente;
			pthread_create(&RecibirMensajesEsi,NULL, (void*)escuchar_esi, esi);
			break;
		}

		case PLANIFICADOR:{
			log_info(logger, "[Conexiones]: Recibi un Planificador en mi socket: %d",socket_cliente);
			socket_planificador = socket_cliente;
			pthread_t RecibirMensajesPlanificador;
			pthread_create(&RecibirMensajesPlanificador, NULL ,(void*) escuchar_planificador, NULL);
			break;
		}

		case INSTANCIA:{
			int* socket_instancia = (int*) malloc (sizeof(int));
			*socket_instancia = socket_cliente;
			pthread_t RecibirMensajesInstancias;
			pthread_create (&RecibirMensajesInstancias,NULL, (void*) escuchar_instancia, socket_instancia);
			break;
		}

		default:
			log_warning(logger, "[Conexiones]: Recibi un cliente no identidicado en mi socket: %d",socket_cliente);
			close(socket_cliente);
			break;
		}

		//ACA SE HACEN LOS FREE
		prot_destruir_mensaje(mensaje_del_cliente);
	}

	log_error(logger,"[Conexiones]: Error al aceptar clientes. Se cayo el servidor");
	close(socket_escucha);
	close(socket_cliente);
}

void escuchar_planificador(){
	while(1){
		mensaje_planificador = prot_recibir_mensaje(socket_planificador);
		switch(mensaje_planificador->head){
			case DESCONEXION: case FALLO_AL_RECIBIR:{
				printf("[Conexiones]: Desconexion del planificador. Cerrando sistema\n");
				log_error(logger, "[Conexiones]: Desconexion del planificador. Cerrando sistema");
				exit(0);
				break;
			}

			case PIDE_STATUS:
				pthread_mutex_lock(&mutex_instancia);
				pthread_mutex_lock(&mutex_registro);
				char* clave = leer_string_de_mensaje(mensaje_planificador);
				prot_destruir_mensaje(mensaje_planificador);
				printf("[Planificador]: Solicita status de clave %s\n", clave );
				t_registro_claves* registro = buscar_clave_en_registro(clave);
				char* instancia_en_la_que_se_encuentra;
				char* valor;
				char* instancia_en_la_que_iria_actualmente;

				if(registro != NULL){
					instancia_en_la_que_se_encuentra = registro->instancia->nombre;
					if(registro->valor != NULL){
						valor = strdup(registro->valor);
					}else{
						valor = "No se ha asignado un valor";
					}
				}else{
					instancia_en_la_que_se_encuentra = "No se encuentra en ninguna instancia";
					valor = "No tiene valor";
				}

				instancia_en_la_que_iria_actualmente = seleccionar_instancia_para_distribuir_clave(clave)->nombre;

				if(instancia_en_la_que_iria_actualmente == NULL){
					instancia_en_la_que_iria_actualmente = "No hay instancias conectadas";
				}

				size_t tamanio_payload = sizeof(size_t)*3 + strlen(instancia_en_la_que_se_encuentra) + strlen(valor) + strlen(instancia_en_la_que_iria_actualmente);
				void* payload = malloc(tamanio_payload);
				memset(payload, 0, tamanio_payload);
				size_t bytes_copiados = 0;

				size_t largo_instancia_actual = strlen(instancia_en_la_que_se_encuentra);
				memcpy(payload, &largo_instancia_actual, sizeof(size_t));
				bytes_copiados += sizeof(size_t);

				size_t largo_valor = strlen(valor);
				memcpy(payload + bytes_copiados, &largo_valor, sizeof(size_t));
				bytes_copiados += sizeof(size_t);

				size_t largo_instancia_en_la_que_iria = strlen(instancia_en_la_que_iria_actualmente);
				memcpy(payload + bytes_copiados, &largo_instancia_en_la_que_iria, sizeof(size_t));
				bytes_copiados += sizeof(size_t);

				memcpy(payload + bytes_copiados, instancia_en_la_que_se_encuentra, largo_instancia_actual);
				bytes_copiados += largo_instancia_actual;

				memcpy(payload + bytes_copiados, valor, largo_valor);
				bytes_copiados += largo_valor;

				memcpy(payload + bytes_copiados, instancia_en_la_que_iria_actualmente, largo_instancia_en_la_que_iria);

				prot_enviar_mensaje(socket_planificador, ENVIA_STATUS, tamanio_payload, payload);

				free(payload);
				free(instancia_en_la_que_iria_actualmente);
				free(instancia_en_la_que_se_encuentra);
				free(valor);
				free(clave);

				pthread_mutex_unlock(&mutex_instancia);
				pthread_mutex_unlock(&mutex_registro);
				break;

			default:
				sem_post(&productor_consumidor_mensajes_planificador);
				break;
		}
	}
}

void escuchar_esi(int *socket_origen){
	int socket_esi = *socket_origen;
	free(socket_origen);
	t_prot_mensaje* mensaje_del_esi;
	bool sigue_ejecutando = true;

	while(sigue_ejecutando){
		mensaje_del_esi = prot_recibir_mensaje(socket_esi);
		pthread_mutex_lock(&mutex_mensaje_planificador);
		usleep(config_info.retardo_miliseg * 1000);
		printf("\n[ESI]:Mensaje recibido de ESI\n");
		/*Cada cada funcion retorna si se el ESI se debe seguir ejecutando o no*/
		switch(mensaje_del_esi->head){
			case SOLICITAR_CLAVE:
				sigue_ejecutando = solicitar_clave(socket_esi, mensaje_del_esi);
				break;
			case ASIGNAR_VALOR:
				sigue_ejecutando = asignar_valor(socket_esi, mensaje_del_esi);
				break;
			case PERSISTIR_CLAVE:
				sigue_ejecutando = persistir_clave(socket_esi, mensaje_del_esi);
				break;
			case DESCONEXION: case FALLO_AL_RECIBIR: default:
				sigue_ejecutando = false;
				printf("El ESI se desconecto\n");
				break;
		}
		pthread_mutex_unlock(&mutex_mensaje_planificador);
	}//Fin del while
	close(socket_esi);
}

bool solicitar_clave(int socket_esi, t_prot_mensaje* mensaje_del_esi){
	bool resultado_operacion;
	char* clave = leer_string_de_mensaje(mensaje_del_esi);
	size_t largo_clave = strlen(clave);
	printf("[GET]: GET de clave: %s\n",clave);
	log_info(logger, "[GET]: GET de clave %s",clave);

	if(largo_clave >= LARGO_MAXIMO_CLAVE){
		printf("[GET]: Clave muy larga. Operacion fallida\n");
		log_info(logger, "[GET]: Clave muy larga. Operacion fallida");
		prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
		printf("[GET]: Fin\n");
		resultado_operacion = false;

	}else{

		printf("[GET]: Consulto al Planificador si la clave <%s> esta libre\n", clave);
		prot_enviar_mensaje(socket_planificador, SOLICITUD_CLAVE_LIBRE, largo_clave, clave);
		//El planificador me responde para asi poder empezar a planificar
		sem_wait(&productor_consumidor_mensajes_planificador);

		switch(mensaje_planificador->head){

			case RESPUESTA_CLAVE_LIBRE:
				printf("[GET]: Planificador responde que la clave esta libre\n");
				prot_destruir_mensaje(mensaje_planificador);

				if(aplicar_algoritmo_distribucion(clave)){ //Si el algoritmo de distribucion pudo guardar la clavE correctamente.. CAMBIAR CON ALGORITMO
					printf("[GET]: Ordeno al Planificador que bloquee la clave <%s>\n", clave);
					prot_enviar_mensaje(socket_planificador, BLOQUEAR_CLAVE_PARA_ESI_EJECUTANDO, largo_clave, clave);
					sem_wait(&productor_consumidor_mensajes_planificador);

					if(mensaje_planificador->head == ORDEN_EJECUTADA){
						printf("[GET]: El Planificador bloqueo la clave <%s> para el ESI. Operracion exitosa\n", clave);
						prot_enviar_mensaje(socket_esi, CLAVE_LIBRE, 0, NULL);
						printf("[GET]: Fin\n");
						resultado_operacion = true;

					}else{
						printf("[GET]: Error al bloquear la clave <%s> para el ESI. Operacion fallida\n", clave);
						prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
						printf("[GET]: Fin\n");
						resultado_operacion = false;
					}

				}else{
					printf("[GET]: Error en la instancia. Operacion fallida\n");
					prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
					printf("[GET]: Fin\n");
					resultado_operacion = false;
				}
				break;

			case RESPUESTA_CLAVE_BLOQUEADA:
				prot_destruir_mensaje(mensaje_planificador);
				printf("[GET]: Planificador responde que la clave esta bloqueada\n");
				printf("[GET]: Ordeno al planificador que bloquee al ESI\n");
				prot_enviar_mensaje(socket_planificador, BLOQUEAR_ESI, largo_clave, clave);
				sem_wait(&productor_consumidor_mensajes_planificador);

				if(mensaje_planificador->head == ORDEN_EJECUTADA){
					printf("[GET]: El Planificador bloqueo al ESI. Operracion exitosa\n");
					prot_enviar_mensaje(socket_esi, CLAVE_YA_BLOQUEADA, 0, NULL);
					printf("[GET]: Fin\n");
					resultado_operacion = true;
				}else{
					printf("[GET]: Error al desbloquear la clave <%s> para el ESI. Operacion fallida\n", clave);
					prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
					printf("[GET]: Fin\n");
					resultado_operacion = false;
				}

				break;

			case DESCONEXION: case FALLO_AL_RECIBIR: default:
				printf("[GET]: Error en el Planificador. Operacion fallida\n");
				prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
				printf("[GET]: Fin\n");
				resultado_operacion = false;
		}
	}

	free(clave);
	prot_destruir_mensaje(mensaje_planificador);
	prot_destruir_mensaje(mensaje_del_esi);
	return resultado_operacion;
}

bool asignar_valor(int socket_esi, t_prot_mensaje* mensaje_del_esi){
	bool resultado_operacion;

	size_t largo_clave;
	size_t largo_valor;

	memset(&largo_clave, 0, sizeof(size_t));
	memset(&largo_valor, 0, sizeof(size_t));

	memcpy(&largo_clave, mensaje_del_esi->payload, sizeof(size_t));
	memcpy(&largo_valor, mensaje_del_esi->payload + sizeof(size_t), sizeof(size_t));

	char* clave = (char*) malloc (largo_clave + 1);
	char* valor = (char*) malloc (largo_valor + 1);

	memcpy(clave, mensaje_del_esi->payload + sizeof(size_t)*2, largo_clave);
	memcpy(valor, mensaje_del_esi->payload + sizeof(size_t)*2 + largo_clave, largo_valor);

	clave[largo_clave] = '\0';
	valor[largo_valor] = '\0';

	printf("[SET]: SET de la clave <%s> con el valor <%s>\n", clave, valor);

	if( strlen(clave) >= LARGO_MAXIMO_CLAVE ){
		printf("[SET]: Clave muy larga. Operacion fallida\n");
		prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
		printf("[SET]: Fin\n");
		resultado_operacion = false;
	}else{

		printf("[SET]: Consulto al Planificador si el ESI tiene la clave <%s> bloqueada\n", clave);
		prot_enviar_mensaje(socket_planificador, SOLICITUD_CLAVE_BLOQUEADA_POR_ESI, strlen(clave), clave);
		sem_wait(&productor_consumidor_mensajes_planificador);

		switch(mensaje_planificador->head){
			case RESPUESTA_CLAVE_BLOQUEADA_POR_ESI_OK:
				printf("[SET]: El Planificador responde que el ESI tiene la clave <%s> bloqueada\n", clave);

				if(set_valor(clave, valor)){
					printf("[SET]: Se seteo el valor <%s> para la clave <%s>. Operracion exitosa\n", valor, clave);
					prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_CORRECTA, 0, NULL);
					printf("[SET]: Fin\n");
					resultado_operacion = true;
				}else{
					printf("[SET]: Error en la instancia. Operacion fallida\n");
					prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
					printf("[SET]: Fin\n");
					resultado_operacion = false;
				}
				break;

			case RESPUESTA_CLAVE_BLOQUEADA_POR_ESI_FALLO:
				printf("[SET]: El Planificador responde que la clave <%s> no esta bloqueada por ESI. Operacion fallida\n", clave);
				prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
				printf("[SET]: Fin\n");
				resultado_operacion = false;
				break;

			case FALLO_AL_RECIBIR: case DESCONEXION: default:
				printf("[SET]: Fallo en el Planificador. Operacion fallida\n");
				prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
				resultado_operacion = false;
				break;
		}
	}

	free(clave);
	free(valor);
	prot_destruir_mensaje(mensaje_planificador);
	prot_destruir_mensaje(mensaje_del_esi);
	return resultado_operacion;
}

bool persistir_clave(int socket_esi, t_prot_mensaje* mensaje_del_esi){
	bool resultado_operacion;
	char* clave = leer_string_de_mensaje(mensaje_del_esi);
	printf("[STORE]: STORE de la clave <%s>\n", clave);

	if( strlen(clave) >= LARGO_MAXIMO_CLAVE ){
		printf("[STORE]: Clave muy larga. Operacion fallida\n");
		prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
		printf("[STORE]: Fin\n");
		resultado_operacion = false;
	}else{

		printf("[STORE]: Consulto al Planificador si el ESI tiene la clave <%s> bloqueada\n", clave);
		prot_enviar_mensaje(socket_planificador, SOLICITUD_CLAVE_BLOQUEADA_POR_ESI, strlen(clave), clave);
		sem_wait(&productor_consumidor_mensajes_planificador);

		switch(mensaje_planificador->head){
			case RESPUESTA_CLAVE_BLOQUEADA_POR_ESI_OK:
				prot_destruir_mensaje(mensaje_planificador);
				printf("[STORE]: Planificador responde que la clave esta libre\n");

				if(store_clave(clave)){ //Store clave
					printf("[STORE]: Ordeno al Planificador que desbloquee la clave <%s>\n", clave);
					prot_enviar_mensaje(socket_planificador, DESBLOQUEAR_CLAVE_DEL_ESI_EJECUTANDO, strlen(clave), clave);
					sem_wait(&productor_consumidor_mensajes_planificador);

					if(mensaje_planificador->head == ORDEN_EJECUTADA){
						printf("[STORE]: El Planificador desbloqueo la clave <%s> para el ESI. Operracion exitosa\n", clave);
						prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_CORRECTA, 0, NULL);
						printf("[STORE]: Fin\n");
						resultado_operacion = true;
					}else{
						printf("[STORE]: Error al desbloquear la clave <%s> para el ESI. Operacion fallida\n", clave);
						prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
						printf("[STORE]: Fin\n");
						resultado_operacion = false;
					}

				}else{
					printf("[STORE]: Error en la instancia. Operacion fallida\n");
					prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
					printf("[STORE]: Fin\n");
					resultado_operacion = false;
				}
				break;

			case RESPUESTA_CLAVE_BLOQUEADA_POR_ESI_FALLO:
				printf("[STORE]: El Planificador responde que la clave no esta bloqueada por ESI. Operacion fallida\n");
				prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
				printf("[STORE]: Fin\n");
				resultado_operacion = false;
				break;

			case FALLO_AL_RECIBIR: case DESCONEXION: default:
				printf("[STORE]: Fallo en el Planificador. Operacion fallida\n");
				prot_enviar_mensaje(socket_esi, RESULTADO_OPERACION_FALLO, 0, NULL);
				resultado_operacion = false;
				break;
		}
	}

	free(clave);
	prot_destruir_mensaje(mensaje_planificador);
	prot_destruir_mensaje(mensaje_del_esi);
	return resultado_operacion;
}

bool set_valor(char* clave, char* valor){
	pthread_mutex_lock(&mutex_registro);
	t_registro_claves* registro = buscar_clave_en_registro(clave);
	pthread_mutex_unlock(&mutex_registro);
	t_instancia* instancia_con_clave = registro->instancia;
	if(instancia_con_clave != NULL){
		void* clave_valor = prot_armar_payload_con_clave_valor(clave, valor);
		size_t tamnio_clave_valor = strlen(clave) + strlen(valor)  + sizeof(size_t)*2;
		prot_enviar_mensaje(instancia_con_clave->socket_instancia, SETEAR_VALOR, tamnio_clave_valor, clave_valor);
		free(clave_valor);
		sem_wait(instancia_con_clave->productor_consumidor_de_esta_instancia);
		printf("[STORE-Instancias]: SET clave <%s> en instancia <%s>\n", clave, instancia_con_clave->nombre);
		switch(instancia_con_clave->mensaje_instancia->head){
			case VALOR_SETEADO:
				log_info(logger, "[SET-Instancia]: Asignado valor <%s> a la clave <%s> en la instancia <%s>", valor, clave, instancia_con_clave->nombre);
				prot_destruir_mensaje(instancia_con_clave->mensaje_instancia);
				free(registro->valor);
				registro->valor = strdup(valor);
				return true;
				break;

			case DESCONEXION: case FALLO_AL_RECIBIR:
				log_warning(logger,"[SET-Instancia]: Desconexion de la instancia <%s>", instancia_con_clave->nombre);
				prot_destruir_mensaje(instancia_con_clave->mensaje_instancia);
				return false;
				break;
			default:
				log_warning(logger,"[SET-Instancia]: No se reconoce respuesta de la instancia <%s>", instancia_con_clave->nombre);
				prot_destruir_mensaje(instancia_con_clave->mensaje_instancia);
				return false;
		}

	}else{
		return false;
	}
}

bool store_clave(char* clave){
	pthread_mutex_lock(&mutex_registro);
	t_registro_claves* registro = buscar_clave_en_registro(clave);
	pthread_mutex_unlock(&mutex_registro);
	t_instancia* instancia_con_clave = registro->instancia;
	if(instancia_con_clave != NULL){
		prot_enviar_mensaje(instancia_con_clave->socket_instancia, STOREAR_CLAVE, strlen(clave), clave);
		sem_wait(instancia_con_clave->productor_consumidor_de_esta_instancia);
		printf("[STORE-Instancias]: STORE clave <%s> en instancia <%s>\n", clave, instancia_con_clave->nombre);
		switch(instancia_con_clave->mensaje_instancia->head){
			case CLAVE_STOREADA:
				log_info(logger, "[STORE-Instancia]: Store de la clave <%s> en la instancia <%s>", clave, instancia_con_clave->nombre);
				prot_destruir_mensaje(instancia_con_clave->mensaje_instancia);

				printf("clave: %s\n", clave);
				bool es_clave_buscada(void* clave_distribuida){
					return strcmp(clave, ((t_registro_claves*)clave_distribuida)->clave ) == 0;
				}
				printf("clave: %s\n", clave);

				pthread_mutex_lock(&mutex_registro);
				list_remove_by_condition(registro_claves, &es_clave_buscada);
				pthread_mutex_unlock(&mutex_registro);

				free(registro->clave);
//				free(registro->valor);
				free(registro);

				return true;
				break;

			case DESCONEXION: case FALLO_AL_RECIBIR:
				log_warning(logger,"[STORE-Instancia]: Desconexion de la instancia <%s>", instancia_con_clave->nombre);
				prot_destruir_mensaje(instancia_con_clave->mensaje_instancia);
				return false;
				break;
			default:
				log_warning(logger,"[STORE-Instancia]: No se reconoce respuesta de la instancia <%s>", instancia_con_clave->nombre);
				prot_destruir_mensaje(instancia_con_clave->mensaje_instancia);
				return false;
		}

	}else{
		return false;
	}
}

t_registro_claves* buscar_clave_en_registro(char* clave){
	t_registro_claves* retorno = NULL;

	bool es_clave_buscada(void* clave_distribuida){
		return strcmp(clave, ((t_registro_claves*)clave_distribuida)->clave ) == 0;
	}

	if(!list_is_empty(registro_claves)){
		return (t_registro_claves*)list_find(registro_claves, &es_clave_buscada);
	}else{
		return NULL;
	}
}

void escuchar_instancia(int *socket_origen) {
	int socket_instancia = *socket_origen;
	free(socket_origen);
	sem_t* productor_consumidor;
	prot_enviar_mensaje(socket_instancia, ENVIA_CONFIGURACIONES, sizeof(t_tabla_instancias), &tabla_instancias);

	t_instancia* instancia = (t_instancia*) malloc(sizeof(t_instancia));
	instancia->espacio_memoria_total = tabla_instancias.cantidad_entradas * tabla_instancias.cantidad_entradas;
	instancia->espacio_utilizado = 0;
	instancia->socket_instancia = socket_instancia;
	productor_consumidor = (sem_t*) malloc(sizeof(sem_t));
	instancia->productor_consumidor_de_esta_instancia = productor_consumidor;
	sem_init(productor_consumidor, 0, 0);
	bool instancia_conectada = true;

	instancia->mensaje_instancia = prot_recibir_mensaje(socket_instancia);
	if(instancia->mensaje_instancia->head == NOMBRE){
		instancia->nombre = leer_string_de_mensaje(instancia->mensaje_instancia);
		log_info(logger, "[Instancia]: Se conecto la instancia en socket <%s>", instancia->nombre);
		printf("Se conecto la instancia %s \n", instancia->nombre);
		prot_destruir_mensaje(instancia->mensaje_instancia);
	}
	else{
		instancia_conectada = false;
	}

	pthread_mutex_lock(&mutex_instancia);
	list_add(lista_instancias, instancia);
	pthread_mutex_unlock(&mutex_instancia);

	while(instancia_conectada){
		instancia->mensaje_instancia = prot_recibir_mensaje(socket_instancia);
		switch(instancia->mensaje_instancia->head){
			case CLAVE_GUARDADA:
				sem_post(instancia->productor_consumidor_de_esta_instancia);
				break;

			case VALOR_SETEADO:
				sem_post(instancia->productor_consumidor_de_esta_instancia);
				break;

			case CLAVE_STOREADA:
				sem_post(instancia->productor_consumidor_de_esta_instancia);
				break;

			case CLAVE_NO_GUARDADA:
				sem_post(instancia->productor_consumidor_de_esta_instancia);
				break;

			case VALOR_NO_SETEADO:
				sem_post(instancia->productor_consumidor_de_esta_instancia);
				break;

			case CLAVE_NO_STOREADA:
				sem_post(instancia->productor_consumidor_de_esta_instancia);
				break;

			case DESCONEXION: case FALLO_AL_RECIBIR:
				printf("[Instancia]: No se reconoce el mensaje\n");
				prot_destruir_mensaje(instancia->mensaje_instancia);
				instancia_conectada = false;
				break;

			default:
				prot_destruir_mensaje(instancia->mensaje_instancia);
				break;
		}
	}

	printf("[Instancia]: Cerrando instancia %s\n", instancia->nombre);
	log_info(logger, "[Instancia]: Cerrando instancia %s", instancia->nombre);
	bool es_instancia(void* instancia_en_lista){
		return ((t_instancia*)instancia_en_lista)->socket_instancia == instancia->socket_instancia;
	}
	pthread_mutex_lock(&mutex_instancia);
	list_remove_by_condition(lista_instancias, &es_instancia);
	pthread_mutex_unlock(&mutex_instancia);

	free(instancia->nombre);
	prot_destruir_mensaje(instancia->mensaje_instancia);
	free(instancia->productor_consumidor_de_esta_instancia);
	close(socket_instancia);
	pthread_exit(0);

	/*Esto genera leak por no joinearlo a nada*/
}

void printear_instancia(void* instancia){
	printf( "Instancia: %s\n" ,((t_instancia*)instancia)->nombre );
}
