#include "get_set_store.h"

void escuchar_coordinador(){
	bool coordinador_conectado = true;
	t_prot_mensaje* mensaje_coordinador;

	while(coordinador_conectado){
		if(pausado){
			pthread_mutex_lock(&mutex_pausar_continuar);
			pthread_mutex_unlock(&mutex_pausar_continuar);
		}

		mensaje_coordinador = prot_recibir_mensaje(socket_coordinador);
		pthread_mutex_lock(&mutex_dump_compactar);
		switch(mensaje_coordinador->head){

			case GUARDAR_CLAVE:{
				guardar_clave(mensaje_coordinador);
				break;
			}

			case SETEAR_VALOR:{
				setear_valor(mensaje_coordinador);
				break;
			}

			case STOREAR_CLAVE:
				storear_valor(mensaje_coordinador);
				break;

			case DESCONEXION: case FALLO_AL_RECIBIR:
				coordinador_conectado = false;
				break;

			default:
				break;
		}
		prot_destruir_mensaje(mensaje_coordinador);
	}
	printf("Coordinador desconectado\n");
}

void guardar_clave(t_prot_mensaje* mensaje_coordinador){
	//1. Buscar la tabla
	//2. Crear registro en la tabla
	//3. Guardar CLAVE - NULL - 0
	//4. Enviar mensajepthread_mutex_lock(&mutex_tabla_entradas); Coordinador prot_enviar(,CLAVE_GUARDADA || CLAVE_NO_GUARDADA,)
	char* clave = leer_string_de_mensaje(mensaje_coordinador);
	printf("[Instancia-GET]: clave %s\n", clave);

	bool es_clave(void* entrada){
		return strcmp( ((t_tabla_entradas*)entrada)->clave , clave ) == 0;
	}
	t_tabla_entradas* entrada_con_clave = (t_tabla_entradas*) list_find(tabla_entradas , &es_clave);

	//Revisar si la clave ya esta en la tabla de entradas
	if(entrada_con_clave != NULL){
		//La clave existe en la tabla
		printf("[Instancia-GET]: La clave <%s> ya se encuentra en la tabla de entradas\n", clave);
		printf("[Instancia-GET]: Actualizando clave <%s>\n", clave);
		entrada_con_clave->entrada_inicial = -1;
		entrada_con_clave->tamanio_valor = -1;
		prot_enviar_mensaje(socket_coordinador,CLAVE_GUARDADA,0,NULL);
		printf("[Instancia-GET]: Fin\n");

	}
	else{
		//La clave no existe en la tabla
		t_tabla_entradas* nueva_entrada = (t_tabla_entradas*) malloc(sizeof(t_tabla_entradas));
		printf("[Instancia-GET]: La clave <%s> no se encuentra en la tabla de entradas\n", clave);
		nueva_entrada->clave = strdup(clave);
		nueva_entrada->entrada_inicial = -1;
		nueva_entrada->tamanio_valor = -1;
		list_add(tabla_entradas, nueva_entrada);
		printf("[Instancia-GET]: Clave <%s> agVALOR_NO_SETEADOregada\n", clave);
		prot_enviar_mensaje(socket_coordinador,CLAVE_GUARDADA,0,NULL);
		printf("[Instancia-GET]: Fin\n");
	}
	free(clave);
}

