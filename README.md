# TP Re Distinto - Grupo Negro - 1c 2018 :smile:

## Checkpoint 1 - 28 de Abril
Distribución de recursos: 5 integrantes

- [x] Aplicar las Commons Libraries, principalmente las funciones para liestas, archivos de conf y logs
- [x] Definir el protocolo de comunicación
- [x] Familiarizarse con el desarrollo de procesos servidor multihilo
- [x] Creación de todos los procesos que intervienen
- [x] Desarrollar una comunicación simple entre los procesos que permita propagar un mensaje por cada conexión necesaria
- [x] Implementar la consola del planificador sin funcionalidades 


## Checkpoint 2 - 19 de Mayo
Distribución de recursos: 1 Planificador - 1 Coordinador - 1 Instancia - 2 Testing general y arreglos

- [x] Implementar el protocolo de comunicación
- [ ] Comprender y aplicar mmap()
- [x] Entender el concepto de shared library
- [x] Comprender con algo de profundidad cómo funcionan algunos algoritmos sencillos
- [x] Lectura de scripts y utilización del Parser del proceso ESI
- [x] El Planificador debe ser capaz de elegir a un ESI utilizando un algoritmo sencillo (FIFO por ej)
- [ ] El Coordinador debe ser capaz de distribuir por Equitative Load
- [ ] Desarrollo de lectura y escritura de Entradas en la Instancia (Operaciones GET/SET)


## Checkpoint 3 - 9 de Junio
Distribución de recursos: 1 ESI - 2 Planificador - 1 Coordinador - 1 Instancia

- [x] Entender el concepto de productor-consumidor y sus problemas de concurrencia
- [ ] Implementar algoritmos similares a los usados en Memoria Virtual
- [x] ESI completo
- [x] Planificador usando SJF con y sin desalojo, con todas sus colas
- [ ] La consola del planificador deberá poder ejecutar los comandos:
   - [x] Pausar
   - [x] Continuar
   - [ ] Bloquear
   - [ ] Desbloquear
   - [ ] Listar
- [ ] El Coordinador deberá tener el Log de Operaciones funcionando
- [ ] El Coordinador deberá ser capaz de comunicar bloqueos
- [ ] La Instancia deberá implementar todas las instrucciones
- [ ] La Instancia deberá implementar el algoritmo Circular para reemplazar claves



## Checkpoint 4 - 30 de Junio
Distribución de recursos: 2 Planificador - 2 Coordinador - 1 Instancia

- [ ] Utilizar el concepto de "interfase" para que cada proceso planifique de forma similar, soportando distintos algoritmos
- [x] Planificador utilizando HRRN
- [ ] La consola del planificador deberá poder ejecutar los comandos:
   - [ ] Kill
   - [ ] Status
- [ ] El Coordinador deberá ser capaz de distribuir utilizando:
   - [ ] LSU
   - [ ] KE 
- [ ] El Coordinador deberá implementar retardos
- [ ] La Instancia deberá ser capaz de soportar desconexiones y reincorporaciones.
- [ ] La Instancia deberá implementar el algoritmo LRU



## Entrega Final - Presencial - Laboratorio - 14 de Julio - 28 de Julio - 4 de Agosto
Distribución de recursos: 1 Planificador - 1 Coordinador - 1 Instancia - 2 Testing general y arreglos

- [ ] Implementar un algoritmo de detección de deadlocks
- [ ] Probar el TP en un entorno distribuido
- [ ] Finalizar el desarrollo de todos los procesos
- [ ] La consola del Planificador deberá poder ejecutar el comando "deadlock"
- [ ] La Instancia deberá ser capaz de soportar dumps y compactación
- [ ] La Instancia deberá poder implementar el algoritmo BSU






## Tips
```
Para compilar todo, poner en la terminal lo siguiente (primero moverse hasta el repo)
$./compilar_todo.sh

Se debera ejectura de la forma ./esi.exe ../script.esi
o cambiando el ../script.esi por la ruta+archivo indicado
```
