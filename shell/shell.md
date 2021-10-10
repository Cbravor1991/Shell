# Lab: shell

### Variable global status
En el presente Lab se instruía que funciones se debían codificar y los archivos donde se encontraban, aparte de escribir el codigo de las funciones indicadas, fue neceserio agregar la variable global status en el archivo def.h de esta manera la misma podra ser modificada desde las funciones que la necesiten en los distintos archivos, en el caso de la implementación realizada la misma fue usada en el archivo builtin.c.

### Búsqueda en $PATH
Pregunta: ¿cuáles son las diferencias entre la syscall execve(2) y la familia de wrappers proporcionados por la librería estándar de C (libc) exec(3)?

La familia de wrappers exec proporcionada por la libreria estándar de C (libc), cargan un nuevo programa en el proceso actual con diversos valores. Pero según que tipo de wrapper de la familia ejecute variara de donde se carga el programa los tipos de parametros que le paso y cuales son sus argumentos.

* Cuando el wrapper posee "v": En este caso tiene como parametro una matriz de punteros a cadenas.El primer argumento debe apuntar al nombre del programa asociado con el programa que se va a ejecutar y el ultimo argumento debe ser un puntero nulo.

* Cuando el wrapper posee "l": En este caso tiene como parametro una lista de argumentos de
    longitud variable compuesta por punteros a cadenas , cuyo ultimo elemento es un puntero
    nulo.El primer argumento debe apuntar al nombre del programa que se va a ejecutar.

* Cuando el wrapper posee "e": En este caso tiene como parametro un argumento adicional que
    proporciona una matriz de punteros a variables de entorno.En el caso contrario el programa hereda el
    entorno del proceso actual.

* Cuando el wrapper posee "p": En este caso significa que se va a buscar en el PATH el nombre del programa
    que se va a ejecutar.En caso contrario se debe asignar una ruta

A su vez la familia de wrapper utiliza la syscall (llamada al sistema) Exceve. 
Las llamadas al sistema, ya que se ejecuta en el kernel, tienen acceso a todo en el sistema, por lo que controlan lo que el proceso que la llama puede hacer por ejmplo, verificar los permisos que solicita el proceso que invoca a la syscalls, además dado que las llamadas al sistema están en el kernel, llamarlas requiere un cambio de contexto en la CPU, que es un proceso muy pesado en relación con solo llamar a una llamada de biblioteca.




En concreto la diferencia entre exceve y exec, es que los parametros del execve se encuentran dados por

    * Una matriz de punteros a cadenas -> v
    * Una matriz de punteros a variables de entorno -> e

    Adcionalmente recibe el nombre de ruta del programa.
    Execve ejecuta el programa al que se refiere el nombre de la ruta y esto permite que el entorno del proceso que se esta ejecutando actualmente sea reemplazado por el entorno del nuevo programa.

 Pregunta : ¿Puede la llamada a exec(3) fallar? ¿Cómo se comporta la implementación de la shell en ese caso?

  La llamada a exec puede fallar, en nuestra implementación tenemos conteplada esta situación y para atrapa este error y no generar algun tipo de inprevisto, consultamos si la ejecución del wrapper fue correcta y en caso contrario utilizamos un perror para identificar el error mostrarlo y luego salimos con un _exit(-1).






---

### Comandos built-in

Pregunta: ¿Entre cd y pwd, alguno de los dos se podría implementar sin necesidad de ser built-in? ¿Por qué? ¿Si la respuesta es sí, cuál es el motivo, entonces, de hacerlo como built-in? (para esta última pregunta pensar en los built-in como true y false)

En el caso de "pwd", se pordria impelementar como un programa ya que nosostros poseemos la syscall getcwd(3), la caul nos permite obtener la ruta actual donde en la que se encuentra la shell. 
Si nosostros lo implementamos como una "built-in" tenemos la posibilidad de agregarle funcionalidades adicionales.



---

### Variables de entorno adicionales
Pregunta: ¿Por qué es necesario hacerlo luego de la llamada a fork(2)?
Es necesario hacerlo luego de una llamada fork, debido a que las variebles temporales deber ser eliminadas luego de que finalize un determinado proceso. Si nosostros no utilizaramos el fork las variables no seran temporales, estas se mantendran hasta que la shell finalize.

Pregunta: En algunos de los wrappers de la familia de funciones de exec(3) (las que finalizan con la letra e), se les puede pasar un tercer argumento (o una lista de argumentos dependiendo del caso), con nuevas variables de entorno para la ejecución de ese proceso. Supongamos, entonces, que en vez de utilizar setenv(3) por cada una de las variables, se guardan en un array y se lo coloca en el tercer argumento de una de las funciones de exec(3).
¿El comportamiento resultante es el mismo que en el primer caso? Explicar qué sucede y por qué.
Describir brevemente (sin implementar) una posible implementación para que el comportamiento sea el mismo.

En nuestra shell implementada nosostros usamos execvp, este wrapper mantiene el entorno en un nuevo proceso que pueda llegar a ejecutar.
Si nosostros usamos alguno de los wrappers que finalice con la letra e nosotros deberemos pasarles por parametros el entorno y la lista de argumentos con las variables necesarias para que se mantenga el mismo comportamiento anterior.











---

### Procesos en segundo plano
Pregunta: Detallar cuál es el mecanismo utilizado para implementar procesos en segundo plano.
En nuestra implementación lo que hacemos es utilizar un fork, en el hijo llamamos en forma recursiva a la función exec_cmd(b->c). pasandole por parametro un struct cmd, que sería la interfaz del comando. Teniendo en cuenta que estamos implementando la funcionalidad de procesos en segundo plano, lo que esperamos de la misma es poder seguir ejecutando distintos procesos sin necesidad de esperar que el anteriormente ejecutado finalize. 
Para esto nosostrtos utilizamos en el proceso padre del fork la sycalls waitpid(i, &n, WNOHANG), ya que el utilizar WNOHANG el waitpid es no bloqueante y nos permite continuar con la utilización de la shell.

