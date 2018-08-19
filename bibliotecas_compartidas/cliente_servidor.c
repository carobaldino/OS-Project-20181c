#include "cliente_servidor.h"

int iniciar_servidor(int puerto){
	int socket_escucha;
	crear_socket(&socket_escucha);
	//Codigo que permite hacer pruebas sin que se bloqueen los puertos
	int activado = 1;
	setsockopt(socket_escucha, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
    //
	reservar_puerto(socket_escucha, puerto); //bind
	escuchar_puerto(socket_escucha, MAX_CLIENTES_ENCOLADOS); //listen
	return socket_escucha;
}

void escuchar_puerto(int socket, int maximo_cliente_encolados){
	//Habilita a que los clientes es conecten
	if(listen(socket, maximo_cliente_encolados) <0){
		close(socket);
		print_error(ERROR_ESCUCHAR_PUERTO);
		exit(VALOR_EXIT);
	}
}

void reservar_puerto(int socket, int puerto){
	//Burocracia inicial
	struct sockaddr_in direccion_servidor;
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_addr.s_addr = INADDR_ANY;
	direccion_servidor.sin_port = htons(puerto);
	//Fin de la burocracia

	//Bind: <CREO> que le pide un puerto al SO, lo reserva para él y le asocia un puerto
	if( bind(socket, (void*) &direccion_servidor, sizeof(direccion_servidor)) <0){
		close(socket);
		print_error(ERROR_RESEVAR_PUERTO);
	}
}

void print_error(char *mensaje){
	printf("ERROR:\t%s\n", mensaje);
	sleep(TIEMPO_SLEEP);
}

void crear_socket(int *socket_destino){
	*socket_destino = socket(AF_INET, SOCK_STREAM, 0);
	//Si no se puede crear socket retorna -
	if(socket_destino <0){
		close(*socket_destino);
		print_error(ERROR_CREAR_SOCKET);
		exit(VALOR_EXIT); // Si no se puede crear el socket deberia cerrarse el proceso... no ?
	}
}

void conectar_socket(int socket_cliente_, int puerto, char *ip){
	//Burocracia inicial
	struct sockaddr_in direccion_servidor;
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_addr.s_addr = inet_addr(ip);
	direccion_servidor.sin_port = htons(puerto);
	//Fin de la burocracia

	//Conectarse con el IP - Puerto del servidor
	if(connect(socket_cliente_, (void*) &direccion_servidor, sizeof(direccion_servidor)) < 0){
		close(socket_cliente_);
		print_error(ERROR_CONECTAR_SERVIDOR);
		exit(1);
	}
}

int conectar_a_servidor(char* ip, int puerto, t_cliente cliente){
	int socket_cliente;
	crear_socket(&socket_cliente);
	conectar_socket(socket_cliente, puerto, ip);
	prot_enviar_mensaje(socket_cliente, CONEXION, sizeof(t_cliente), &cliente);;
	return socket_cliente;
}

//Funciones bobas para tests y esas cosas, borrar en un futuro
char* tipo_de_cliente(t_cliente cliente){
	switch(cliente){
	case ESI:
		return "ESI";
		break;

	case PLANIFICADOR:
		return "Planificador";
		break;

	case INSTANCIA:
		return "Instancia";
		break;
	default:
		return "Cliente no reconocido";
		break;
	}
}
