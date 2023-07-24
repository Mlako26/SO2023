# INTRODUCCION

Los Sistemas Operativos son una pieza de software que hacen de intermediario entre el HW y los programas de usuario. Tienen dos funciones principales:
- Para el usuario, se encargan de que el software especifico no se tenga que preocupar con detalles de bajo nivel del HW.
- Para el dueño del HW, se encargan de que el usuario utilice correctamente el HW.
Deben de manejar la contención (acceso a mismos recursos por parte de varios programas) y la concurrencia con el mejor rendimiento posible. Para ello, corre con privilegio 0.

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



# COMUNICACION INTER-PROCESOS 

# SCHEDULING 

# SINCRONIZACION 

# GESTION DE MEMORIA 

# SISTEMA DE E/S 

# SISTEMAS DE ARCHIVOS 

# SISTEMAS DISTRIBUIDOS 

# VIRTUALIZACION 

# SEGURIDAD