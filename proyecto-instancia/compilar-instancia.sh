#!/bin/bash

#echo "Compilando INSTANCIA";
#cd ~/workspace/tp-2018-1c-Grupo-Negro/proyecto-instancia;
#gcc instancia.c ../bibliotecas_compartidas/cliente_servidor.c config_instancia.c ../bibliotecas_compartidas/protocolo.c -o instancia.exe -lcommons;
#echo "INSTANCIA compilado";

archivosBibliotecaCompartida=$(cd ../bibliotecas_compartidas; ls $PWD/*.c)
referencias="-lcommons -lpthread -lm -lreadline"

echo "Compilando proyecto instancia";
gcc *.c ${archivosBibliotecaCompartida} -o instancia.exe ${referencias};
