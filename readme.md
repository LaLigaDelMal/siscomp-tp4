BORRAR (DESPUES): RESPUESTAS [ACA](https://tldp.org/LDP/lkmpg/2.4/lkmpg.pdf)

## Desafío 1 

Revisar la [bibliografia](https://access.redhat.com/documentation/es-es/red_hat_enterprise_linux/8/html/managing_monitoring_and_updating_the_kernel/signing-kernel-modules-for-secure-boot_managing-kernel-modules) para impulsar acciones que permitan mejorar la seguridad del kernel, concretamente: evitando cargar módulos que no estén firmados.


## Desafío 2

Debe tener respuestas precisas a las siguientes pregutas y sentencias:
- ¿ Qué funciones tiene disponible un programa y un módulo ?
**Un módulo comienza con la ejecución de una función module_init(), le dice al kernel qué funcionalidad ofrece y setea el kernel a la espera de que lo utilice cuando el kernel lo necesite. Los módulos deben tener una función de entrada y otra de salida (module_exit())**

**¿Qué funciones tiene disponible un programa? Primero construir un programa que llame a printf(), compilarlo (con opción -Wall y ejecutar strace (strace -tt y strace -c). Explicar lo que se observa.!!**

**¿Qué funciones tiene disponible un módulo? La definición de los símbolos proviene del propio kernel; las únicas funciones externas que puede utilizar un módulo, son las proporcionadas por el kernel. Investigar el contenido del archivo /proc/kallsyms .!!**
- Espacio de usuario vs espacio del kernel.

**¿ Qué funciones tiene disponible un módulo ? La definición de los símbolos proviene del propio kernel; las únicas funciones externas que puede utilizar un módulo, son las proporcionadas por el kernel. Investigar el contenido del archivo /proc/kallsyms .!!**

- Espacio de datos.

**Un programita en C, utiliza nombres de variables convenientes y con sentido para el lector. Ahora, si escribe rutinas que serán parte de un gran programa, cualquier variable global que tenga es parte de una comunidad de variables globales de otras personas y algunos de los nombres pueden entrar en conflicto. Cuando existen muchas variables globales que no son lo suficientemente significativas para ser distinguidas, se contamina el espacio de nombres. En grandes proyectos, se debe hacer un esfuerzo para recordar nombres reservados y encontrar formas de desarrollar un esquema para nombres y símbolos únicos de variables. El archivo /proc/kallsyms contiene todos los símbolos que el kernel conoce y que, por lo tanto, son accesibles para sus módulos, ya que comparten el espacio de código del kernel.**

- Drivers. Investigar contenido de /dev.
 
**Una clase particular de módulo es el “driver” o controlador de dispositivo, que proporciona funcionalidad para hardware específico. Como en los sistemas “unix” los dispositivos se mapean como archivos, los vamos a encontrar asociados con alguna entrada en /dev. Observaremos de forma general, qué tipos existen y prestaremos atención a los números mayor y menor.**

## Desafio 3

- ¿Qué es exactamente un módulo del núcleo? 

Los módulos son fragmentos de código que se pueden cargar y descargar en el kernel según se requiera. Extienden la funcionalidad del kernel sin necesidad de reiniciar el sistema. Por ejemplo, un tipo de módulo es el controlador de dispositivo, que permite que el núcleo acceda al hardware conectado al sistema. Sin módulos, tendríamos que construir kernels monolíticos y agregar nuevas funciones directamente en la imagen del kernel. Además de tener kernels más grandes, esto tiene la desventaja de requerir que reconstruyamos y reiniciemos el kernel cada vez que querramos una nueva funcionalidad.

## Desafio 4

### Preparación

Ubuntu:

`apt-get install build-essentials kernel-package`

`apt-get install linux-source`

Fedora:

`sudo dnf install kernel-devel kernel-headers`

### Comandos útiles

`sudo insmod <modulo>.ko`

`sudo lsmod | grep mimodulo`

`dmesg | tail`

`sudo rmmod mimodulo`

`modinfo mimodulo.ko`

Docu Rasp