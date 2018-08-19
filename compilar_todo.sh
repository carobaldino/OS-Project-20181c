#!/bin/bash

# -*- ENCODING: UTF-8 -*-

#echo "Compilando todo"
#cd ~/workspace/tp-2018-1c-Grupo-Negro/proyecto-coordinador;
#source compilar-coordinador.sh;

#cd ~/workspace/tp-2018-1c-Grupo-Negro/proyecto-planificador;
#source compilar-planificador.sh;

#cd ~/workspace/tp-2018-1c-Grupo-Negro/proyecto-esi;
#source compilar-esi.sh;

#cd ~/workspace/tp-2018-1c-Grupo-Negro/proyecto-instancia;
#source compilar-instancia.sh;

#cd ..;

directorioActual=$(pwd)

echo "Compilando todo"

cd ${directorioActual}/proyecto-coordinador;
source compilar-coordinador.sh;

cd ${directorioActual}/proyecto-planificador;
source compilar-planificador.sh;

cd ${directorioActual}/proyecto-esi;
source compilar-esi.sh;

cd ${directorioActual}/proyecto-instancia;
source compilar-instancia.sh;

cd ..;

