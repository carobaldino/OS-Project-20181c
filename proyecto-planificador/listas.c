#include "listas.h"

void inicializar_listas(){
	//creacion de listas;
	listos = list_create();
	ejecutando = list_create();
	bloqueados = list_create();
	terminados = list_create();
	claves_bloqueadas = list_create();
	//Inicializando claves inicialmente bloquedas
	for(int indice = 0; indice < config_info.cantidad_claves_inicialmente_bloqueadas; indice++){
		list_add(claves_bloqueadas, strdup(config_info.claves_inicialmente_bloqueadas[indice]));
		free(config_info.claves_inicialmente_bloqueadas[indice]);
	}
	free(config_info.claves_inicialmente_bloqueadas);
	//Inicializacion de algunas cosas necesarias para listas
	ultimo_id = 1;
	pausado = true;
	sem_init(&contador_esis, 0, 0);
	sem_init(&producto_consumidor_status, 0, 0);
	llego_esi_nuevo = false;
	pthread_mutex_init(&mutex_listas, NULL);
	pthread_mutex_init(&mutex_pausar_continuar, NULL);
	if(pausado){
		pthread_mutex_lock(&mutex_pausar_continuar);
	}
	esi_a_ejecutar = NULL;
}

void agregar_nuevo_esi(int socket, char* nombre){
	t_esi* nuevo_esi = (t_esi*) malloc (sizeof(t_esi));
	nuevo_esi->socket = socket;
	nuevo_esi->ID = ultimo_id;
	ultimo_id++;
	nuevo_esi->terminado = false;
	nuevo_esi->claves_bloqueadas = list_create();
	nuevo_esi->estimacion.estimacion_actual = config_info.estimacion_inicial;
	nuevo_esi->estimacion.ya_estimado = true;
	nuevo_esi->estimacion.rafaga_actaual = 0;
	nuevo_esi->estimacion.responsive_ratio = 0;
	nuevo_esi->nombre = nombre;
	nuevo_esi->clave_bloqueante = NULL;
	pthread_mutex_lock(&mutex_listas);
	nuevo_esi->estimacion.tiempo_ultima_llegada_a_listos = tiempo;
	list_add(listos, nuevo_esi);
	llego_esi_nuevo = true;
	sem_post(&contador_esis);
	pthread_mutex_unlock(&mutex_listas);
	/*Creo que habria un problema si hay muchos nuevos llegando. Podrian
	 * "atorarse" en la entrada del socket de escucha*/
}

void mover_esi(t_esi* esi, t_list* origen, t_list* destino){
	list_add(destino, esi);
	bool buscar_id(void* esi_lista){
		return ((t_esi*)esi_lista)->ID == esi->ID;
	}
	list_remove_by_condition(origen, &buscar_id);
}

void listo_a_ejecutando(t_esi* esi){
	esi->estimacion.tiempo_ultima_llegada_a_listos = tiempo;

	if(config_info.algoritmo == SJF_CD && esi->estimacion.desalojado){
		;
	}else{
		esi->estimacion.rafaga_actaual = 0;
	}
	esi->estimacion.desalojado = false;
	mover_esi(esi, listos, ejecutando);
}

void ejecutando_a_bloqueado(t_esi* esi, char* clave_bloqueante){
	esi->clave_bloqueante = strdup(clave_bloqueante);
	esi->bloqueado = true;
	esi->estimacion.estimacion_anterior = esi->estimacion.estimacion_actual;
	esi->estimacion.rafaga_actaual++;//Por si se bloquea antes de enviar mensaje de ejecucion correcta
	esi->estimacion.real_anterior = esi->estimacion.rafaga_actaual;
	esi->estimacion.ya_estimado = false;
	estimar(esi);
	mover_esi(esi, ejecutando, bloqueados);
}

void ejecutando_a_listo(t_esi* esi){
	esi->estimacion.desalojado = true;
	mover_esi(esi, ejecutando, listos);
	sem_post(&contador_esis);
}

void bloqueado_a_listo(t_esi* esi){
	free(esi->clave_bloqueante);
	esi->clave_bloqueante = NULL;
	esi->bloqueado = false;
	esi->estimacion.tiempo_ultima_llegada_a_listos = tiempo;
	mover_esi(esi, bloqueados, listos);
	llego_esi_nuevo = true;
	sem_post(&contador_esis);
}

