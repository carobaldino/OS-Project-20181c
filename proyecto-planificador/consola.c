#include "consola.h"

void iniciar_consola(){
	bool comando_valido;
	bool comando_vacio = false;

	while(!cerrando){
		entrada = NULL;
		comando_valido = true;
		if(modo_debug){
			if(!comando_vacio){
				getchar();
			}
		}
		entrada = readline("Planificador> ");
		if(entrada){
			entrada[strlen(entrada)] = '\0';

			if(strlen(entrada) > 2){
				comando_vacio = false;
			}else{
				comando_vacio = true;
			}

			t_comando* comando = NULL;
			comando = parsear_entrada(entrada);

			pthread_t hilo_ejecucion_comando;
			if(comando != NULL){
				char* funcion = comando->argumentos[0];
				if( strcmp(funcion, "pausar") == 0 ){
					pausar(comando);

				}else
				if( strcmp(funcion, "continuar") == 0 ){
					continuar(comando);

				}else
				if( strcmp(funcion, "bloquear") == 0 ){
					pthread_create(&hilo_ejecucion_comando, NULL, (void*)bloquear, comando);

				}else
				if( strcmp(funcion, "desbloquear") == 0 ){
					pthread_create(&hilo_ejecucion_comando, NULL, (void*)desbloquear, comando);

				}else
				if( strcmp(funcion, "listar") == 0 ){
					pthread_create(&hilo_ejecucion_comando, NULL, (void*)listar, comando);

				}else
				if( strcmp(funcion, "kill") == 0 ){
					pthread_create(&hilo_ejecucion_comando, NULL, (void*)kill_consola, comando);

				}else
				if( strcmp(funcion, "status") == 0 ){
					pthread_create(&hilo_ejecucion_comando, NULL, (void*)status, comando);

				}else
				if( strcmp(funcion, "deadlock") == 0 ){
					pthread_create(&hilo_ejecucion_comando, NULL, (void*)deadlock, comando);

				}else
				if( strcmp(funcion, "salir") == 0 ){
					salir(comando);

				}else
				if( strcmp(funcion, "mostrar-estado") == 0){
					printear_todo(comando);

				}else
				if( strcmp(funcion, "debug") == 0){
					debug(comando);
				}else
				if( strcmp(funcion, "menu") == 0){
					mostrar_menu(comando);

				}else{
					printf("CONSOLA:\tComando '%s' no reconocido.\nIngrese 'menu' para mostrar comandos.\n", funcion);
					comando_valido = false;
					free_comando(comando);
				}

				if(comando_valido){
					add_history(entrada);
				}
			}

		}
	free(entrada);
	}//Fin del while
	pthread_exit(0);
}

t_comando* parsear_entrada(char* entrada){
	if(strlen(entrada) == 0){
		return NULL;
	}
	size_t cantidad_argumentos = 1;
	/*Por si la entrada empieza con espacios, se mueve el cursor hasta el primer caracter*/
	int indice;
	for(indice = 0; entrada[indice] == ' '; indice++);
	if(entrada[indice] == '\0'){
		return NULL;
	}
	/*Se lee cuantos argumentos hay
	 * Para esto se suma 1 cada vez que se encuentre una caracter despues de un espacio en blanco*/
	for(int indice_aux = indice; entrada[indice_aux+1] != '\0'; indice_aux++){
		if( (entrada[indice_aux] == ' ') && (entrada[indice_aux + 1] != ' ') ){
			cantidad_argumentos++;
		}
	}
	t_comando* retorno = (t_comando*) malloc(sizeof(retorno));
	retorno->argumentos = (char**) malloc(sizeof(char*) * cantidad_argumentos);

	for(int indice_argumento = 0; indice_argumento < cantidad_argumentos; indice_argumento++){
		for(; entrada[indice] == ' ' ; indice++);
		//Contar el largo del proximo argumento
		size_t largo_argumento = 0;
		for(int indice_aux = indice ; !((entrada[indice_aux]==' ')||(entrada[indice_aux]=='\0')); indice_aux++, largo_argumento++);
		retorno->argumentos[indice_argumento] = (char*) malloc (largo_argumento + 1);
		memset(retorno->argumentos[indice_argumento], 0, largo_argumento);
		memcpy(retorno->argumentos[indice_argumento], entrada + indice, largo_argumento);
		retorno->argumentos[indice_argumento][largo_argumento] = '\0';
		indice += largo_argumento + 1;
	}

	retorno->cantidad_argumentos = cantidad_argumentos;
	return retorno;
}

