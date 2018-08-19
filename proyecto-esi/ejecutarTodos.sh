#!/bin/bash

listaDeScripts=$(ls ./scripts);

for script in $listaDeScripts; do
	echo "Ejecutando script $script";
	./esi.exe scripts/$script;
done
