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

# SINCRONIZACION - 1

Buscamos resolver los problemas de contencion y concurrencia correctamente y con buen rendimiento. Contencion se refiere a que los datos entre los procesos se compartan, o no, correctamente, y concurrencia es poder ejecutar varios procesos en "simultaneo".

Al permitir la concurrencia, se pueden generar lo que se llaman `condiciones de carrera`. Esto es cuando no todas las ejecuciones dan el mismo resultado, segun la ejecucion secuencial particular de los mismos procesos. Basicamente, el orden en el cual se secuencializan los procesos no deberia de influir en el resultado final.

Una solucion a este problema son las `secciones criticas`. Son una parte del codigo que solo una cantidad determinada de procesos particulares pueden acceder en simultaneo, donde todo proceso que este esperando a entrar lo hara eventualmente, y ningun proceso fuera de estas secciones puede bloquear a otro.

Para implementar secciones criticas, podemos utilizar variables atomicas o semaforos.

### Variables atomicas
El problema con la concurrencia es que, a nivel instrucciones de maquina, varias operaciones de lenguajes imperativos no son atomicas, sino que requieren de varias instrucciones de assembler. De cambiar el scheduler de proceso en el medio de una funcion imperativa, podria no haber terminado, y el nuevo proceso tapar los progresos de la misma.

Para solucionar esto, el HW suele proveer instrucciones, como **TAS** (*TestAndSet*), que permiten alterar variables de forma atomica, es decir, indivisible (una sola instruccion de assembler). De este modo, las modificaciones se realizaran antes de atender la interrupcion del scheduler y de ser desalojado el proceso.

### Semaforos
Un semaforo es una variable entera que:
- Se inicializa en un valor.
- `wait()` revisa si el valor es menor o igual a 0. De serlo, se pone a dormir hasta que lo despierten. Luego, decrementa el semaforo en 1.
- `signal()` incrementa el semaforo en 1. Luego, de haber algun proceso esperando al semaforo despierta a alguno.
Es importante que la forma en la que se elija el proximo proceso a despertar sea una cola y no por ejemplo una pila, porque sino puede darse que un proceso quede dormido por siempre.

Pueden tambien generarse *deadlocks*, donde procesos tienen los recursos que necesita el otro para continuar, y ninguno puede avanzar. Existen tambien los *livelocks*, donde procesos continuamente cambian su estado en respuesta a los cambios de estado de otros (ej: proceso detecta poco espacio, otro proceso escribe que hay poco espacio, y se vuelve a detectar que hay poco espacio).

### Locks
Se pueden implementar **locks** con `TestAndSet`, aunque no son muy eficientes ya que producen *Busy Waiting*. Podemos ademas implementar **Spin locks** con bools atomicos, que es un tipo de mutex. Los spinlocks tambien generan busy waiting, pero a su vez producen menos overhead que la utilizacion de semaforos.

### Condiciones de Coffman
Son una serie de condiciones necesarias para la existencia de un deadlock.
- **Exclusion mutua**: Un recurso no puede estar asignado a mas de un proceso.
- **Hold and wait**: Los procesos que ya tienen algun recurso pueden solicitar otro.
- **No preemption**: No hay mecanismo para quitarle los recursos a un proceso.
- **Espera circular**: Tiene que haber un ciclo de mas de 1 proceso, tal que Pi espera un recurso que tiene Pi+1.

### Monitores
Son una estructura de programacion de alto nivel que tambien controla el acceso a `secciones criticas` entre procesos e hilos. Permite que solo un proceso entre a la seccion critica a la vez. 
Se diferencia de un semaforo en que es mas estructurado, y provee herramientas convenientes para la sincronizacion como variables compartidas y procedimientos para accederlas. Por otro lado, los semaforos son variables mas primitivas, donde su funcionalidad se restringe a señalizar y esperar.

### Variables de condicion
Son un mecanismo de sincronizacion que permiten que un proceso espere hasta que se cumpla una determinada condicion antes de continuar ejecutando. Se suelen utilizar dentro de los monitores para coordinar el acceso a recursos compartidos y la sincronizacion.
Proveen dos operaciones principales:
- `wait()`: Un proceso que quiere acceder a la `seccion critica` espera a que se cumpla una condicion. De no estarla, se bloquea y espera a que cambie el estado.
- `signal()`: Un proceso lo envia al liberar un recurso por ejemplo. Luego, cambia el estado de la variable de condicion y notifica a todos, o solo uno, de los procesos bloqueados.

# SINCRONIZACION - 2
Se puede pensar en la correctitud de un sistema de sincronizacion desde los siguientes puntos de vista:
- **Safety**: No pasan cosas malas como *deadlocks*. Tienen un contraejemplo finito.
- **Liveness/progreso**: En algun momento algo bueno sucede. Tienen contraejemplos no finitos.
- **Fairness**: Los procesos reciben su turno con infinita frecuencia. Es decir, no se va a postergar una tarea para siempre. 

### Turnos
El problema de los turnos es, teniendo **n** procesos, hacer que ejecuten una tarea s_i en orden.

### Barrera o Rendezvous
Hay **n** procesos, cada uno tiene que ejecutar *a(i)* y *b(i)*. Luego, queremos garantizar que los *b(j)* se ejecuten despues de todos los *a(i)*.

**WAIT-FREEDOM**: Todo proceso que intenta acceder a la seccion critica, en algun momento lo logra, cada vez que lo intenta. No hayan deadlocks.
**Exclusion mutua (EXCL)**: no puede haber mas de un proceso en la seccion critica.
**LOCK-FREEDOM**: Si existe un proceso que quiere entrar en la zona critica, y en ningun otro proceso se encuenta en la misma, entonces algun proceso entrara en `CRIT`.
**STARVATION-FREEDOM**: No haya inanicion.