void setear_valor(t_prot_mensaje* mensaje_coordinador){
	size_t largo_clave;
	size_t largo_valor;

	memset(&largo_clave, 0, sizeof(size_t));
	memset(&largo_valor, 0, sizeof(size_t));

	memcpy(&largo_clave, mensaje_coordinador->payload, sizeof(size_t));
	memcpy(&largo_valor, mensaje_coordinador->payload + sizeof(size_t), sizeof(size_t));

	char* clave = (char*) malloc (largo_clave + 1);
	char* valor = (char*) malloc (largo_valor + 1);

	memcpy(clave, mensaje_coordinador->payload + sizeof(size_t)*2, largo_clave);
	memcpy(valor, mensaje_coordinador->payload + sizeof(size_t)*2 + largo_clave, largo_valor);

	clave[largo_clave] = '\0';
	valor[largo_valor] = '\0';

	printf("[Intancia-SET]: clave %s\n", clave);
	printf("[Intancia-SET]: valor %s\n", valor);

	bool es_clave(void* entrada){
		return strcmp( ((t_tabla_entradas*)entrada)->clave , clave ) == 0;
	}
	t_tabla_entradas* entrada_con_clave = (t_tabla_entradas*) list_find(tabla_entradas , &es_clave);
		//0. Ver si la clave ya tiene una valor asignado.
			//Si tiene un valor y es mas corto o igual que el nuevo se pisa
			//Si tiene un valor y es mas largo
				//Si entra en el espacio que tiene se graba
				//Si no entra se busca espacio
		//1. Ver si hay lugar para poner el valor
		//2. Ver si se puede asignar el valor
		//3. Si es necesario, compactar
		//4. Asignar valor

	if(entrada_con_clave != NULL){
		int cantidad_entradas_necesarias = (int)ceil( (double)strlen(valor) / (double)datos_tabla.tam_entrada);
		printf("[Intancia-SET]: Se necesitan <%d> entradas para escribir el valor <%s>\n", cantidad_entradas_necesarias,valor);
		int cantidad_entradas_disponibles = 0;

		if(entrada_con_clave->entrada_inicial == -1){
			/*Entra aca cuando la clave no tiene ningun valor*/
			printf("[Intancia-SET]: La clave <%s> no tiene valor asignado\n", clave);
			insertar_nuevo_valor_en_storage(valor, entrada_con_clave, cantidad_entradas_necesarias);
		}
		else{
			/*Si entra por aca es porque ya tiene un valor seteado*/
			int entradas_actuales = (int)ceil( (double)entrada_con_clave->tamanio_valor / (double)datos_tabla.tam_entrada );
			int inicio = entrada_con_clave->entrada_inicial;
			printf("[Intancia-SET]: La clave <%s> ya tenia un valor en storage de la entrada %d a la %d\n", valor, inicio, inicio+entradas_actuales-1);

			if(cantidad_entradas_necesarias == entradas_actuales){
				/*Se copia el nuevo valor sobre el viejo*/
				printf("[Intancia-SET]: Se inserta el valor %s\n", valor);
				escribir_valor(valor, inicio, entrada_con_clave, cantidad_entradas_necesarias);
			}

			if(cantidad_entradas_necesarias < entradas_actuales){
				/*Se copia el nuevo valor sobre el viejo y se liberan las entradas no usadas*/
				asignar_bit_ocupado(inicio, entradas_actuales, false);
				escribir_valor(valor, inicio, entrada_con_clave, cantidad_entradas_necesarias);
			}

			if(cantidad_entradas_necesarias > entradas_actuales){
				/*Se revisa si grabando desde aqui no pisa ningun otro valor*/
				bool necesita_reubicar = false;
				int ultima_entrada_ocupada = entrada_con_clave->entrada_inicial + entradas_actuales;
				for(int i = 0; i < cantidad_entradas_necesarias - entradas_actuales; i++){
					if(bitmap[ultima_entrada_ocupada + i].ocupado){
						necesita_reubicar = true;
						break;
					}
				}

				if(necesita_reubicar){
					asignar_bit_ocupado(inicio, entradas_actuales, false);
					buscar_entradas_contiguas_y_asignar(valor, entrada_con_clave, cantidad_entradas_necesarias);

				}
				else{
					printf("[Instancia-SET]: Se graba el valor <%s> de la entrada %d a %d\n", valor, entrada_con_clave->entrada_inicial, entrada_con_clave->entrada_inicial + cantidad_entradas_necesarias);
					escribir_valor(valor, inicio, entrada_con_clave, cantidad_entradas_necesarias);
				}
			}
		}
	}
	else{
		printf("[Instancia-SET]: No se encuentra la calve <%s> en la tabla de entradas\n", clave);
		prot_enviar_mensaje(socket_coordinador, VALOR_NO_SETEADO ,0,NULL);
	}

	free(clave);
	free(valor);
}