void listo_a_bloqueado(t_esi* esi, char* clave_bloqueante){
	esi->clave_bloqueante = strdup(clave_bloqueante);
	esi->bloqueado = true;
	mover_esi(esi, listos, bloqueados);
	sem_wait(&contador_esis);
}

void bloquear_clave_para_esi_ejecutando(char* clave){
	list_add(esi_a_ejecutar->claves_bloqueadas, strdup(clave));
	list_add(claves_bloqueadas, strdup(clave));
}

void desbloquear_clave_de_esi(char* clave, t_esi* esi){
	bool es_clave_a_desbloquear(void* clave_en_lista){
		return strcmp( clave , (char*)clave_en_lista ) == 0;
	}

	char* clave_a_desbloquear = list_find(esi->claves_bloqueadas, &es_clave_a_desbloquear);

	if(clave_a_desbloquear != NULL){
		list_remove_and_destroy_by_condition(esi->claves_bloqueadas, &es_clave_a_desbloquear, &free);
		eliminar_clave_de_lista_de_claves_bloqueadas(clave);
	}else{
		printf("[Error borrando clave]:\tEl ESI <%d> no posee la clave <%s>\n", esi->ID, clave);
	}
}

void eliminar_clave_de_lista_de_claves_bloqueadas(char* clave){
	bool es_clave_a_desbloquear(void* clave_en_lista){
		return strcmp( clave , (char*)clave_en_lista ) == 0;
	}
	if(list_any_satisfy(claves_bloqueadas, &es_clave_a_desbloquear) ){
		list_remove_and_destroy_by_condition(claves_bloqueadas, &es_clave_a_desbloquear, &free);
	}
}

void desbloquear_esi_bloqueados_por_clave(char* clave_a_desbloquear){

	bool bloqueado_por_clave_liberada(void* esi){
		return strcmp( ((t_esi*)esi)->clave_bloqueante , clave_a_desbloquear) == 0;
	}
	if(modo_debug){
	printf("[Listas]: Desbloqueando ESIs bloqueados por clave <%s>\n", clave_a_desbloquear);
	}
	//Desbloquear una clave implica agarrar al primer ESI que este bloqueado por el mismo y mandarlo a listos
	t_esi* esi_a_desbloquear = NULL;
	esi_a_desbloquear = list_find(bloqueados, &bloqueado_por_clave_liberada);
	if(esi_a_desbloquear != NULL){
		if(modo_debug){
		printf("[Listas]: Desbloqueando ESI <%d>\n", esi_a_desbloquear->ID);
		}
		bloqueado_a_listo(esi_a_desbloquear);
	}else{
		if(modo_debug){
		printf("[Listas]: Ningun ESI bloqueado por clave <%s>\n", clave_a_desbloquear);
		}
	}
}

void terminar_esi(t_esi* victima, t_list* lista_origen){
	victima->terminado = true;

	/*Adaptacion de tipos:
	 * Las funciones estan definidas para t_clave per les paso void*
	 * No las voy a redefinir como void* por simplicidad en otros lados*/;
	void adaptacion_desbloquear_esi_bloqueados_por_clave(void* clave){
		desbloquear_esi_bloqueados_por_clave( (char*)clave );
	}
	void adaptacion_eliminar_clave_de_lista_de_claves_bloqueadas(void* clave){
		eliminar_clave_de_lista_de_claves_bloqueadas( (char*)clave );
	}

	if( !list_is_empty( victima->claves_bloqueadas)){
		list_iterate(victima->claves_bloqueadas, &adaptacion_desbloquear_esi_bloqueados_por_clave);
		list_iterate(victima->claves_bloqueadas, &adaptacion_eliminar_clave_de_lista_de_claves_bloqueadas);
		list_iterate(victima->claves_bloqueadas, &free);
	}

	list_destroy(victima->claves_bloqueadas);

	/*Remueve de la lista de origen*/
	bool buscar_id(void* esi_lista){
		return ((t_esi*)esi_lista)->ID == victima->ID;
	}
	list_remove_by_condition(lista_origen, &buscar_id);

	/*Agrega a la lista de terminados*/
	t_esi_terminado* esi_terminado = (t_esi_terminado*) malloc(sizeof(t_esi_terminado));
	esi_terminado->ID = victima->ID;//id_terminado;
	esi_terminado->nombre = strdup(victima->nombre);
	list_add(terminados, esi_terminado);

	/*Libera memoria*/
	close(victima->socket);
	free(victima->nombre);
	if(victima->clave_bloqueante != NULL){
		free(victima->clave_bloqueante);
	}
	free(victima);
}
