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

Los dispositivos de E/S permiten la comunicación con el mundo físico exterior a la computadora, almacenar datos, etc. Se componen conceptualmente de:
- El dispositivo fisico.
- Un **controlador**, el cual interactua con el SO mediante algun `bus` o `registro`.

### Drivers
Son componentes del SO que conocen las particularidades del HW con el que hablan. Corren con maximo privilegio y de ellos depende la E/S.

Se comunican con los dispositivos de varias formas:
- **Polling**: Periodicamente checkea si el dispositivo se comunico. Es sencillo de implementar, pero desperdicia CPU.
- **Interrupciones**: El dispositivo le avisa al driver cuando se quiere comunicar con una interrupcion. Esto no desperdicia CPU como el **polling** pero si agrega un overhead por los cambios de contexto.
- **DMA (Direct Memory Access)**: Sirve para transferir grandes volumenes de informacion. Requiere de un controlador de DMA, el cual se encarga de esta transferencia, y al finalizar interrumpe la CPU.

### Subsistema de E/S
Es el subsistema propio de E/S del SO del cual los drivers forman parte. Se ocupa de proveerle al programador una API sencialla para comunicarse con los dispositivos, como `open()` o `read()`, mientras que los drivers traducen estas operaciones a comandos especificos entendidos por el HW.

Separa los dispositivos en dos **tipos**:
- **Char Device**: Se transmite informacion byte a byte, como un teclado.
- **Block Device**: Se transmite informacion en bloque, como un disco rigido.

Se comunica con los mismos para leer, escribir, o ambas a la vez, e invisibilisa sus particularidades para el usuario, como su **tipo**, su **acceso** (secuencial o aleatorio), si son **a/sincronicos**, o su **velocidad de respuesta**.

### Disco
De los dispositivos mas importantes de E/S esta el `disco duro`. Para mejorar la performance de la E/S al `disco`, buscamos minimizar los movimientos de la cabeza fisica del dispositivo. Es decir, el *seek time*. Esto puede hacerse manejando correctamente la cola de pedidos de E/S.

Como politicas de schedulong de E/S a disco, estan:
- **FIFO**: Es el esquema mas simple. De todos modos, no ayuda para minimizar los movimientos de la cabeza.
- **Shortest Seek Time First (SSTF)**: El proximo pedido a atender es el mas cercano a donde esta la cabez en el momento. Mientras que mejora los tiempos de respuesta, puede producir inanicion si constantemente llegan requests de la misma zona del disco.
- **Scan/Elevator**: Ir primero en un sentido, atendiendo los pedidos que se van encontrando, y luego en el otro. Mientras que evita la inanicion, podria llegar una solicitud para el cilindro inmediatamente anterior pero tendra que esperar a que cambie de direccion.
En la practica, se mezclan los algoritmos con politicas de prioridades, etc.

Por otro lado, en el aspecto de gestionar el disco, tenemos:
- **Formateo**: Se ponen unos codigos como prefijo y postfijo a cada sector del disco, los cuales de no tener el valor esperado indican que el sector esta dañado.
- **Booteo**: Las computadores suelen tener un programa chico en `ROM` que carga algunos sectores del disco a memoria para comenzar a ejecutar.
- **Bloques dañados**: A veces se manejan por software, en conjunto con el `sisteme de archivos`. Hay otros discos que vienen con sectores extra para reemplazar a los defectuosos.

### Spooling
Forma de manejar a los dispositivos en un contexto de multiprogramacion, como una impresora. La idea es poner pedidos en una cola, y designar un proceso que los vaya desencolando mientras se hacen.

### Seguridad de los datos
Para proteger los datos del disco, se pueden realizar copias de seguridad, que implican resguardar todo lo imporante en otro lado. Suele hacerse en cintas. El mayor problema que tienen es que toman mucho tiempo, ya que es copiar todo dos veces. Una estrategia para hacer backups es hacer backups totales cada cierto tiempo, y todos los dias solo resguardar los cambios realizados desde el ultimo backup parcial.

Por otro lado, una alternativa a las copias de seguridad es la redundancia. Un metodo muy comun es `RAID`, *Redundant Array of Inexpensive Disks*. La idea es tener dos discos al mismo tiempo, y realizar cada escritura en ambos. Esto es costoso pero conveniente. Ademas, permite realizar dos lecturas en simultaneo. Hay varios niveles de `RAID` con varias diferencias implementativas y sistemas de proteccion.

