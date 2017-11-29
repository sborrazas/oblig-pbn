# Cola de Mensajes – Obligatorio PBN

Sistema de software para distribuir y monitorear mensajes entre distintos
sistemas.

## Estructura del proyecto

* `src/` – fuentes
* `scripts/` – scripts para inicializar las aplicaciones
* `build/` – archivos compilados (.o y ejecutables)

## Correr el proyecto

Para correr el proyecto es necesario crear el archivo /etc/message_queue.mem y
otorgar pemisos de escritura/lectura al usuario y grupo que ejecuta el programa.

```
$ sudo touch /etc/msg_queue.mem
$ sudo chown <miuser>:<migrupo> /etc/msg_queue.mem
```

Luego para correr la cola de mensajes (el archivo de logs será colocado en `~/msg_queue.log`):

```
$ ./scripts/run_message_queue.sh
```

Para correr un orígen/procesador:

```
$ ./scripts/run_origin.sh
$ ./scripts/run_processor.sh
```

Para ver los logs en tiempo real:

```
$ tail -f ~/msg_queue.log
```

Para correr múltiples orígenes y procesadores de forma automática:

```
$ ./scripts/run_all.sh
```

## Formatting

Los cambios deberán ser formateados de acuerdo a los siguientes lineamientos:

* El máximo de línea es de 80 caracteres.
* La indentación es de 4 espacios (sin tabs).
* Variables y nombres de funciones con `snake_case`.

## Autores

* Sebastián Borrazás – 166089
* Tania Cagnoli – 154307
