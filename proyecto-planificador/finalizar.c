#include "finalizar.h"

void finalizar(char* mensaje_de_cierre){
	/*Falta desarrollar...*/
	if(!cerrando){
		printf("%s\nCerrando Planificador...\n", mensaje_de_cierre);
		printf("\n\n");

		cerrando = true;

		void adaptacion_terminar_esi_listos(void* esi){
			terminar_esi((t_esi*)esi, listos);
		}

		void adaptacion_terminar_esi_ejecutando(void* esi){
			terminar_esi((t_esi*)esi, ejecutando);
		}

		void adaptacion_terminar_esi_bloqueados(void* esi){
			terminar_esi((t_esi*)esi, bloqueados);
		}

		if(!list_is_empty(bloqueados)){
			list_iterate(bloqueados, &adaptacion_terminar_esi_bloqueados);
		}

		if(!list_is_empty(listos)){
			list_iterate(listos, &adaptacion_terminar_esi_listos);
		}

		if(!list_is_empty(ejecutando)){
			list_iterate(ejecutando, &adaptacion_terminar_esi_ejecutando);
		}

		if(!list_is_empty(claves_bloqueadas)){
			list_iterate(claves_bloqueadas, &free);
		}

		void free_terminado(void* terminado){
			t_esi_terminado* esi = (t_esi_terminado*)terminado;
			free(esi->nombre);
			free(esi);
		}

		if(!list_is_empty(terminados)){
			list_iterate(terminados, &free_terminado);
		}

		list_destroy(listos);
		list_destroy(bloqueados);
		list_destroy(ejecutando);
		list_destroy(terminados);
		list_destroy(claves_bloqueadas);

		exit(1);
	}
	getchar();
}

void handler_ctrl_c(int a){
	printf("Para cerrar el Planificador ingersar 'salir'\n");
}