# SISTEMAS DE ARCHIVOS 

Un archivo es una secuencia de bytes, sin estructura, identificados con un nombre. Existe un modulo dentro del SO que se encarga de organizar estos archivos en disco: el `sistema de archivos`. Algunos SO soportan solo un tipo de `file system` y otros varios. Para el mismo, un archivo es una lista de bloques + metadata.

Una de las principales tareas del mismo es estructurar la organizacion de los archivos, tanto internamente (la informacion dentro del file) como externamente (cómo se ordenan los archivos). Ademas, suelen soportar una nocion de link, es decir un alias para que un archivo tenga mas de un nombre, y definen que nombres posibles pueden tener.

Otra funcion del `File System` es ocuparse del correcto manejo del *montaje* de una unidad de almacenamiento externo. Es decir, de colocar una nueva, reconocer que esta colocada, y desde donde se pueden leer los archivos y su formato.

La **performance** de un `sistema de archivos` esta determinada por muchos factores, como la tecnologia del disco, las politicas de E/S, los tamaños del bloque, cache, etc.

### Contiguo
Se colocan los bloques de cada archivo contiguos en el disco. De este modo, las lecturas son extremadamente rapidas, pero de querer quitar el ultimo bloque del archivo podriamos generar fragmentacion externa, o tener que realocar los demas archivos.

### Lista enlazada de bloques
Esto soluciona los problemas de fragmentacion y realocacion, pero de querer hacer lecturas de bloques aleatorios se debe de recorrer linealmente la lista.

### FAT
Se tiene una tabla donde por cada bloque hay una entrada donde me dice cual es la ubicacion del proximo. Es decir, se mapea todo el disco en una tabla, donde cada indice es el numero de bloque, y en ese indice se encuentra el numero del proximo bloque del arhcivo. El mayor problema de FAT es que la tabla puede llegar a ser grande, y se requiere que este en memoria para funcionar. Ademas, como hay una sola tabla hay mucha contencion, y de caerse el sistema la tabla modificada se pierde (estaba en memoria).

### Inodos
Cada archivo tiene uno. Tiene en el mismo los atributos del archivo, como el size, permisos, etc, y luego tiene algunas entradas con las direcciones de los primeros k bloques del archivo. De tener mas, se cuenta con algunas entradas para indirecciones simples, dobles y hasta triples. Estas apuntan a bloques que contienen solamente direcciones a mas bloques del archivo.

El sistema de inodos permite tener en memoria solo los inodos que necesitamos, y no la tabla entera de la FAT.

A su vez, cada directorio tiene su propio inodo, donde sus bloques de datos contienen `DirEntry` con la informacion de los inodos de los archivos/directorios que contiene. Se reserva un inodo distinguido para el *root directory*.

Como links, se pueden implementar tanto `hard` como `soft` links. Los `hard` son un enlace directo a un archivo existente en el sistema de archivos, y todos los liks del mismo archivo tienen el mismo numero de inodo (solo son entradas diferentes en directorios con nombres distintos). Se borra el archivo cuando se eliminan todos los `hard` links que apuntan a el.

Los `soft` links contienen una referencia a otro archivo o directorio en el sistema de archivos. Pueden apuntar a elementos de otros sistemas de archivos. De eliminarse no se afecta el archivo/directorio al que apuntan.

En `ext2`, en particular, se divide el disco en grupos de bloques, donde cada uno contiene un superbloque, con todos los datos del `file system` (como la ubicacion de los datos), y entre varias cosas la tabla de inodos, con punteros a los inodos correspondientes dentro de los bloques de datos del grupo.

### Consistencia
De cortarse la corriente o colgarse la PC, los datos que todavia no se pasaron a memoria secundaria se pierden. Para solucionar esto, existe la syscal `fsync()` que fuerza al SO a grabar las cosas si o si. Sin embargo, de caerse el sistema durante el grabado, se provee ademas un programa que restaura la consistencia del FS.

La idea es agregarle al FS un bit que indique apagado normal, y si cuando la PC levanta el bit no esta prendido, algo sucedio y se corre este programa.

