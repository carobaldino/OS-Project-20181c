#!/bin/bash

#echo "Compilando planificador";
#cd ~/workspace/tp-2018-1c-Grupo-Negro/proyecto-planificador;
#gcc planificador.c finalizar.c soporte.c conexiones_planificador.c ../bibliotecas_compartidas/cliente_servidor.c config_planificador.c consola.c ../bibliotecas_compartidas/protocolo.c listas.c algoritmos_planificacion.c -o planificador.exe -lcommons -lreadline -lpthread;
#echo "Planificador compilado";	

#-g para que salgan mas cosas en el debug

archivosBibliotecaCompartida=$(cd ../bibliotecas_compartidas; ls $PWD/*.c)
referencias="-lcommons -lreadline -lpthread"

echo "Compilando proyecto planificador";
gcc *.c ${archivosBibliotecaCompartida} -o planificador.exe ${referencias};
