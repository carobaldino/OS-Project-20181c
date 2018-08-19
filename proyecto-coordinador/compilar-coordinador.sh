#!/bin/bash

#Version anterior..
#echo "Compilando coordinador";
#cd ~/workspace/tp-2018-1c-Grupo-Negro/proyecto-coordinador;
#gcc algoritmos_distribucion.c conexiones_coordinador.c logueador.c coordinador.c config_coordinador.c ../bibliotecas_compartidas/protocolo.c  ../bibliotecas_compartidas/cliente_servidor.c -o coordinador.exe -lcommons -lpthread;
#echo "Coordinador compilado";


archivosBibliotecaCompartida=$(cd ../bibliotecas_compartidas; ls $PWD/*.c)
referencias="-lcommons -lpthread -lm"

echo "Compilando proyecto coordinador";
gcc *.c ${archivosBibliotecaCompartida} -o coordinador.exe ${referencias};