### Journaling
Se lleva un registro con los cambios que habria que hacer. Cuando se baja el cache a disco, se actualiza una marca que indica que cambios ya se reflejaron. Cuando se llena el buffer se baja el cache a disco. De caerse el sistema, cuando levanta la PC el SO puede determinar que operaciones quedaron pendientes antes del fallo. A su vez, si se cae el sistema durante una operacion, el sistema puede hacer un `roll-back` y evitar estados de inconsistencia.

### Network File System (NFS)
Es un protocolo que permite acceder a FS remotos como si fueran locales. La idea es montar el FS remoto a algun punto de un sistema local, y luego se pueden acceder a los archivos de alli sin saber que son remotos. Nacen los *vnodes*, y *virtual file systems*, para que estos archivos de ser abiertos se almacenen con otra informacion.

# SISTEMAS DISTRIBUIDOS - 1

Un sistema distribuido es un conjunto de recursos conectados que interactuan. Por ejemplo, varias maquinas conectadas en red, un CPU con varias memorias, etc.

Como fortalezas, permite la descentralizacion del procesamiento y de los datos, y paralelismo. De todos modos, la sincronizacion y la coherencia se dificulta.

Para memoria compartida, se puede implementar con:
- **Hardware**: Tanto con **UMA**, donde todos los procesadores se conectan a una memoria por igual, como **NUMA**, donde la memoria esta repartida en diferentes nodos, y cada procesador tiene acceso mas rapido a la memoria localizada en el suyo.
- **Software**.

Las arquitecturas de **cliente/servidor** involucran un sistema que da servicios cuando un sistema cliente se lo pide. Como implementaciones, tenemos:
- **Telnet**: Protocolo y programa que permite conexiones remotas, pero por ello tiene como significado las `conexiones remotas`. La idea es utilizar los recursos y el poder de procesamiento de otro equipo como si estuviesemos utilizandolo.
- **RPC**: Mecanismo que le permite hacer a los programas (en C) hacer *procedure calls* en forma *remota*. Involucra una serie de bibliotecas que invisibilisan los detalles de la comunicacion y la trasmision de datos. Es sincronico.

### Pasaje de Mensajes
Es el mecanismo mas comun para comunicacion asincronica.
Tiene algunos problemas, como:
- Comunicacion lenta.
- Se pueden perder mensajes.
- Hay que manejar la de/codificacion de los datos.
- Podria darse que se caigan nodos o se parta la red y quede incomunicada alguna parte.

Esta el `Teorema CAP`: En un entorno distribuido, solo se puede tener dos de tres, consistencia, disponibilidad y tolerancia a fallas.

### Sincronizacion
Para sincronizar las diferentes partes de un sistema distribuido, podemos hacer varias soluciones.

Por un lado, podemos implementar un nodo lider, que coordine la ejecucion de un servicio o tarea. Este lider seria un coordinador de recursos. Este enfoque centralizado tiene el problema de que, de caer el nodo coordinador, se cae todo el sistema, y en que se genera un cuello de botella.

Observemos que un problema grande en este tipo de sistemas es saber que paso antes y que despues, como la toma de un mutex. Sincronizar relojes seria muy caro. Entonces, se puede simplemente intentar ordenar parcialmente los eventos. Para ello, **Lamport** define un *orden parcial no reflexivo*, que dentro de un mismo proceso, se tiene que:
- Si A es antes que B, A -> B.
- Si E y R es el envio/recepcion de un mensaje, E -> R.
- Si A -> B y B -> C, A -> C.
- Si ni A -> B ni B -> A, entonces son *concurrentes*. 
Se implementa haciendo que cada procesador tenga un *reloj*, y cada mensaje lleva adosado una lectura de reloj.

Esta el problema del **Acuerdo Bizantino**, donde necesitamos coordinar ejercitos para la hora de un ataque con mensajeros que pueden desaparecer. ¿Como hacemos que todos se coordinen? De aca sale un `Teorema` que dice que si fallan a lo sumo k < n procesos, entonces se puede resolver consenso si n > 3k y la conectividad (min. |nodos| tq G-N no es conexo) es mayor que 2k.

### Clusters
Es un conjunto de computadoras que trabajan en conjunto. Puede ser para proveer servicios, y pueden tener un scheduler de trabajos comun. Las *grids* son conjuntos de clusters, y las *clouds* son clustes alquilables por capacidad o bajo demanda.

