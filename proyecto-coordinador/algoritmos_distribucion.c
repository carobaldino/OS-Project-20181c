#include "algoritmos_distribucion.h"

bool aplicar_algoritmo_distribucion(char* clave){

	if(!list_is_empty(lista_instancias)){
		pthread_mutex_lock(&mutex_instancia);
		switch(config_info.algoritmo_distribucion){
			case EL:
				printf("[GET]: distribuyendo por Equitative Load\n");
				return distribuirPorEL(clave);
				break;
			case LSU:
				printf("[GET]: distribuyendo por Least Space Used\n");
				return distribuirPorLSU(clave);
				break;
			case KE:
				printf("[GET]: distribuyendo por Key Explicit\n");
				return distribuirPorKE(clave);
				break;
			default:
				printf("No se detecto algoritmo\n");
				return false;
				break;
		}
	}else{
		log_warning(logger, "No hay ninguna instancia conectada");
		return false;
	}
}

bool distribuirPorEL(char* clave){
	t_instancia* instancia_a_elegir = (t_instancia*) list_get(lista_instancias, prox_instancia_a_seleccionar % lista_instancias->elements_count);
	prox_instancia_a_seleccionar++;
	pthread_mutex_unlock(&mutex_instancia);
	return guardarEnInstancia(clave, instancia_a_elegir);
}

bool distribuirPorLSU(char* clave){
	list_sort(lista_instancias, &ordenar_por_espacio_restante);
	t_instancia* instancia_a_eligir = (t_instancia*) list_get(lista_instancias,0);
	if(guardarEnInstancia(clave, instancia_a_eligir)){
		instancia_a_eligir->espacio_utilizado -= strlen(clave);
		pthread_mutex_unlock(&mutex_instancia);
		return true;
	}else{
		pthread_mutex_unlock(&mutex_instancia);
		return false;
	}
}

bool ordenar_por_espacio_restante(void* instancia_void_1, void* instancia_void_2){
	/*Retorna si el primero elemento va antes que el segundo*/
	t_instancia* instancia1 = (t_instancia*) instancia_void_1;
	t_instancia* instancia2 = (t_instancia*) instancia_void_2;

	int espacio_libre_1 = instancia1->espacio_memoria_total - instancia1->espacio_utilizado;
	int espacio_libre_2 = instancia2->espacio_memoria_total - instancia2->espacio_utilizado;

	return espacio_libre_1 > espacio_libre_2;
}

bool distribuirPorKE(char* clave){
	if( ((*clave) <= ('z')) && ((*clave) >= ('a')) ){
		int letras_por_instancia = (int) ceil((double)CANTIDAD_DE_LETRAS / (double)lista_instancias->elements_count);
		t_instancia* instancia_a_elegir = (t_instancia*) list_get(lista_instancias, ((int)(*clave) - 'a') / letras_por_instancia );
		pthread_mutex_unlock(&mutex_instancia);
		return guardarEnInstancia(clave, instancia_a_elegir);
	}
	else{
		/*Esto va porque no esta contemplado una
		 * clave que no empieze con una caracter
		 * que no valla de a - z*/
		printf("Primer letra de clave <%s> es invalida\n", clave);
		log_warning(logger, "Primer letra de clave <%s> es invalida", clave);
		pthread_mutex_unlock(&mutex_instancia);
		return false;
	}
}

bool guardarEnInstancia(char* clave, t_instancia* instancia){ //HAY QUE FIRMALA
	prot_enviar_mensaje(instancia->socket_instancia,GUARDAR_CLAVE,strlen(clave),clave);
	log_info(logger,"[GET-Instancias]: Enviando clave <%s> a guardar en instancia <%s>", clave, instancia->nombre);
	printf("[GET-Instancias]: Enviando clave <%s> a guardar en instancia <%s>\n", clave, instancia->nombre);
	sem_wait(instancia->productor_consumidor_de_esta_instancia);
	t_prot_mensaje* respuesta = instancia->mensaje_instancia;

	if (respuesta->head == CLAVE_GUARDADA){
		prot_destruir_mensaje(respuesta);
		t_registro_claves* nueva_clave_distribuida = (t_registro_claves*) malloc (sizeof(t_registro_claves));
		nueva_clave_distribuida->clave = strdup(clave);
		nueva_clave_distribuida->instancia = instancia;
		nueva_clave_distribuida->valor = NULL;
		list_add(registro_claves, nueva_clave_distribuida);
		log_info(logger,"[GET-Instancias]: Clave <%s> guardada en instancia <%s>", clave, instancia->nombre);
		return true;
	}
	else{
		prot_destruir_mensaje(respuesta);
		printf("[GET-Instancias]: ERROR: No se pudo guardar la clave\n");// LOGGER!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		log_info(logger,"[GET-Instancias]: ERROR: No se pudo guardar la clave <%s> en la instancia <%s>", clave, instancia->nombre);
		return false;
	}
}

t_instancia* seleccionar_instancia_para_distribuir_clave(char* clave){
	if(!list_is_empty(lista_instancias)){

		switch(config_info.algoritmo_distribucion){
			case EL:{
				return (t_instancia*) list_get(lista_instancias, prox_instancia_a_seleccionar % lista_instancias->elements_count);
				break;
			}
			case LSU:{
				list_sort(lista_instancias, &ordenar_por_espacio_restante);
				return (t_instancia*) list_get(lista_instancias,0);
				break;
			}
			case KE:{
				if( ((*clave) <= ('z')) && ((*clave) >= ('a')) ){
					int letras_por_instancia = (int) ceil((double)CANTIDAD_DE_LETRAS / (double)lista_instancias->elements_count);
					return (t_instancia*) list_get(lista_instancias, ((int)(*clave) - 'a') / letras_por_instancia );
				}
				else{
					printf("Primer letra de clave <%s> es invalida. Por lo que no se distribuiria\n", clave);
					return NULL;
				}

				break;
			}
			default:
				printf("No hay algoritmo: exit\n");
				exit(0);
				return NULL;
				break;
		}
	}else{
		printf("[Status]: No ha ninguna instancia conectada, por lo tanto no se podría distribuir");
		return NULL;
	}
}