void escribir_valor(char* valor, int entrada_inicial, t_tabla_entradas* entrada_con_clave, int entradas_utilizadas){
	/*Escribe el valor y pone las entradas ocupadas*/
	printf("[Instancia-SET]: Se escribe el valor <%s> de la entrada %d a %d\n", valor, entrada_inicial, entrada_inicial + entradas_utilizadas);
	memcpy( bitmap[entrada_inicial].inicio_bloque, valor, strlen(valor));
	asignar_bit_ocupado(entrada_inicial, entradas_utilizadas, true);
	entrada_con_clave->entrada_inicial = entrada_inicial;
	entrada_con_clave->tamanio_valor = strlen(valor);
	prot_enviar_mensaje(socket_coordinador, VALOR_SETEADO,0,NULL);
	printf("[Instancia-SET]: Fin\n");
}

void asignar_bit_ocupado(int inicio, int cantidad, bool estado){
	for(int i = 0; i < cantidad; i++){
		bitmap[inicio+i].ocupado = estado;
	}
}

void buscar_entradas_contiguas_y_asignar(char* valor, t_tabla_entradas* entrada_con_clave, int cantidad_entradas_necesarias){
	int cantidad_entradas_contiguas = 0;
	int entrada_actual = 0 ;

	printf("[Intancia-SET]: Buscando <%d> entradas libres contiguas\n", cantidad_entradas_necesarias);

	while( (cantidad_entradas_contiguas < cantidad_entradas_necesarias) && (entrada_actual < datos_tabla.cant_entrada) ) {

		if(!bitmap[entrada_actual].ocupado){
			cantidad_entradas_contiguas++;
		}
		else{
			cantidad_entradas_contiguas=0;
		}

		entrada_actual++;
	}


	if(cantidad_entradas_contiguas == cantidad_entradas_necesarias){
		/*Si las hay se graba*/
		int inicio = entrada_actual - cantidad_entradas_necesarias;
		printf("[Instancia-SET]: Se graba el valor <%s> de la entrada %d a %d\n",valor, inicio, inicio + cantidad_entradas_necesarias);
		escribir_valor(valor, inicio, entrada_con_clave, cantidad_entradas_necesarias);
	}
	else{
		printf("[Instancia-SET]: Se necesita compactar\n");
		compactar();
		prot_enviar_mensaje(socket_coordinador, VALOR_SETEADO,0,NULL);
	}
}

void insertar_nuevo_valor_en_storage(char* valor, t_tabla_entradas* entrada_con_clave, int cantidad_entradas_necesarias){
	/*Entra aca cuando la clave no tiene ningun valor*/
	printf("[Intancia-SET]: Se busca entrada para escribir el valor %s\n", valor);
	int cantidad_entradas_disponibles = 0;
	for(int i = 0; i < datos_tabla.cant_entrada; i++){
		if(!bitmap[i].ocupado){
			cantidad_entradas_disponibles++;
		}
	}

	printf("[Instancia-SET]: Entradas necesarias %d\n", cantidad_entradas_necesarias);
	printf("[Instancia-SET]: Entradas disponibles %d\n", cantidad_entradas_disponibles);

	if(cantidad_entradas_disponibles < cantidad_entradas_necesarias){
		/*Reemplazo*/
		printf("[Instancia-SET]: Se necesita hacer reemplazo\n");
		reemplazar();
	}
	else{
		buscar_entradas_contiguas_y_asignar(valor, entrada_con_clave, cantidad_entradas_necesarias);
	}
}