void bloquear(t_comando* comando){
	if(comando->cantidad_argumentos == 3){
		//arg[1] = clave
		//arg[2] = id
		/*Magia*/
		int id = atoi(comando->argumentos[2]);
		if(id != 0){
			char* clave = comando->argumentos[1];
			if(strlen(clave) <= LARGO_MAXIMO_CLAVE){
				//Si el comando y los argumentos estan bien...
				bool tiene_id_buscada(void* esi_en_lista){
					return ((t_esi*)esi_en_lista)->ID == id;
				}
				//Buscar en ejecutando

				pthread_mutex_lock(&mutex_listas);
				printf("\n[Consola-Bloquear]: Bloqueando...\n");
				t_esi* esi_a_bloquear = NULL;

				if( (esi_a_bloquear = list_find(ejecutando, &tiene_id_buscada)) != NULL ){
					/*Esta en ejecutando*/
					ejecutando_a_bloqueado(esi_a_bloquear, clave);
					printf("[Consola-Bloquear]: ESI <%d> (ejecutando) bloqueado con clave <%s>...\n",id, clave);
				}else
				if( (esi_a_bloquear = list_find(listos, &tiene_id_buscada)) != NULL ){
					/*Esta en listos*/
					listo_a_bloqueado(esi_a_bloquear, clave);
					printf("[Consola-Bloquear]: ESI <%d> (listo) bloqueado con clave <%s>...\n",id, clave);
				}else{
					printf("[Consola-Bloquear]: No se encontro un ESI con la ID:<%d>\n",id);
				}
				pthread_mutex_unlock(&mutex_listas);
			}else{
				printf("[Consola-Bloquear]: Clave muy larga\n");
			}
		}else{
			printf("[Consola-Bloquear]: ID <%s> no reconocido\n",comando->argumentos[2]);
		}
		/*Fin de magia*/
	}else{
		printf("Muchos argumentos\n");
	}
	free_comando(comando);
}

void desbloquear(t_comando* comando){
	if(comando->cantidad_argumentos == 2){
		//arg[1] = clave
		/*Magia*/
		//1- Verificar que la clave sea valida
		if(strlen(comando->argumentos[1]) <= LARGO_MAXIMO_CLAVE ){
			bool es_clave_buscada(void* clave_en_lista){
				return strcmp( (char*)clave_en_lista , comando->argumentos[1]) == 0;
			}
			pthread_mutex_lock(&mutex_listas);
			printf("\n[Consola-Bloquear]: Desbloqueando clave <%s>...\n", comando->argumentos[1]);
			desbloquear_esi_bloqueados_por_clave(comando->argumentos[1]);
			eliminar_clave_de_lista_de_claves_bloqueadas(comando->argumentos[1]);
			pthread_mutex_unlock(&mutex_listas);
		}else{
			printf("Clave muy larga\n");
		}
		/*Fin de magia*/
	}else{
		printf("Muchos argumentos\n");
	}
	free_comando(comando);
}

void pausar(t_comando* comando){
	if(comando->cantidad_argumentos == 1){
		/*Magia*/
		if(!pausado){
			pthread_mutex_lock(&mutex_pausar_continuar);
			pausado = true;
			printf("Planificacion pausada\n");
		}else{
			printf("Ya esta pausado\n");
		}
		/*Fin de magia*/
	}else{
		printf("Muchos argumentos\n");
	}
	free_comando(comando);
}

