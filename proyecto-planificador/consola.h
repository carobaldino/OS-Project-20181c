#ifndef CONSOLA_H
#define CONSOLA_H

//Includes
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include "listas.h"
#include "finalizar.h"

typedef struct{
	int cantidad_argumentos;
	char** argumentos;
}t_comando;

char* entrada;

/**Declaraciones**/
void iniciar_consola();
t_comando* parsear_entrada(char* entrada);

void bloquear(t_comando* comando);
void desbloquear(t_comando* comando);
void pausar(t_comando* comando);
void continuar(t_comando* comando);
void listar(t_comando* comando);
void kill_consola(t_comando* comando);
void status(t_comando* comando);
void deadlock(t_comando* comando);
void salir(t_comando* comando);
void mostrar_menu(t_comando* comando);
void free_comando(t_comando* comando);
void cerrar_hilo_consola();

void printear_claves_bloqueadas_esi(void* esi_void);
void printear_esi(void* esi_void);
void printear_todo(t_comando* comando);
void debug(t_comando* comando);

void buscar_deadlock_para_un_esi(void* esi_bloqueado);
void listar_esis_bloqueados_por_clave(char* clave);
#endif
