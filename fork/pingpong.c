#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


int
main(void)
{
	int valor = rand();
	int valor_recibido;
	int fd_1[2];
	int fd_2[2];
	pid_t pidC;

	int ejecucionPipe_1 = pipe(fd_1);
	if (ejecucionPipe_1 < 0) {
		perror("pipe");
		_exit(-1);
	}
	int ejecucionPipe_2 = pipe(fd_2);
	if (ejecucionPipe_2 < 0) {
		perror("pipe");
		_exit(-1);
	}

	printf("Hola, soy PID %d\n", getpid());
	printf("    - primer pipe me devuelve: [%d, %d]: \n", fd_1[0], fd_1[1]);
	printf("    - segundo pipe me devuelve: [%d, %d]: \n", fd_2[0], fd_2[1]);

	pidC = fork();

	/* Esta parta del codigo lo que hace es indicarme que se debe hacer segun sea el padre el hijo etc*/
	if (pidC > 0)  // se ejecuta el padre
	{
		int cerrado_1 = close(fd_1[0]);
		if (cerrado_1 < 0) {
			perror("close");
			_exit(-1);
		}
		ssize_t escritura_1 =
		        write(fd_1[1],
		              &valor,
		              sizeof(valor));  // se escribe en la posición uno
		if (escritura_1 < 0) {
			perror("write");
			_exit(-1);
		}
		printf("Donde fork me devuelve %d \n", pidC);
		printf("    -getpid me devuelve %d \n", getpid());
		printf("    -getpid me devuelve %d \n", getpid());
		printf("    -random me devuelve %d \n", valor);
		printf("    -envio valor %d a través de fd=%d \n", valor, fd_1[1]);


		ssize_t lectura_1 = read(fd_2[0], &valor, sizeof(valor));
		if (lectura_1 < 0) {
			perror("read");
			_exit(-1);
		}
		printf("Hola de nuevo PID %d \n", getpid());
		printf("    -recibi valor %d via fd=%d\n ", valor, fd_2[0]);
	}

	if (pidC == 0)  // es el valor que esta ejecutando el hijo
	{
		int cerrado_2 = close(fd_1[1]);
		if (cerrado_2 < 0) {
			perror("close");
			_exit(-1);
		}
		ssize_t lectura_2 = read(
		        fd_1[0],
		        &valor_recibido,
		        sizeof(valor_recibido));  // se escribe en la posición uno
		if (lectura_2 < 0) {
			perror("read");
			_exit(-1);
		}
		printf("Donde fork me devuelve %d \n", pidC);
		printf("    -getpid me devuelve %d \n", getpid());
		printf("    -getpid me devuelve %d \n", getpid());
		printf("    -recibo valor %d via fd=%d \n", valor, fd_1[0]);
		ssize_t escritura_2 = write(fd_2[1], &valor, sizeof(valor));
		if (escritura_2 < 0) {
			perror("write");
			_exit(-1);
		}
		printf("    -reenvio valor en fd= %d y termino \n", fd_2[1]);
		int cerrado_3 = close(fd_2[1]);
		if (cerrado_3 < 0) {
			perror("close");
			_exit(-1);
		}
	}

	return 0;
}