void continuar(t_comando* comando){
	if(comando->cantidad_argumentos == 1){
		/*Magia*/
		if(pausado){
			pthread_mutex_unlock(&mutex_pausar_continuar);
			pausado = false;
			printf("Continunado planificacion\n");
		}else{
			printf("No esta pausado\n");
		}
		/*Fin de magia*/
	}else{
		printf("Muchos argumentos\n");
	}
	free_comando(comando);
}

void listar(t_comando* comando){
	if(comando->cantidad_argumentos == 2){
		//arg[1] = clave
		/*Magia*/
		if( strlen(comando->argumentos[1]) <= LARGO_MAXIMO_CLAVE ){
			listar_esis_bloqueados_por_clave(comando->argumentos[1]);
		}else{
			printf("Clave muy larga\n");
		}
		/*Fin de magia*/
	}else{
		printf("Muchos argumentos\n");
	}
	free_comando(comando);
}

void listar_esis_bloqueados_por_clave(char* clave){
	printf("\nESIs bloqueados por la clave <%s>:\n", clave);
	int total_bloqueados = 0;
	void printear_si_esta_bloqueado_por_clave(void* esi_bloqueado){
		if( strcmp( ((t_esi*)esi_bloqueado)->clave_bloqueante , clave ) == 0 ){
			printf("\tESI <%d>\n", ((t_esi*)esi_bloqueado)->ID);
			total_bloqueados++;
		}
	}

	if(total_bloqueados == 0){
		printf("No hay esis bloqueados por la clave <%s>\n", clave);
	}
	printf("\n");
	list_iterate(bloqueados, &printear_si_esta_bloqueado_por_clave);
}

void kill_consola(t_comando* comando){
	if(comando->cantidad_argumentos == 2){
		//arg[1] = id
		/*Magia*/
		int id = atoi(comando->argumentos[1]);
		if( id != 0 ){

			bool es_esi_buscado(void* esi_en_lista){
				return ((t_esi*)esi_en_lista)->ID == id;
			}

			pthread_mutex_lock(&mutex_listas);

			t_esi* esi_a_matar = NULL;
			esi_a_matar = list_find(listos, &es_esi_buscado);
			if(esi_a_matar != NULL){
				terminar_esi(esi_a_matar, listos);
				sem_wait(&contador_esis);
			}else{

				esi_a_matar = list_find(ejecutando, &es_esi_buscado);
				if(esi_a_matar != NULL){
					terminar_esi(esi_a_matar, ejecutando);
				}else{

					esi_a_matar = list_find(bloqueados, &es_esi_buscado);
					if(esi_a_matar != NULL){
						terminar_esi(esi_a_matar, bloqueados);

					}else{
						printf("No existe ningun ESI con ese ID\n");
					}
				}
			}

			pthread_mutex_unlock(&mutex_listas);
		}else{
			printf("ID <%s> invalida\n", comando->argumentos[1]);
		}
		/*Fin de magia*/
	}else{
		printf("Muchos argumentos\n");
	}
	free_comando(comando);
}

void status(t_comando* comando){
	if(comando->cantidad_argumentos == 2){
		/*Magia*/
		char* clave = comando->argumentos[1];
		if( strlen(clave) <= LARGO_MAXIMO_CLAVE ){
			pthread_mutex_lock(&mutex_listas);
			printf("\n[Status]: Solicitando status de clave <%s>\n", clave);
			prot_enviar_mensaje(socket_coordinador, PIDE_STATUS, strlen(clave), clave);
			sem_wait(&producto_consumidor_status);
			listar_esis_bloqueados_por_clave(clave);
			pthread_mutex_unlock(&mutex_listas);
		}else{
			printf("[Status]: clave muy larga\n");
		}
		/*Fin de magia*/
	}else{
		printf("Muchos argumentos\n");
	}
	free_comando(comando);
}

void deadlock(t_comando* comando){
	if(comando->cantidad_argumentos == 1){
		//Sin bloqueados no hay deadlock
		if(!list_is_empty(bloqueados)){
			/*Magia*/
			pthread_mutex_lock(&mutex_listas);
			printf("\n[Deadlock]: Buscando deadlocks...\n");
//			esis_en_deadlock = list_create();
			list_iterate( bloqueados, &buscar_deadlock_para_un_esi );

			pthread_mutex_unlock(&mutex_listas);
			/*Fin de magia*/
		}else{
			printf("No hay bloqueados, por lo que no hay deadlock\n");
		}
	}else{
		printf("Muchos argumentos\n");
	}
	free_comando(comando);
}