### Scheduling
Si requerimos de hacer un scheduling comun para un sistema distribuido, entonces tenemos el nivel `local` de cada procesador y otro `global`. El global debe de encargarse de hacer el **mapping**, es decir distribuir las tareas entre procesadores y compartir la carga. 

Existe la **migracion**, donde un procesador sobrecargado envia sus tareas a otro, o donde un procesador libre pide mas tareas.

# SISTEMAS DISTRIBUIDOS - 2

Segun si los procesos o nodos fallan, fallan y se reincorporan, etc, cambian los algoritmos utilizados en sistemas distribuidos.

Para medir la eficiencia de uno de estos algoritmos, se pueden utilizar la cantidad de mensajes enviados, o el cuello de botella que tenga el sistema (como los tipos de fallas que soportan o cantidad de procesos).

### Exclusion mutua distribuida
- **Token Passing**: Armar un anillo logico entre procesos y circular un token. Solo puedo entrar a la zona critica si tengo el token. El problema es que el token circula cuando nadie quiere entrar tambien.
- Envio a todos los procesos una solicitud con un timestamp. Solo entro a la seccion critica cuando todos me responden. Los demas responden inmediatamente si no quieren entrar a la seccion critica, o si quieren entrar pero su timestamp es menor que el recibido.
- **Protocolo de mayoria**: De querer obtener el `lock` de un objeto distribuido en n nodos, le pedimos permiso a mas de la mitad de los nodos. Luego, tomamos la version mas actualizada del archivo, la modificamos en los lugares donde nos dieron el lock, e incrementamos el numero de version. No puede darse que hayan versiones desactualizadas ni dos nodos con el lock al mismo tiempo, porque en ese caso se deberia de poder tener mas de la mitad de los locks en dos lugares a la vez, lo cual es imposible.

### Eleccion de Lider
Se quiere elegir a un proceso como lider para una tarea.

Si no hay fallas, podemos organizar los procesos en un anillo y que cada uno circule su ID a sus vecinos. Luego, por cada ID que llega se devuelve la mayor comparando con la propia. Cuando a un proceso le llega su ID significa que es *lider* y le avisa a todos los demas. En peor caso son `O(n^2)` mensajes.

### 2PC - Two Phase Commit
Se utiliza para realizar operaciones en conjunto y que no falte ninguna parte. Se tiene un proceso coordinador, que en la primera fase le pregunta a los demas si estan listos. Si alguno le responde que no o no responde, entonces les dice que aborten. Si todos dicen que si, entonces les dice que operen y buscamos recibir los `ok` de operacion commiteada. Si algun proceso no envia el `ok` o devuelve un `error`, entonces existen para algunos sistemas mecanismos de *roll-back*. Su principal problema es que si se cae el lider se cae todo.

# SISTEMAS DISTRIBUIDOS - 3
**Distributed File Systems** son tales que sus clientes, servidores y dispositivos de almacenamiento estan en maquinas de un sistema distribuido. Se le presenta a los clientes como un `File System` normal. Existen de dos *modelos*, `Cliente-Servidor` y `Basado en Cluster`.

El primero se basa en un servidor que almacena todos los archivos y metadata en almacenamiento conectado al mismo. Cuando un cliente le pide archivos, se encarga de la autenticacion y obtencion de los mismos. El principal problema es la centralizacion de las operaciones en el servidor, que funciona como cuello de botella. De caerse cae todo el sistema.

El segundo apunta a ser mas resistente a fallas. Se tiene un servidor de metadata y archivos distribuidos de a pedazos en servidores de datos. Luego, el cliente al pedir un archivo, el primer servidor se fija donde se encuentran los pedacitos y los pide (tiene un mapeo de los mismos). Los chunks de cada archivo se replican n veces.

Estos **Sistemas de Archivos Distribuidos** son transparentes si ocultan la ubicacion en la que se almacena un archivo en la red. 

### Archivos
- *Transparencia de Ubicacion*: El nombre del archivo no revela la ubicacion fisica del mismo.
- *Independencia de Ubicacion*: El nombre del archivo no cambia cuando la ubiccacion fisica del mismo lo hace.

Los **Esquemas de Nombres** de los archivos en un **DFS** pueden ser:
- Una mezcla del `hostname` con el nombre local.
- Montar directorios remotos en directorios locales.
- Tener una unica estructura global de nombres, que abarca a todos los archivos.

