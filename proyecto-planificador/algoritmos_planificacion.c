#include "algoritmos_planificacion.h"

void planificar(){
	tiempo = 0;
	if(pausado){
		printf("Ingrese 'continuar' en la consola para planificar\n");
	}
	while(!cerrando){ /*Se hace por siempre*/
		//1- Seleccionar proximo a ejecutar si corresponde
		//Mucho control si esta pausado porque puede pausarse en muchos lados
		if(pausado){
			pthread_mutex_lock(&mutex_pausar_continuar);
			pthread_mutex_unlock(&mutex_pausar_continuar);
		}

		if(config_info.algoritmo == SJF_CD && llego_esi_nuevo ){
			desalojar_si_corresponde();
			llego_esi_nuevo = false;
		}

		if(!sigue_ejecutando(esi_a_ejecutar)){
			sem_wait(&contador_esis);

			if(pausado){
				pthread_mutex_lock(&mutex_pausar_continuar);
				pthread_mutex_unlock(&mutex_pausar_continuar);
			}

			seleccionar_siguiente_esi_a_ejecutar();
		}

		if(pausado){
			pthread_mutex_lock(&mutex_pausar_continuar);
			pthread_mutex_unlock(&mutex_pausar_continuar);
		}
		if(modo_debug){
			printf("\n\n[Planificacion]: Iniciando ciclo\n");
		}

		pthread_mutex_lock(&mutex_listas);
		//2- Enviar orden de ejecutar
		if(modo_debug){
			printf("[Planificacion]: Enviando orden de ejecucion a ESI <%d>, <%s>\n", esi_a_ejecutar->ID, esi_a_ejecutar->nombre);
		}

		/*Meto mutex de las listas aca?*/

		prot_enviar_mensaje(esi_a_ejecutar->socket, EJECUTAR_SCRIPTS, 0, NULL);
		tiempo++;
		//3- Esperar respuesta del planificador si
		mensaje_esi = prot_recibir_mensaje(esi_a_ejecutar->socket);

		switch(mensaje_esi->head){
			case EJECUCION_FALLO:
				if(modo_debug){
					printf("[Planificacion]: Ejecucion - fallo\n");
				}
				terminar_esi(esi_a_ejecutar, ejecutando);
				break;

			case EJECUCION_CORRECTA:
				if(modo_debug){
					printf("[Planificacion]: Ejecucion - correcta\n");
				}
				(esi_a_ejecutar->estimacion.rafaga_actaual)++;
				break;

			case FALLO_AL_RECIBIR:
				if(modo_debug){
					printf("[Planificacion]: Ejecucion - fallo al recibir respuesta\n");
				}
				terminar_esi(esi_a_ejecutar, ejecutando);
				break;

			case DESCONEXION:
				if(modo_debug){
					printf("[Planificacion]: Ejecucion - desconexion del ESI\n");
				}
				terminar_esi(esi_a_ejecutar, ejecutando);
				break;

			case SCRITP_FINALIZADO:
				if(modo_debug){
					printf("[Planificacion]: Ejecucion - ESI finalizado correctamente\n");
				}
				terminar_esi(esi_a_ejecutar, ejecutando);
				break;

			default:
				if(modo_debug){
					printf("[Planificacion]: Ejecucion - no se reconoce la respuesta\n");
				}
				terminar_esi(esi_a_ejecutar, ejecutando);
				break;

		}//Fin del switch
		prot_destruir_mensaje(mensaje_esi);
		pthread_mutex_unlock(&mutex_listas);
		/*Solo para test*/
//		pthread_mutex_lock(&mutex_pausar_continuar);
//		pausado = true;
//		printf("Planificacion pausada\n");
//		printear_todo(NULL);
//		getchar();
		/**/
	}//Fin del while
	pthread_exit(0);
}

bool sigue_ejecutando(t_esi* esi){
	if(esi != NULL){
		return (!esi_a_ejecutar->terminado) && (!esi_a_ejecutar->bloqueado);
	}else{
		return false;
	}
}

