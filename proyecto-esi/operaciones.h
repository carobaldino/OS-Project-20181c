#ifndef OPERACIONES_H_
#define OPERACIONES_H_

#include <parsi/parser.h>
#include "../bibliotecas_compartidas/protocolo.h"
#include "../bibliotecas_compartidas/cliente_servidor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esi.h"

/*Variables globales*/
FILE* script_a_pasear;
char* linea;
bool hay_operacion_pendiente;
t_esi_operacion proxima_operacion;
t_prot_mensaje* resultado_operacion;

void OPERACION_GET();
void OPERACION_SET();
void OPERACION_STORE();
void finalizar(int retorno);
void terminar_operacion(t_header resultado);

#endif /* OPERACIONES_H_ */
