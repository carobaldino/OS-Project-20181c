#include "conexiones_planificador.h"

void aceptar_clientes_planificador(){
	int socket_cliente;
	struct sockaddr_in direccion_cliente;
	unsigned int tamanio_direccion = sizeof(direccion_cliente);
	//Burocracia...
	t_prot_mensaje* mensaje_del_cliente;
	// Se aceptan clientes cuando los haya
	// accept es una funcion bloqueante, si no hay ningun cliente esperando ser atendido, se queda esperando a que venga uno.
	while( (!cerrando) &&  (socket_cliente = accept(socket_escucha, (void*) &direccion_cliente, &tamanio_direccion)) > 0){
		//Se recibe un mensaje del cliente
		mensaje_del_cliente = prot_recibir_mensaje(socket_cliente);

		t_header header_recibido = mensaje_del_cliente->head;
		t_cliente cliente_recibido = *(t_cliente*) mensaje_del_cliente->payload;
		prot_destruir_mensaje(mensaje_del_cliente);

		if(cliente_recibido != ESI){
			close(socket_cliente);
		}else{
			mensaje_del_cliente = prot_recibir_mensaje(socket_cliente);
			char* nombre = leer_string_de_mensaje(mensaje_del_cliente);
			if(modo_debug){
			printf("[Conexiones]: Llego nuevo ESI: <%s>\n", nombre);
			}
			agregar_nuevo_esi(socket_cliente, nombre);
			prot_destruir_mensaje(mensaje_del_cliente);
		}
	}//Fin del while

	close(socket_escucha);
	prot_destruir_mensaje(mensaje_del_cliente);
	finalizar("Se desconectó el socket de escucha");
	pthread_exit(0);
}
