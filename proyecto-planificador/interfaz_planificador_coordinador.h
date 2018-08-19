#ifndef INTERFAZ_PLANIFICADOR_COORDINADOR_H_
#define INTERFAZ_PLANIFICADOR_COORDINADOR_H_

#include "listas.h"
#include "conexiones_planificador.h"
#include "finalizar.h"

t_prot_mensaje* solicitud_coordinador;

void soporte();
void cerrar_hilo_ipc();

#endif /* INTERFAZ_PLANIFICADOR_COORDINADOR_H_ */