# GESTION DE MEMORIA 

Para que un programa se ejecute, debe de encontrarse en memoria RAM. De todos modos, con la multiprogramacion, pueden encontrarse varios programas en RAM en simultaneo, y en consecuente puede darse que haya que quitar uno para agregar otro.

Por un lado, existe el `swapping`, que consiste en pasar a disco el espcio de memoria de los procesos que no se esten ejecutando. El problema es que esto es insostenible por los largos tiempos de acceso a la memoria secundaria. Luego, buscamos solo swappear programas en memoria hasta disco duro de ser necesario.

A su vez, necesitamos que las direcciones en los programas sean relativas a su ubicacion en memoria, porque sino cada vez que las carguemos deberian de encontrarse en la misma.

La fragmentacion de memoria es uno de los problemas a solucionar, que genera espacios de la memoria que quedan inutilizables por procesos.
- Interna: Se le da mas memoria a un proceso de la que utiliza.
- Externa: Se tiene memoria para atender solicitudes pero no es continua.
  
Se intenta solucionar organizando la memoria de distintas formas:
- A traves de un bitmap, dividiendola en bloques de iguales tamaños, y registrando cuales estan ocupados y cuales no. Es un problema encontrar bloques consecutivos (O(n)).
- A traves de una lista enlazada, donde cada nodo representa un proceso o bloque libre. Luego, liberar es O(1), y asignar es O(1) si se a donde.

Para asignar bloques, tenemos:
- **First Fit**: Asigno en el primer bloque que entra. Es rapido pero fragmentacion externa.
- **Best Fit**: Asigno donde entra mas justo. Es mas lento y tampoco es mucho mejor, ya que genera fragmentacion externa pero con bloques mas pequeños.
- **Quick Fit**: Es **Best Fit** pero manteniendo una lista de bloques libres con los tamaños mas frequentes.

### Memoria Virtual
La **Memory Management Unit (MMU)** es la unidad del SO encargada de virtualizar el espacio de direcciones y realizar las traducciones de las mismas. Se ayuda del HW.

- Pongo la direccion en el bus de memoria.
- MMU traduce la direccion virtual a una fisica.
- La tabla de traduccion se fija si esta cargado o no.
- Si no esta, hay que cargarlo.
- La direccion fisica se pone en el bus que va hacia la RAM.
- Obtengo el contenido.
  
Basicamente, se divide la memoria virtual en paginas, y la memoria fisica en page frames. Luego, la MMU traduce las direcciones virtuales como pagina + offset. Al swappear, lo hace con paginas enteras. Si no se encuentra en memoria una pagina, se produce un page fault que maneja el SO.

Para saber que paginas se encuentran cargadas y cuales no, se tiee una tabla de paginas multinivel, lo cual nos permite no tenerla toda en memoria. En cada entrada se encuentra el Page Frame, el bit de ausencia/presencia, privilegios, dirty, etc.

Hay algunas paginas que son muy importantes y se acceden muy seguido. Para ello, se agrega un poco de cache llamado `TLB` que permite mapear paginas a frames uy rapido, sin consultar con las tablas.

### Reemplazo de paginas
- **FIFO**: Desalojo las paginas en el orden de entrada.
- **Second Chance**: Es un **FIFO** donde si la pagina que voy a desalojar fue referenciada, la considero como recien subida y paso a la siguiente.
- **Not Recently Used**: Desalojo primero las que no fueron ni referenciadas ni modificadas. Luego, las referenciadas pero no modificadas, y por ultimo las modificadas.
- **Least Recently Used**: Las ultimas paginas que se utilizaron son las mas probables de volver a ser utilizadas.

Estos algortmos tambien se combinan con carga de paginas por adelantado, aprovechando la *localidad de referencia* de los programas.

### Page Faults
Cuando se emite la interrupcion de `Page Fault`, el kernel checkea si la direccion virtual a la que se queria acceder era valida o si se tenian los permisos para acceder (de no ser asi, se mata al proceso). Luego, se selecciona un page frame libre o se reemplaza alguno ocupado.

Si se swappea, de estar el bit de `dirty` encendido, y estaba modificada, entonces hay que pasarla a disco primero. Luego, se sube la pagina a utilizar. Al finalizar de subirse, se actualiza la tabla de paginas para indicar que esta cargada, y el proceso se desbloquea desde la instruccion que causo el `page fault`.

### Thrashing
Se llama **thrashing** cuando no alcanza la memoria y constantemente se esta cambiando paginas en memoria.

### Proteccion
Cada proceso tiene su probpia tabla de paginas. Con lo cual, no tiene forma de acceder a una pagina de otro proceso, ya que mismas direcciones virtuales apuntarian a distintos frames en la memoria fisica.

Tambien estan los segmentos, los cuales son espacios de memoria que la dividen con diferentes atributos y niveles de privilegio. Estos nos son de tamaño fijo, a diferencia de las paginas, y se suelen utilizar en conjunto. Los segmentos a demas son visibles para el programador, pueden solaparse, facilitan la proteccion y brindan espacios de memorias separados al mismo proceso.

### Fork()
Al Crear un proceso hijo, no se copian las paginas hasta que se escriba alguna. Esto es, cuando uno de los dos procesos escribe alguna, se duplican y cada uno queda con su copia independiente.

# SISTEMA DE E/S 

# SISTEMAS DE ARCHIVOS 

# SISTEMAS DISTRIBUIDOS 

# VIRTUALIZACION 

# SEGURIDAD