Luego de encontrar el archivo pedido por un cliente, se puede utilizar un *mecanismo de servicio remoto*, donde el que accede al archivo es el servidor y este le transfiere la informacion al usuario. 

Tambien se pueden intentar reducir el trafico de red guardando bloques de disco accedidos recientemente en cache (lo cual puede traer problemas de consistencia). Este cache puede ubicarse en disco, lo cual es mas confiable, o puede estar en memoria, lo cual permite que no se tengan discos en las estaciones de trabajo y el acceso sea mas rapido.

Como politicas de actualizacion del mismo, esta el **Write-Through** o el **Delayed-Write o Write-Back**. La primera es mas confiable pero lenta, mientras que la segunda tiene escrituras rapidas pero se pierden datos si la maquina del usuario se cuelga.

### Consistencia
Es mi copia local en cache consistente con la copia maestra?
- Si el usuario quiere saber, entonces inicia un checkeo de validez y el servidor checkea si lo es.
- Si el servidor lo quiere saber, tiene registrado para cada cliente los archivos cacheados. Luego, reacciona si detecta alguna inconsistencia.

# VIRTUALIZACION 

**Virtualizacion** es la posibilidad de que un conjunto de recursos fisicos de vean como varias copias de recursos logicos. Es decir, usualmente es una computadora realizando el trabajo de varias (maquinas virtuales).

Tiene varias ventajas utilizar maquinas virtuales, como portabilidad, simulacion, aislamiento, proteccion, etc.
La idea es, en vez de tener un solo kernel de un SO, tener un `Virtual Machine Manager` que soporta varios kernels de distintos SO, los cuales pueden utilizar el HW a traves del mismo.

Se puede **simular**, donde se tiene el estado de la maquina virtual y por cada instruccion, en vez de realmente ejecutarse, se interpreta y se altera el estado. Esto es muy ineficiente, de todos modos, y tiene su complejidad simular interrupciones, concurrencia, etc.

Se puede por otro lado **emular** HW. Es decir, que las operaciones efectivamente se hagan en CPU, pero que todos los componentes se emulen y los pedidos pasen primero por el controlador de maquinas virtuales. Por ejemplo la E/S. De todos modos, las MV corren en modo usuario y se dificulta emular correctamente el SO.

Queremos evitar problemas con:
- Privilegios.
- Memoria virtual y encapsulamiento.
- Interrupciones.

Se agrego soporte a *virtualizacion* en HW para solucionar. En `Intel`, por ejemplo, se tienen modos del procesador VMX root y non-root, para distinguir instrucciones del huesped y del anfitrion. Tambien se agrega la *Virtual Machine Control Structure*, que controla el estado del huesped/anfitrion e indican las interrupciones o E/S de cada huesped. Uno de los problemas de esto es que, de caer una pieza de HW, pueden caer varias MV.

### Contenedores
A diferencia de tener un anfitrion que administra varias MV con sus SO y Kernels completos, los contenedores comparten el mismo kernel del SO anfitrion, lo que los hace mas ligeros y eficientes en terminos de recursos.

Entonces, los contenedores permiten crear y ejecutar aplicaciones en forma aislada y portatil. Docker es una de las mas conocidas.

# SEGURIDAD

La **seguridad de la informacion** se basa en la preservacion de la `Confidencialidad`, `Integridad` y `Disponibilidad`. Es decir, que la informacion debe de no ser modificada ni vista indebidamente, y que debe de estar disponible cuando se supone que deberia pasar. La **seguridad informatica** es la seguridad de la informacion en sistemas computacionales.

Estos *sistemas de seguridad* suelen tener tres roles:
- Sujetos.
- Objetos.
- Acciones.
Las politicas de seguridad definen que sujetos pueden realizar que acciones sobre que objetos.

Se utilizan como principios para gestionar el acceso de los usuarios a los objetos:
- **Autenticacion**: ¿Sos quien decis ser? Contraseñas, etc.
- **Autorizacion**: ¿Podes hacer lo que queres hacer?
- **Auditoria**: Dejo registrado lo que hiciste.

### Criptografia
Rama de la matematica/informatica que se ocupa del cifrado/descifrado de informacion. Utiliza metodos que permiten que mensajes solo sean leidos por la gente autorizada.