---

### Flujo estándar

Pregunta: Investigar el significado de 2>&1, explicar cómo funciona su forma general y mostrar qué sucede con la salida de cat out.txt en el ejemplo. Luego repetirlo invertiendo el orden de las redirecciones. ¿Cambió algo?

2>&1, es una forma de redireccionar el flujo estándar producido por errores en la ejecución de un programa, concretamente redirige el flujo estándar de error a la salida estandar.
Y en su forma general funciona de la siguiente manera [fd_src]>&[fd_dest] .

Su funcionamiento se puede observar a tráves del siguiente ejemplo:

    $ ls -C /home /noexiste >out.txt 2>&1

    $ cat out.txt
    ---????---

Resultado obtenido en bash:

    christian@christian-Smart-E24:~/Escritorio/shell_prueba/shell$ ls -C /home /noexiste >out.txt 2>&1
    christian@christian-Smart-E24:~/Escritorio/shell_prueba/shell$ cat out.txt
    ls: no se puede acceder a '/noexiste': No existe el archivo o el directorio
    /home:
    christian
     


Resultado obtenido en la shell implementada en el presente lab :

    (/home/christian) 
    $ ls -C /home /noexiste >out.txt 2>&1
        Program: [ls -C /home /noexiste >out.txt 2>&1] exited, status: 2 
    (/home/christian) 
    $ cat out.txt
    ls: no se puede acceder a '/noexiste': No existe el archivo o el directorio
    /home:
    christian
        Program: [cat out.txt] exited, status: 0 


En ambos ejemplos se puede ver que la salida que se encuentra en out.txt, sera una combinación de stdout y stderr.
Luego si se cambia el orden de redirección lo que sucede es que se redirige el error estandar a la salida estandar, y luego a la salida.

Cabe destacar que en este lab solo se contempla el caso del ejemplo y si invirtio el orden no cambiara la salida.

---

### Tuberías simples (pipes)

Pregunta: Investigar qué ocurre con el exit code reportado por la shell si se ejecuta un pipe ¿Cambia en algo? ¿Qué ocurre si, en un pipe, alguno de los comandos falla? Mostrar evidencia (e.g. salidas de terminal) de este comportamiento usando bash. Comparar con la implementación del este lab.

Al ejecutar un pipe utilizando bash, pipe ejecuta todos los comandos aunque estos fallen. Si hacemos la ejecución usando la shell implementada por nosostros sucede lo mismo.


El exit code reportado por bash al ejecutar pipe es el reportado por el úiltimo comando de la derecha

    Ejemplo: 

        christian@christian-Smart-E24:~/Escritorio/shell_prueba/shell$ cat falla | echo holMundo
        holMundo
        cat: falla: No existe el archivo o el directorio
        christian@christian-Smart-E24:~/Escritorio/shell_prueba/shell$ echo $? 
        0
        christian@christian-Smart-E24:~/Escritorio/shell_prueba/shell$ echo holaMundo | cat falla
        cat: falla: No existe el archivo o el directorio
        christian@christian-Smart-E24:~/Escritorio/shell_prueba/shell$ echo $?
        1

Por el contrario en el exit code reportado por nuestra shell no sucede lo mismo que en bash ya que no se produce un acarreo del exit code

    Ejemplo:
                Program: [clear] exited, status: 0 
            (/home/christian) 
            $ ls noeciste|echo hola
            hola
            ls: no se puede acceder a 'noeciste': No existe el archivo o el directorio
            (/home/christian) 
            $ echo $?
            0
                Program: [echo $?] exited, status: 0 
            (/home/christian) 
            $ echo hola | ls noexiste
            ls: no se puede acceder a 'noexiste': No existe el archivo o el directorio
            (/home/christian) 
            $ echo $?
            0

---

### Pseudo-variables

Pregunta: Investigar al menos otras tres variables mágicas estándar, y describir su propósito. Incluir un ejemplo de su uso en bash (u otra terminal similar).

Dentro de las variables de bash nos podemos encontrar con las siguientes:

*\$\$: Esta variable deberia darme el PID mi proceso actual. En este caso el proceso padre es el generado por mi bash. 
    
    Ejemplo: 
    christian@christian-Smart-E24:~/Escritorio/shell_prueba/shell$ echo $$
    96552

*\$\!: Esta variable deberia darme el PID del ultimo proceso ejecutado.
    
    Ejemplo
    christian@christian-Smart-E24:~/Escritorio/shell_prueba/shell$ sleep 5&
    [3] 96754
    christian@christian-Smart-E24:~/Escritorio/shell_prueba/shell$ echo $!
    96754
    [3]+  Hecho                   sleep 5
    christian@christian-Smart-E24:~/Escritorio/shell_prueba/shell$ 

*$@: Esta se establece al comienzo de la shell y contiene el nombre de archivo absoluto de la shell o script que se ejecuta como se pasó en la lista de argumentos. Posteriormente, se expande al último argumento del comando previo, después de la expansión. También se establece en el nombre de ruta completo de cada comando ejecutado y se coloca en el entorno exportado a ese comando. Al comprobar el correo, este parámetro contiene el nombre del archivo de correo. 

    Ejemplo: 
    christian@christian-Smart-E24:~/Escritorio$ echo hola
    hola
    christian@christian-Smart-E24:~/Escritorio$ echo $_
    hola







