#include "esi.h"

int main(int argc, char** argv){
	signal(SIGINT, &hander_ctrl_c);
	ejecutando_instrucciones = false;
	if(argc != 2) {
		printf("Se debe ingrsar la ruta al script\n");
		return -1; //Error de ruta faltante
	}
	//Abrir el script
	script_a_pasear = fopen(argv[1], "r");

	//Si no puede abrir el script, fopen retorna NULL
	if(script_a_pasear == NULL){
		printf("No se pudo abrir la ruta\n");
		return -1; //Error de ruta invalida
	}

	/*Innecesario, pero lindo para los tests: que el esi sepa su nombre*/
	size_t largo_nombre = strlen(argv[1]) - strlen("script/");
	char* nombre = (char*) malloc(largo_nombre + 1);
	memset(nombre, 0, largo_nombre);
	memcpy(nombre, argv[1] + strlen("scripts/"), largo_nombre);
	nombre[largo_nombre] = '\0';
//	printf("Soy el ESI: %s\n", nombre);

	levantar_configFile(); //carga a config_info
	socket_coordinador = conectar_a_servidor(config_info.coordinador_ip, config_info.coordinador_puerto, ESI);
	free(config_info.coordinador_ip);
	socket_planificador = conectar_a_servidor(config_info.planificador_ip, config_info.planificador_puerto, ESI);
	free(config_info.planificador_ip);
	prot_enviar_mensaje(socket_planificador, NOMBRE, largo_nombre, nombre);
	free(nombre);

	//Para parsear
	linea = malloc(MAXIMO_TAMANIO_LINEA);
	hay_operacion_pendiente = false;

	//Para recibir orden
	t_prot_mensaje* orden_planificador;

	//Cuando el planificador mande la orden de ejectuar se obeteniene proxima instruccion
	//Si no llega la orden el proceso se bloquea porque por dentro hace un recv -es una funcion bloqueante-
	//Si no hay mas lineas para parsear sale del ciclo
	while( (orden_planificador = prot_recibir_mensaje(socket_planificador))->head == EJECUTAR_SCRIPTS /*Llega orden de ejecutar*/){
		printf("Llego orden de ejecucion\n\n");
		ejecutando_instrucciones = true;
		prot_destruir_mensaje(orden_planificador);
		/*Si el ESI vuelve de un bloqueo debe seguir ejecutando la instruccion que lo bloqueo*/
		if(!hay_operacion_pendiente){

			/*Si no hay operacion pendiente entonces se debe obetener la proxima operacion*/
			if(fgets(linea, MAXIMO_TAMANIO_LINEA, script_a_pasear) != NULL){
				//Parsear la linea leida
				proxima_operacion = parse(linea);
			}else{
				/*No hay mas lineas de script, deberia salir del ciclo*///Parsear la linea leida
				printf("Fin de script\n");
				prot_enviar_mensaje(socket_planificador, SCRITP_FINALIZADO, 0, NULL);
				prot_enviar_mensaje(socket_coordinador, SCRITP_FINALIZADO, 0, NULL);
				finalizar(FINALIZAR_CORRECTAMENTE);
				break;
			}
		}

		//Verifica que lo que se parseo es valido
		if(proxima_operacion.valido){
			hay_operacion_pendiente = true;
			switch(proxima_operacion.keyword){

				case GET:{
					OPERACION_GET();
					break;
				}

				case SET:{
					OPERACION_SET();
					hay_operacion_pendiente = false;
					break;
				}

				case STORE:{
					OPERACION_STORE();
					hay_operacion_pendiente = false;
					break;
				}

				default:{
					printf("No pude interpretar <%s>\n", linea);
					finalizar(FINALIZAR_CON_ERROR);
				}
			}

			ejecutando_instrucciones = false;

		}else{
			printf("No se pudo parsear la linea\t%s\n", linea);
			destruir_operacion(proxima_operacion);
			prot_enviar_mensaje(socket_planificador, EJECUCION_FALLO, 0, NULL);
			finalizar(FINALIZAR_CON_ERROR);
		}
	}
	//Por si el mensaje que llega es raro
	prot_destruir_mensaje(orden_planificador);
	//Cerrando
	finalizar(FINALIZAR_CORRECTAMENTE);
}

void hander_ctrl_c(){

	void* buffer = malloc(sizeof(t_header));

	if(ejecutando_instrucciones){
		prot_enviar_mensaje(socket_planificador, DESCONEXION, 0, NULL);
		recv(socket_coordinador, buffer, sizeof(t_header), 0);

	}else{
		prot_enviar_mensaje(socket_coordinador, DESCONEXION, 0, NULL);
		recv(socket_planificador, buffer, sizeof(t_header), 0);
	}

	free(buffer);

	finalizar(FINALIZAR_CON_ERROR);
}
