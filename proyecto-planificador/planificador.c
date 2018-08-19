#include "planificador.h"
//SL significa Sin Leaks

int main(){
	signal(SIGINT, handler_ctrl_c);
	modo_debug = true;
	levantar_configFile(); //--SL
	inicializar_listas(); //--SL
	cerrando = false;
	socket_escucha = iniciar_servidor(config_info.puerto_escucha); //--SL
	socket_coordinador = conectar_a_servidor(config_info.ip_coordinador, config_info.puerto_coordinador, PLANIFICADOR);
	free(config_info.ip_coordinador);

	pthread_create(&hilo_Aceptar_Clientes, NULL, (void*)aceptar_clientes_planificador, NULL);
	pthread_create(&hilo_planificacion, NULL, (void*)planificar, NULL);
	pthread_create(&hilo_interfaz_coordinador_planificador, NULL, (void*)soporte, NULL);
	pthread_create(&hilo_consola, NULL, (void*)iniciar_consola, NULL);

	pthread_join(hilo_Aceptar_Clientes, NULL);
	pthread_join(hilo_planificacion, NULL);
	pthread_join(hilo_interfaz_coordinador_planificador, NULL);
	pthread_join(hilo_consola, NULL);

//	finalizar("Finalizados todos los hilos");
	printf("Cerrando...\n");
	return 0;
}
