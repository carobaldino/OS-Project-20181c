#include "dump.h"

void dump(){
	while(1){
		sleep(config_info.intervalo_dump_seg);
		pthread_mutex_lock(&mutex_dump_compactar);
		if(!list_is_empty(tabla_entradas)){
			list_iterate(tabla_entradas, &persistir_entrada);
		}
		pthread_mutex_unlock(&mutex_dump_compactar);
	}
}

void persistir_entrada(void* entrada_en_tabla){
	t_tabla_entradas* entrada = (t_tabla_entradas*)entrada_en_tabla;
	if(entrada->entrada_inicial != -1){
		char* path_con_archivo = string_from_format("%s/%s",config_info.punto_montaje, entrada->clave);
		char* valor = (char*)malloc(entrada->tamanio_valor);
		memset(valor, 0, entrada->tamanio_valor);
		memcpy(valor, bitmap[entrada->entrada_inicial].inicio_bloque, entrada->tamanio_valor);
		valor[entrada->tamanio_valor] = '\0';
		FILE *archivo_pers_clave = txt_open_for_append(path_con_archivo);
		txt_write_in_file(archivo_pers_clave, valor);
		free(path_con_archivo);
		free(valor);
	}
}