void buscar_deadlock_para_un_esi(void* esi_bloqueado){
	t_esi* esi = (t_esi*)esi_bloqueado;
	t_list* camino_de_esis_bloqueados = list_create();
	list_add(camino_de_esis_bloqueados, esi);
	t_esi* esi_bloqueante = NULL;
	bool sigue_buscando = true;
	t_esi* ultimo_esi_del_camino;

//	esi_bloqueante = buscar_esi_bloqueante(esi);

	while(sigue_buscando){

		ultimo_esi_del_camino = list_get( camino_de_esis_bloqueados, camino_de_esis_bloqueados->elements_count - 1 );

		bool es_clave_bloqueada(void* clave){
			return strcmp((char*)clave , ultimo_esi_del_camino->clave_bloqueante) == 0;
		}

		bool tiene_clave_bloqueante(void* esi_con_clave){
			return list_any_satisfy( ((t_esi*)esi_con_clave)->claves_bloqueadas, &es_clave_bloqueada);
		}

		esi_bloqueante = list_find(bloqueados, &tiene_clave_bloqueante);

		if(esi_bloqueante != NULL){

			bool tiene_mismo_id(void* esi_en_lista){
				return ((t_esi*)esi_en_lista)->ID == esi_bloqueante->ID;
			}

			//Busco si el bloqueante ya forma parte del camino
			if( list_any_satisfy(camino_de_esis_bloqueados, &tiene_mismo_id) ){
				sigue_buscando = false;
				printf("\tESI <%d> , <%s> esta en deadlock\n", esi->ID, esi->nombre);
				break;
			}else{
				list_add(camino_de_esis_bloqueados, esi_bloqueante);
			}

		}else{
			printf("\tESI <%d> , <%s> NO esta en deadlock\n", esi->ID, esi->nombre);
			sigue_buscando = false;
			break;
		}

	}

	list_destroy(camino_de_esis_bloqueados);

}

void salir(t_comando* comando){
	if(comando->cantidad_argumentos == 1){
		/*Magia*/
		free_comando(comando);
		finalizar("Salida por consola");
		/*Fin de magia*/
	}else{
		printf("Muchos argumentos\n");
	}
}

void mostrar_menu(t_comando* comando){
	if(comando->cantidad_argumentos == 1){
		/*Magia*/
		printf("\tMenu del planificador:\n"
				"\tmenu\t:Muestra este menu\n"
				"\tpausar\t:Pausa la planifiacion de ESIs\n"
				"\tcontinuar\t:Continua la planificacion de ESIs\n"
				"\tbloquear <ID> <clave>\t:Bloquea al ESI que tenga la <ID> ingresada con la <clave> como recurso faltante\n"
				"\tdesbloquear <clave>\t:Desbloquea la <clave>\n"
				"\tlistar <clave>\t:Lista todos los ESIs bloqueados esperando por esa clave\n"
				"\tkill <ID>\t:Termina el ESI indicado\n"
				"\tstatus <ID>\t:Muestra el estado del ESI indicador\n"
				"\tmostrar-estado\t:Muestra el estado de todos los esis\n"
				"\tsalir\t:Cierra el Planificador\n");
		/*Fin de magia*/
	}else{
		printf("Muchos argumentos\n");
	}
	free_comando(comando);
}

void free_comando(t_comando* comando){
	if(comando != NULL){
		for(int i = 0; i < comando->cantidad_argumentos; i++){
			free(comando->argumentos[i]);
		}
		free(comando->argumentos);
		free(comando);
	}
}

