
#ifndef GET_SET_STORE_H_
#define GET_SET_STORE_H_

#include "estructuras-comunes.h"
#include "consola-instancia.h"

typedef struct{
	char* valor;
	t_tabla_entradas* entrada;
}t_backup;

pthread_t RecibirMensajesCoordinador;
t_list* backup;

void escuchar_coordinador();
void guardar_clave(t_prot_mensaje* mensaje_coordinador);
void setear_valor(t_prot_mensaje* mensaje_coordinador);
void storear_valor(t_prot_mensaje* mensaje_coordinador);
void escribir_valor(char* valor, int entrada_inicial, t_tabla_entradas* entrada_con_clave, int entradas_utilizadas);
void asignar_bit_ocupado(int inicio, int cantidad, bool estado);
void buscar_entradas_contiguas_y_asignar(char* valor, t_tabla_entradas* entrada_con_clave, int cantidad_entradas_necesarias);
void insertar_nuevo_valor_en_storage(char* valor, t_tabla_entradas* entrada_con_clave, int cantidad_entradas_necesarias);
void compactar();
void reemplazar();
void backupear_valor(void* entrada);
bool esta_en_storage(void* entrada);
void borrar_backup(void* back_en_lista);
void escribir_en_storage(void* back);
#endif /* GET_SET_STORE_H_ */
