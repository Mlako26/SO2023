# INTRODUCCION

Los Sistemas Operativos son una pieza de software que hacen de intermediario entre el HW y los programas de usuario. Tienen dos funciones principales:
- Para el usuario, se encargan de que el software especifico no se tenga que preocupar con detalles de bajo nivel del HW.
- Para el dueño del HW, se encargan de que el usuario utilice correctamente el HW.
Deben de manejar la contención (acceso a mismos recursos por parte de varios programas) y la concurrencia con el mejor rendimiento posible. Debe ademas administrar recursos, y funcionar como una interfaz de programacion. Para ello, corre con privilegio 0.

Nace desde que se pasan de sistemas batch, donde los programas llegan en tandas, a sistemas con multiprogramación, donde mientras se lee un programa o se acceden a dispositivos, el procesador pueda procesar otro trabajo. En este paradigma, se necesitaba de un software que permitiese contener los recursos y manejar accesos en simultaneo. Nacen los SO interactivos, donde el usuario puede interactuar con el mismo.

Elementos de un SO:
- Drivers: Programas del SO que manejan la operación de los distintos dispositivos a bajo nivel.
- Kernel: Es el propio SO. Encargado de tareas fundamentales, y contiene distintos subsistemas.
- Interprete de comandos: Programa que permite al usuario interactuar con el SO.
- Procesos: Programas en ejecucion mas su espacio de memoria y atributos asociados.
- Sistema de Archivos: Formade organizar los datos en el disco para gestionar su acceso, permisos, etc.
- Archivo: Secuencia de bits con nombre y atributos.
- Directorio: Coleccion de archivos y otros directorios con nombre y organizado jerarquicamente.
- Archivo de configuración: Donde se saca la informacion para funcionar el SO. Ej: /etc/passwd.
- Usuario: La representacion de las personas/entidades que lo utilizan. Existen para aislar informacion entre si.
- Grupo: Coleccion de usuarios.

Hay dos tipos principales de SO:
- Monolitico, donde todas las funcionalidades y programas se encuentran integrados al kernel. Mientras que ocupa mucho espacio y un crash de un servicio podria tirar abajo todo el sistema, suelen ser mas rapidos (al estar todo en el kernel).
- Microkernel, enfoque modular, donde solo las funcionalidades escenciales se implementan en el nucleo, mientras que los controladores y otros servicios se ejecutan como procesos fuera del kernel en modo usuario.

# PROCESOS, SEÑALES Y LLAMADAS AL SISTEMA 

Un programa es una secuencia de pasos escrita en algun lenguaje. Un proceso es un programa en ejecucion. Cada uno tiene un ID unico.

Un proceso tiene:
- Un area de `texto`, con el codigo maquina del programa.
- Un area de `datos`, donde almacena el heap.
- Su `stack`.

Pueden iniciarse, terminar, lanzar otros procesos, realizar system calls y E/S, etc. Los procesos estan organizados en un arbol jerarquico de procesos, donde todos tienen una relacion de padre/hijo.

**Terminacion**: El proceso indica al sistema operativo que puede liberar sus recursos, indicando su estatus de terminacion.

**Ejecutacion en CPU**: Puede realizar operaciones entre registros/memoria, E/S y syscalls. El tiempo que puede ejecutar en el mismo esta regulado por el Scheduler, el cual cambia los programas constantemente. Para hacerlo, se debe de guardar el estado del programa (IP, regs, etc) y cargar el del nuevo (context switch). Todo esto se almacena en la `PCB` (Process Control Block).

**Llamadas al sistema**: Se llama al kernel y se ejecuta una subrutina con cambio de privilegio. Estas son una interfaz a los servicios brindados por el SO, su `API` (Application Programming Interface). Se utilizan usualmente con un wrapper en C, que proveen portabilidad y sencillez.

**Entrada y Salida**: La entrada y salida es muy lenta, y durante la misma el proceso se encuentra bloqueado hasta que termine, con lo cual de hacer busy waiting (proceso no libera la CPU) se gastarán ciclos del CPU. Alternativas para solucionar esto es hacer polling (proceso libera CPU pero recibe quantum hasta que la E/S termine) o el uso de interrupciones (proceso libera CPU, no quantum, el HW comunica que terminó la E/S y la interrupcion es atendida por el SO, que despierta al proceso).

Algunas definiciones:
- Multiprogramacion: Capacidad de un SO de tener varios procesos en ejecucion.
- Multiprocesamiento: Tipo de procesamiento de equipos con más de un CPU.
- Multitarea: Forma especial de multiprogramacion, donde los context switch son tan eficientes que da la sensacion de varios programas corriendo en simultaneo.

Los procesos tienen entonces distintos estados, donde pueden estar `corriendo`, `bloqueado` o `listo`. En orden, indican estar en la CPU, esperando el resultado de una E/S, o esperando a que le toque su quantum.

Para manejarlos, se tiene una lista de PCBs, la *tabla de procesos*, utilizada por el scheduler. En cada PCB se guardan cosas como la prioridad del proceso, su estado, y recursos por los que espera.

Las `señales` son un mecanismo de comunicacion incorporada por sistemas POSIX (un estandard). Permiten notificar a un proceso la ocurrencia de un evento. Cuando uno recibe una señal, se pausa su ejecucion y se pasa al handler. Cada señal tiene su propio handler default, modificable con `signal()`. Hay dos especiales, `SIGKILL` y `SIGSTOP` que no pueden ser alteradas su handler ni bloqueadas, indican la terminacion del proceso. `kill()` es la funcion para enviar señales, las cuales pueden ser enviadas entre procesos.

# COMUNICACION INTER-PROCESOS 



# SCHEDULING 

# SINCRONIZACION 

# GESTION DE MEMORIA 

# SISTEMA DE E/S 

# SISTEMAS DE ARCHIVOS 

# SISTEMAS DISTRIBUIDOS 

# VIRTUALIZACION 

# SEGURIDAD