Existen algoritmos de encriptacion *simetricos*, que utilizan la misma clave para encriptar y desencriptar (`Caesar`, `DES`, etc), y *asimetricos* que no (`RSA`).

Las funciones de hash son tales que dados un string de input devuelven otro string modificado, tal que cumpla:
- *Resistencia a la preimagen*: Dado el hash, es dificil encontrar la clave.
- *Resistencia a la seguda preimagen*: Es dificil encontrar dos claves con el mismo hash.

El metodo **RSA** es uno de encriptado *asimetrico* que utiliza la dificultad computacional de calcular numeros primos como certificado de seguridad. La idea principal es que cada persona tenga dos claves, una privada y una publica.
- Para *encriptar* un mensaje, tomo la clave publica del receptor y la utilizo para encriptarlo antes de enviar.
- Para *desencriptar*, utilizo mi propia clave privada. 
Uno de los usos de **RSA** es la *firma digital*, la cual implica enviar un archivo + un hash del documento encriptado con mi clave privada, que luego se desencripta con mi publica. Si se desencripta exitosamente yo era el dueño, y ademas si coincide con el archivo no se modifico.

Se puede intentar bypassear algunos sitemas de encriptado con un ataque `replay-attack`, aprovechando que el encriptado se hace antes de enviar. Se lee ese hash y se reenvia, haciendome pasar por la persona.

### Autorizacion
El **Monitor de referencias** es el mecanismo responsable de mediar entre los sujetos y los objetos cuando intentan interactuar. Para ello, debe de conocer los permisos de estos sujetos por sobre estos objetos, y puede hacerlo con una matriz de `Sujetos` x `Objetos`, donde cada celda indica la acciones permitidas. Un principio muy comun es el de *minimo privilegio*, que dice que los sujetos deben de tener los minimos privilegios para llevar a cabo sus operaciones, y nada mas.

Hay dos esquemas para manejar los atributos de seguridad:
- **Discretionary Access Control (DAC)**: Los atributos se definen explicitamente, y el dueño decide los permisos. Es el utilizado por UNIX, con SETUID y SETGID.
- **Mandatory Access Control (MAC)**: Cada sujeto tiene un grado, y los objetos creados heredan el grado del ultimo sujeto que los modifico. Luego, un sujeto puede acceder solo a los objetos con grados menor o iguales a los de el. Utilizado para manejar informacion sensible.

Los **procesos** como sujetos en un principio heredan los permisos del usuario que los corre. Tambien pueden ser los permisos del propietario del programa (como SETUID bit en UNIX).

### Seguridad de Software
Hay que pensar en ella al momento de `pensar` la aplicacion, de `implementar` el codigo y al `operar` el programa. El lugar mas comun de ataque es el de diseño.

Ataques comunes son:
- **Buffer Overflow**: En C, de tener un buffer en la pila, el usuario puede sobrepasar el tamaño del buffer y pisar otros datos del stack, como la direccion de retorno. Tambien hay `heap based` buffer overflows.
- **Control de parametros**: Si no se controla el input del usuario podria ingresar comandos maliciosos o realizar buffer overflows. Se soluciona validandolos o utilizando el minimo privilegio posible.
- **SQL Injections**: Ataque por mal control de parametros en bases de datos.

### Malware
El `malware`, o `Malicious Software`, esta diseñado para llevar a cabo acciones indeseados y sin el consentimiento del usuario. Hay de varios tipos, como `keylogers`, `virus`, `troyanos`, etc.

Los principales metodos de infeccion son descargas desde paginas webs, adjuntos por mail, compartir dispositivos de almacenamiento, etc.

Para prevenir infecciones, hay que contar con un antivirus, SO, navegador, etc, actualizados, y sentido comun y uso responsable de la maquina.

### Rol de la confianza
Dentro de todo esto que abarca la `Seguridad Informatica`, la confianza abarca un rol fundamental. Y esto es porque todas las soluciones que planteamos para prevenir ataques o violacion a los principios de la seguridad de la informacion pueden fallar, pueden no haber sido programadas para nuestro ambiente, puede que no se instalen correctamente en nuestra maquina, etc. De ser incorrecta cualquier politica o procedimiento de seguridad se destruye todo lo demas.
