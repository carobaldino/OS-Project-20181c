#!/bin/bash

#Version anterior..
#echo "Compilando ESI";
#cd ~/workspace/tp-2018-1c-Grupo-Negro/proyecto-esi;
#gcc esi.c operaciones.c ../bibliotecas_compartidas/cliente_servidor.c config_esi.c ../bibliotecas_compartidas/protocolo.c -o esi.exe -lcommons -lpthread -lparsi;
#echo "ESI compilado";

archivosBibliotecaCompartida=$(cd ../bibliotecas_compartidas; ls $PWD/*.c)
referencias="-lcommons -lpthread -lparsi"

echo "Compilando proyecto ESI";
gcc *.c ${archivosBibliotecaCompartida} -o esi.exe ${referencias};
