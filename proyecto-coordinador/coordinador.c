#include "coordinador.h"

int main(){
	//Inicializacion de estructuras necesarias
	levantar_configFile();
	prox_instancia_a_seleccionar=0;
	logger = log_create("registros.log", "Coordinador", false, LOG_LEVEL_INFO);
	pthread_mutex_init(&mutex_instancia,NULL);
	pthread_mutex_init(&mutex_mensaje_planificador,NULL);
	pthread_mutex_init(&mutex_registro,NULL);
	sem_init(&productor_consumidor_mensajes_planificador, 0, 0);
	lista_instancias = list_create();
	registro_claves = list_create();

	socket_escucha = iniciar_servidor(config_info.puerto_escucha);
	log_info(logger, "Servidor levantado. Socket de escucha: <%d>", socket_escucha);
	printf("Servidor levantado\n");

	pthread_create(&hilo_Aceptar_Clientes, NULL, (void*)aceptar_clientes_coordinador, NULL); //se crea un hilo para que el main del cordinador pueda hacer otras cosas en paralelo
	pthread_join(hilo_Aceptar_Clientes, NULL);
	return 0;
}