void compactar(){
	pthread_mutex_lock(&mutex_dump_compactar);
	t_list* claves_en_storage = list_filter(tabla_entradas, &esta_en_storage);
	backup = list_create();
	list_iterate(claves_en_storage,&backupear_valor);
	asignar_bit_ocupado(0, datos_tabla.cant_entrada,false);
	list_iterate(backup, &escribir_en_storage);
	list_iterate(backup, &borrar_backup);
	pthread_mutex_unlock(&mutex_dump_compactar);
}

void escribir_en_storage(void* back){
	t_backup*bkp = (t_backup*)back;
	int entradas_necesarias = (int)ceil ( ((double)strlen(bkp->valor))/(double)(datos_tabla.tam_entrada));
	buscar_entradas_contiguas_y_asignar(bkp->entrada->clave, bkp->entrada, entradas_necesarias);
}

bool esta_en_storage(void* entrada){
	return ((t_tabla_entradas*)entrada)->entrada_inicial != -1;
}

void backupear_valor(void* entrada_lista){
	t_tabla_entradas* entrada = (t_tabla_entradas*)entrada_lista;
	t_backup* nuevo_backup = (t_backup*) malloc(sizeof(t_backup));
	nuevo_backup->entrada = entrada;
	char* valor = (char*)malloc(entrada->tamanio_valor+1);
	memset(valor, 0, entrada->tamanio_valor);
	memcpy(valor, bitmap[entrada->entrada_inicial].inicio_bloque, entrada->tamanio_valor);
	nuevo_backup->valor = valor;
	list_add(backup, nuevo_backup);
}

void borrar_backup(void* back_en_lista){
	t_backup* back_a_borra = (t_backup*)back_en_lista;
	free(back_a_borra->valor);
	free(back_a_borra);
}

void reemplazar(){

}

void storear_valor(t_prot_mensaje* mensaje_coordinador){
	//1. Buscar la tabla
	char* clave = leer_string_de_mensaje(mensaje_coordinador);
	bool es_clave(void* entrada) {
		return strcmp( ((t_tabla_entradas*)entrada)->clave , clave ) == 0;
	}
	t_tabla_entradas* entrada_con_clave = (t_tabla_entradas*) list_find(tabla_entradas , &es_clave);
	//3. Persistir valor en archivo de

	if(entrada_con_clave != NULL){
		if(entrada_con_clave->entrada_inicial != -1){
			char* path_con_archivo = string_from_format("%s/%s",config_info.punto_montaje, clave);
			printf("[Instancia-Store]: Grabando %s en %s\n", clave, path_con_archivo);
			FILE *archivo_pers_clave = txt_open_for_append(path_con_archivo);
			char* valor = (char*) malloc( entrada_con_clave->tamanio_valor + 1);
			memset(valor, 0, entrada_con_clave->tamanio_valor);
			memcpy(valor, bitmap[entrada_con_clave->entrada_inicial].inicio_bloque, entrada_con_clave->tamanio_valor);
			valor[entrada_con_clave->tamanio_valor] = '\0';
			txt_close_file(archivo_pers_clave);
			prot_enviar_mensaje(socket_coordinador,CLAVE_STOREADA,0,NULL);
			free(path_con_archivo);
			printf("[Instancia-Store]: Fin\n");
		}
		else{
			printf("[Instancia-Store]: La clave %s no tiene ningun valor accesible\n", clave);
			prot_enviar_mensaje(socket_coordinador,CLAVE_NO_STOREADA,0,NULL);
			printf("[Instancia-Store]: Fin\n");
		}
	}
	else{
		printf("[Instancia-Store]: No se encuentra la clave %s en la tabla de entradas\n", clave);
		prot_enviar_mensaje(socket_coordinador,CLAVE_NO_STOREADA,0,NULL);
		printf("[Instancia-Store]: Fin\n");
	}

	free(clave);
}
