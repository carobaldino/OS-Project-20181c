#include "instancia.h"

int main(int argc, char** argv) {
	int a = 5;
	if( a == 3 | 4 | 5 | 6 ){
		printf("hola magdalena\n");
	}
	modo_debug = false; //Poner en true para debuggear
	pausado = false;
	if(argc == 2){
		nombre_por_parametro = true;
		config_info.nombre_instancia = argv[1];
//		char* string_fijo = "/home/utnso/";
		char* string_fijo = "/home/enzo/";
		size_t largo_fijo = strlen(string_fijo);
		size_t largo_variable = strlen(argv[1]);
		config_info.punto_montaje = (char*) malloc(largo_fijo + largo_variable + 1);
		memcpy(config_info.punto_montaje, string_fijo, largo_fijo);
		memcpy(config_info.punto_montaje + largo_fijo , argv[1], largo_variable);
		config_info.punto_montaje[largo_fijo + largo_variable] = '\0';
	}

	levantar_configFile();
	printf("Soy la instancia: %s, punto de montaje %s\n", config_info.nombre_instancia, config_info.punto_montaje);
	mkdir(config_info.punto_montaje, S_IRWXU);


	socket_coordinador = conectar_a_servidor(config_info.ip_coordinador, config_info.puerto_coordinador, INSTANCIA);
	t_prot_mensaje* configuracion_tabla = prot_recibir_mensaje(socket_coordinador);
	t_header header_recibido = configuracion_tabla->head;
	datos_tabla = *(datos_armado_tabla*) configuracion_tabla->payload;
	printf("La Tabla es de %d entradas y el tamaño de cada una es de %d\n",datos_tabla.cant_entrada,datos_tabla.tam_entrada);

	tabla_entradas = list_create();
	espacio_memoria = datos_tabla.cant_entrada * datos_tabla.tam_entrada;
	memoria = malloc(espacio_memoria);
	memset(memoria, 0 , espacio_memoria); //Solo para debug
	bitmap = (t_bitmap_bloques*) malloc( sizeof(t_bitmap_bloques) * datos_tabla.cant_entrada );

	for(int i = 0; i < datos_tabla.cant_entrada; i++){
		bitmap[i].inicio_bloque = memoria + (i * datos_tabla.tam_entrada);
		bitmap[i].ocupado = false;
	}

	printf("Capacidad de memoria total: %d\n",espacio_memoria);
	prot_enviar_mensaje(socket_coordinador, NOMBRE, strlen(config_info.nombre_instancia) ,config_info.nombre_instancia);

	pthread_mutex_init(&mutex_pausar_continuar, NULL);
	pthread_mutex_init(&mutex_dump_compactar, NULL);

	pthread_create(&RecibirMensajesCoordinador,NULL,(void*)&escuchar_coordinador,NULL);
	pthread_create(&hilo_consola, NULL, (void*)consola, NULL);
	pthread_create(&HiloDump,NULL,(void*)dump,NULL);

	pthread_join(RecibirMensajesCoordinador, NULL);

	return 0;
}
