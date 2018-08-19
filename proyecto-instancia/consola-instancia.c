#include "consola-instancia.h"

void consola(){
	char* entrada;
	while(1){
		entrada = readline("Instancia> ");
		if(entrada != NULL){
			entrada[strlen(entrada)] = '\0';
			if(strlen(entrada) > 4){
				if( strcmp( entrada , "debug-on" ) == 0 ){
					add_history(entrada);
					debug_on();
				}
				else if( strcmp( entrada , "debug-off" ) == 0 ){
					add_history(entrada);
					debug_off();
				}
				else if( strcmp( entrada , "estado" ) == 0 ){
					add_history(entrada);
					estado();
				}
				else if( strcmp( entrada , "pausar" ) == 0 ){
					add_history(entrada);
					pausar();
				}
				else if( strcmp( entrada , "continuar" ) == 0 ){
					add_history(entrada);
					continuar();
				}else{
					printf("Comando no reconocido\n");
				}
			}
			free(entrada);
		}
	}//Fin del while
	pthread_exit(0);
}

void pausar(){
	if(!pausado){
		pthread_mutex_lock(&mutex_pausar_continuar);
		pausado = true;
		printf("Instancia pausada\n");
	}else{
		printf("Ya esta pausado\n");
	}
}

void continuar(){
	if(pausado){
		pthread_mutex_unlock(&mutex_pausar_continuar);
		pausado = false;
		printf("Continunado instancia\n");
	}else{
		printf("No esta pausado\n");
	}
}

void estado(){
	printf("[Estado]: Mostrando bitmap:\n");
	for(int i = 0; i < datos_tabla.cant_entrada ;i++){
		if(bitmap[i].ocupado){
			printf("\t[bitmap]: Entrada <%d> esta ocupada\n", i);

		}
		else{
			printf("\t[bitmap]: Entrada <%d> esta libre\n", i);
		}
	}

	if(list_is_empty(tabla_entradas)){
		printf("[Estado]: Tabla de entradas vacia\n");
	}
	else{
		printf("[Estado]: Mostrando tabla de entradas vacia\n");
		list_iterate(tabla_entradas, &imprimir_entrada);

		printf("[Estado]: Mostrando storage");
		list_iterate(tabla_entradas, &imprimir_storage);
	}
}

void imprimir_entrada(void* entrada_en_tabla){
	t_tabla_entradas* entrada = (t_tabla_entradas*)entrada_en_tabla;
	printf("\t[entrada]: clave <%s>, entrada inicial <%d>, largo <%d>\n", entrada->clave, entrada->entrada_inicial, entrada->tamanio_valor);
}

void imprimir_storage(void* entrada_en_tabla){
	t_tabla_entradas* entrada = (t_tabla_entradas*)entrada_en_tabla;
	if(entrada->entrada_inicial != -1){
		int entradas_usadas = (int)ceil((double)entrada->tamanio_valor / (double)datos_tabla.tam_entrada);
		printf("\t[storage]: Clave <%s>", entrada->clave);
		printf("\t\t[storage]: Entrada inicial: <%d>\n", entrada->entrada_inicial);
		printf("\t\t[storage]: Entradas utilizadas: <%d>\n", entradas_usadas);
		printf("\t\t[storage]: Largo del valor: <%d>\n", entrada->tamanio_valor);
		printf("\t\t[storage]: Entradas usadas: <%d>\n", entradas_usadas);
		printf("\t\t[storage]: Entradas ocupadas: de la <%d> a la <%d>\n", entrada->entrada_inicial, entrada->entrada_inicial + entradas_usadas - 1);

		int largo_restante = entrada->tamanio_valor;

		for(int i = 0; i < entradas_usadas; i++){
			size_t largo_en_esta_entrada = 0;
			if(i == entradas_usadas-1){
				largo_en_esta_entrada = largo_restante;
			}else{
				largo_en_esta_entrada = datos_tabla.tam_entrada;
			}
			largo_restante -= largo_en_esta_entrada;
			char* valor_en_entrada = (char*) malloc (largo_en_esta_entrada + 1);
			memset(valor_en_entrada, 0, largo_en_esta_entrada);
			memcpy(valor_en_entrada, bitmap[entrada->entrada_inicial+i].inicio_bloque, largo_en_esta_entrada);
			valor_en_entrada[largo_en_esta_entrada] = '\0';
			printf("\t\tEntrada %d, contenido: %s\n", entrada->entrada_inicial+i, valor_en_entrada);
			free(valor_en_entrada);
		}
	}
}

void debug_on(){
	if(modo_debug){
		printf("[Consola]: Ya esta en modo debug\n");
	}
	else{
		printf("[Consola]: Modo debug activado\n");
	}
	modo_debug = true;
}

void debug_off(){
	if(!modo_debug){
		printf("[Consola]: No esta en modo debug\n");
	}
	else{
		printf("[Consola]: Modo debug desactivado\n");
	}
	modo_debug = false;
}
