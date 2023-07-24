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

La comunicacion sirve para compartir informacion, mejorar la velocidad de procesamiento y modularizar, y puede ser realizada entre procesos en un mismo equipo o remotos. Hay varias formas de IPC.

### Memoria Compartida
La comunicacion se realiza compartiendo un mismo area de memoria, donde ambos procesos pueden leer o leer/escribir.

### Pipes
Un pipe es un "pseudo archivo" que habilita una forma de IPC, indicados en linux con el simbolo `|`.

Utilizan la existencia de los *File Descriptors*, los cuales son indices en una tabla donde las posiciones representan instancias de archivos abiertos por el proceso. Cada proceso tiene la suya en la PCB, y son utilizados por el Kernel para saber que archivo tiene abierto cada proceso (y por ende puede leer o escribir).
Con `open()`, `close()` y `dup2()` podemos modificar la tabla. Ademas, es importante saber que los *File Descriptors* se heredan al crear un `fork()`.

Hay indices predeterminados, donde 0 es el `stdin`, 1 el `stdout` y 2 el `stderr`. Luego, con `read()` y `write()`, dado un *File Descriptor* se puede leer o escribir una cantidad determinada de bytes.

Por ultimo, los **Pipes** hacen uso de estos, redirigiendo la entrada de `stdout` del primer programa a la de `stdin` del segundo. El **pipe** en si es un archivo temporal que sirve como buffer, interpretado como un byte stream. Se crean con la syscall `pipe()`.

# SCHEDULING 

La politica de scheduling es de las partes mas importantes de un SO, y grandes partes de los esfuerzos por optimizar el rendimiento de un SO se invierten en mejorar la misma.

Los objetivos a mejorar son:
- **Fairness**: Que cada proceso reciba una dosis "justa" de CPU.
- **Eficiencia**: Tratar que el CPU nunca descanse.
- **Carga del sistema**: Minimizar la cantidad de procesos en estado de `listo`.
- **Response Time**: Minimizar el tiempo de respuesta percibido por los usuarios interactivos.
- **Latency**: Minimizar el tiempo requerido para que un proceso empiece a dar resultados.
- **Turnaround**: Minimizar el tiempo total de un proceso hasta terminar.
- **Throughput**: Maximizar el numero de procesos terminados por unidad de tiempo.
- **Waiting time**: Minimizar la suma de los periodos en `listo`.
- **Liberacion de recursos**: Priorizar los procesos con mas recursos asociados.
Claramente, muchos de estos objetivos son contradictorios, con lo cual se deben de priorizar algunos (por ejemplo, no se puede garantizar la fairness si se priorizan los procesos con mas recursos).


El scheduling puede ser `cooperativo` o `con desalojo`. 

De ser el primero, cada proceso puede ocupar la CPU tanto como quiera hasta finalizar de procesar. Esto es bueno de tener tareas con poca E/S, pero de tener mucha o de colgarse, no solo se desperdiciarian rafagas de CPU, sino que podria colgar todo el sistema (nadie lo va a desalojar).

De ser el segundo, `preemtive`, el scheduler utiliza la interrupcion del reloj para decidir si el proceso actual sigue ejecutandose o le toca a otro. Esto es bueno ya que permite el desalojo de E/S bloqueantes y programas que no terminan nunca, y suele mejorar la **Fairness** del sistema, agrega un overhead al momento de realizar los cambios de contexto.

### FIFO
Un enfoque de tipo `First In First Out` supone todos los procesos iguales, y se ejecutan en el orden en el que llegan. Esto es un problema en el caso en que llegue un proceso que requiera mucha CPU, y demora procesos que en poco tiempo hubiesen ya terminado de procesar.

### Round-Robin
Este enfoque le otortga un quantum a cada proceso, y alterna entre ellos siguiendo una cola en orden. Un quantum muy largo puede perjudicar el **Response Time** de los sistemas interactivos, pero un quantum muy pequeño genera mucho overhead en el context switch.

### Shortest-Job-First
Se asocia a cada proceso el largo de su proxima rafaga de CPU y se elige para ejecutar el proceso con menor. De este modo, se minimiza el **Waiting Time** y se mejora el **Throughput**. De todos modos, es imposible saber cuanta rafaga de CPU requiere cada proceso exactamente, aunque se puede aproximar. Ademas, en caso de llegar muchos procesos con rafagas cortas constantemente, se generaria `starvation` de los demas.

### Multiples Colas
Se asocia cada cola a una prioridad distinta, y a la hora de elegir el proximo proceso la prioridad siempre la tiene la cola con menos quantum. Cuando un proceso no finaliza en su quantum, se lo baja a la siguiente cola con menos prioridad, pero se le asignara mas CPU en el proximo turno. Se puede ademas implementar con `aging`, donde cuanto mas tiempo estuvo el proceso sin terminar mas prioridad tendra, y subiendo la prioridad tambien cada vez que llama a una E/S.

### RT Scheduling
Para los sistemas de Tiempo Real, donde los procesos tienen *deadlines* para finalizar, se puede utilizar por ejemplo `Earliest-Deadline-First`, donde se priorizan los procesos que se deben de finalizar antes.

### SMP Scheduling
Con SMP, donde tenemos mas de un procesador y cache, se intenta tratar de utilizar siempre el mismo procesador para cada proceso, `afinidad al procesador`, la cual puede ser blanda o dura. Tambien, se intenta distribuir la carga sobre los procesadores.

Elegir una buena politica de scheduling es dificil, ya que entran en juego muchas variables, distintos tipos de procesos, distintas prioridades y distintos HW. Suele requerir extensas prueba/error.

# SINCRONIZACION 



# GESTION DE MEMORIA 

# SISTEMA DE E/S 

# SISTEMAS DE ARCHIVOS 

# SISTEMAS DISTRIBUIDOS 

# VIRTUALIZACION 

# SEGURIDAD