void printear_todo(t_comando* comando){
	printf("\n===PRINTEANDO TODO===\n");

	if(modo_debug){
		if(config_info.algoritmo == SJF_CD){
			if(llego_esi_nuevo){
				printf("\tBool nuevo esi: True\n");
			}else{
				printf("\tBool nuevo esi: False\n");
			}
		}

		int a;
		sem_getvalue(&contador_esis, &a);
		printf("\tSemaforo contador de ESIs: <%d>\n", a );
	}


	printf("\nPrinteando listos\n");
	if(!list_is_empty(listos)){
		list_iterate(listos, &printear_esi);
	}else{
		printf("\tLista vacia\n");
	}

	printf("\nPrinteando ejecutando\n");
	if(!list_is_empty(ejecutando)){
		list_iterate(ejecutando, &printear_esi);
	}else{
		printf("\tLista vacia\n");
	}

	printf("\nPrinteando bloqueados\n");
	if(!list_is_empty(bloqueados)){
		list_iterate(bloqueados, &printear_esi);
	}else{
		printf("\tLista vacia\n");
	}

	void printear_terminado(void* esi){
		printf("\tESI: <%d>\tNombre: <%s>\n", ((t_esi_terminado*)esi)->ID, ((t_esi_terminado*)esi)->nombre );
	}

	printf("\nPrinteando termiandos:\n");
	if(!list_is_empty(terminados)){
		list_iterate(terminados, &printear_terminado);
	}else{
		printf("\tLista vacia\n");
	}

	printf("\nClaves bloqueadas:\n");
	void printear(void* clave){
		printf("Clave:\t<%s>\n", (char*) clave);
	}

	if(!list_is_empty(claves_bloqueadas)){
		list_iterate(claves_bloqueadas, &printear);
	}else{
		printf("\tLista vacia\n");
	}

	if(comando != NULL){
		free_comando(comando);
	}
}

void printear_esi(void* esi_void){
	t_esi* esi = (t_esi*)esi_void;
	printf("\n\tESI: <%d>\tNombre:<%s>\n", esi->ID, esi->nombre);

	if( (config_info.algoritmo == SJF_CD) || (config_info.algoritmo == SJF_SD) ){
		printf("\tEstimacion SJF: <%f>\n", esi->estimacion.estimacion_actual);
	}

	if(config_info.algoritmo == HRRN){
		double estimacion = esi->estimacion.estimacion_actual;
		double espera = tiempo - esi->estimacion.tiempo_ultima_llegada_a_listos;
		printf("\tEstimacion HRRN: <%f>\n", estimacion);
		printf("\tEspera HRRN: <%f>\n", espera);
		printf("\tResponsive Ratio: <%f>\n", (estimacion + espera)/estimacion );
	}

	if(esi->bloqueado){
		printf("\tBloqueado: SI\n");
	}else{
		printf("\tBloqueado: NO\n");
	}

	if(esi->terminado){
		printf("\tTerminado: SI\n");
	}else{
		printf("\tTerminado: NO\n");
	}

	printear_claves_bloqueadas_esi(esi);
	if(esi->clave_bloqueante != NULL){
		printf("\tClave bloqueante:\n\t\tClave:<%s>\n", esi->clave_bloqueante);
	}
}

void printear_claves_bloqueadas_esi(void* esi_void){
	t_esi* esi = (t_esi*)esi_void;
	printf("\tClaves bloqueadas:\n");
	void print(void* clave){
		printf("\t\tClave:<%s>\n", (char*)clave );
	}
	list_iterate(esi->claves_bloqueadas, &print);
}

void debug(t_comando* comando){
	if(comando->cantidad_argumentos == 2){

		if(strcmp(comando->argumentos[1], "on") == 0){

			if(modo_debug){
				printf("Ya esta en modo debug\n");
			}else{
				printf("Modo debug ON\n");
				modo_debug = true;
			}
		}else{
			if(strcmp(comando->argumentos[1], "off") == 0){
				if(modo_debug){
					printf("Modo debug OFF\n");
					modo_debug = false;
				}else{
					printf("No esta modo debug\n");
				}
			}
		}

	}else{
		printf("Muchos argumentos\n");
	}
	free_comando(comando);
}