void seleccionar_siguiente_esi_a_ejecutar(){
	pthread_mutex_lock(&mutex_listas);
	if(modo_debug){
		printf("[Planificacion]: Buscando proximo ESI a ejecutar\n");
	}
	switch(config_info.algoritmo){
	case FIFO:{
		esi_a_ejecutar = (t_esi*)list_get(listos, 0);
		listo_a_ejecutando(esi_a_ejecutar);
		break;
	}

	case HRRN:{
		list_iterate(listos, &estimar);
		list_iterate(listos, &calcular_responsive_ratio);
		list_sort(listos, &ordenar_por_responsive_ratio);
		esi_a_ejecutar = (t_esi*)list_get(listos, 0);
		listo_a_ejecutando(esi_a_ejecutar);
		break;
	}

	case SJF_SD: case SJF_CD:{
		list_iterate(listos, &estimar);
		list_sort(listos, &ordenar_por_estimacion);
		esi_a_ejecutar = (t_esi*)list_get(listos, 0);
		listo_a_ejecutando(esi_a_ejecutar);
		break;
	}

	default:
		finalizar("No se reconoce algoritmo de planificacion");
		break;
	}

	if(modo_debug){
		printf("[Planificacion]: Proximo ESI elegido\n");
	}

	pthread_mutex_unlock(&mutex_listas);
}

void estimar(void* esi_a_estimar){
	t_esi* esi = (t_esi*) esi_a_estimar;
	if(!esi->estimacion.ya_estimado){
		double anterior = esi->estimacion.estimacion_anterior;
		double real = esi->estimacion.real_anterior;
		double alfa = (((double)config_info.alfa) / (double)100);
		esi->estimacion.estimacion_actual = real * alfa + (1-alfa) * anterior;
		esi->estimacion.ya_estimado = true;
		if(modo_debug){
			printf("[Estimacion]: Estimando ESI <%d>, <%s>\n", esi->ID, esi->nombre);
			printf("\tEstimacion anterior : <%f>\n", anterior);
			printf("\tRafaga anterior : <%f>\n", real);
			printf("\tAlfa : <%f>\n", alfa);
			printf("\tEstimacion actual : <%f>\n", esi->estimacion.estimacion_actual);
			printf("\n");
		}
	}
}

void calcular_responsive_ratio(void* esi_a_calcular){
	t_esi* esi = (t_esi*) esi_a_calcular;
	u_int64_t tiempo_de_espera = tiempo - esi->estimacion.tiempo_ultima_llegada_a_listos;
	esi->estimacion.responsive_ratio = (esi->estimacion.estimacion_actual + tiempo_de_espera) / esi->estimacion.estimacion_actual;
}

bool ordenar_por_estimacion(void* esi_1, void* esi_2 ){
	return ((t_esi*)esi_1)->estimacion.estimacion_actual <= ((t_esi*)esi_2)->estimacion.estimacion_actual;
}

bool ordenar_por_responsive_ratio(void* esi_1, void* esi_2 ){
	return ((t_esi*)esi_1)->estimacion.responsive_ratio >= ((t_esi*)esi_2)->estimacion.responsive_ratio;
}

void mostrar_esi_y_estimacion(void* esi_lista){
	t_esi* esi = (t_esi*)esi_lista;
	printf("\tESI: <%d>, <%s>, estimacion: <%f>\n", esi->ID, esi->nombre, esi->estimacion.estimacion_actual);
}

void desalojar_si_corresponde(){
	if(!list_is_empty(ejecutando) && !list_is_empty(listos)){
		pthread_mutex_lock(&mutex_listas);
		list_iterate(listos, &estimar);
		list_sort(listos, &ordenar_por_estimacion);
		t_esi* posible_desalojador = (t_esi*)list_get(listos, 0);
		double restante_esi_actual = esi_a_ejecutar->estimacion.estimacion_actual - esi_a_ejecutar->estimacion.rafaga_actaual;

		if(posible_desalojador->estimacion.estimacion_actual < restante_esi_actual){
			if(modo_debug){
				printf("ESI <%d> <%s> Desalojado por ESI <%d> <%s>\n", esi_a_ejecutar->ID, esi_a_ejecutar->nombre, posible_desalojador->ID, posible_desalojador->nombre);
			}
			ejecutando_a_listo(esi_a_ejecutar);
			esi_a_ejecutar = posible_desalojador;
			listo_a_ejecutando(esi_a_ejecutar);
			sem_wait(&contador_esis);
		}
		pthread_mutex_unlock(&mutex_listas);
	}
}
