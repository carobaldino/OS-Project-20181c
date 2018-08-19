#ifndef ALGORITMOS_PLANIFICACION_H
#define ALGORITMOS_PLANIFICACION_H_

#include "listas.h"
#include "conexiones_planificador.h"
#include "finalizar.h"

/*Globales*/
unsigned long tiempo; //Para simular el paso del tiempo
t_prot_mensaje* mensaje_esi;

/**
* @NAME: planificar
* @DESC: hace magias llamadas fifo, sjf, hrrn y todo eso de planificar. Comentado dentro
* @PARAMS:
*		socket_victima - socket que se desconecta y se tiene que terminar
*/
void planificar();
void seleccionar_siguiente_esi_a_ejecutar();
void estimar(void* esi_a_estimar);
void calcular_responsive_ratio(void* esi_a_calcular);
bool ordenar_por_estimacion(void* esi_1, void* esi_2);
bool ordenar_por_responsive_ratio(void* esi_1, void* esi_2);
bool sigue_ejecutando(t_esi* esi);
void cerrar_hilo_planificacion();
void mostrar_esi_y_estimacion(void* esi_lista);
void desalojar_si_corresponde();

#endif /* ALGORITMOS_PLANIFICACION_H_ */
