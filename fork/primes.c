#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define N 10000

int
main(int argc, char *argv[])
{
	int p;
	int p_prev = 0;
	int n;
	int status;
	int pipe_derecho[2];
	int cerrado;
	bool terminado = true;
	int pipe_izquierdo[2];

	int ejecucion_pipe1 = pipe(pipe_derecho);
	if (ejecucion_pipe1 < 0) {
		perror("pipe");
		_exit(-1);
	}


	for (int n = 2; n < N + 1; n++) {
		ssize_t write_1 = write(pipe_derecho[1], &n, 4);
		if (write_1 < 0) {
			perror("write");
			_exit(-1);
		}
	}
	close(pipe_derecho[1]);

	while (terminado) {
		int i = fork();
		if (i < 0) {
			perror("fork");
			_exit(-1);
		}

		if (i > 0) {
			int cerrar_1 = close(pipe_derecho[0]);
			if (cerrar_1 < 0) {
				perror("close");
				_exit(-1);
			}

			wait(&status);

			_exit(0);
		}
		if (i == 0) {
			pipe_izquierdo[0] = dup(pipe_derecho[0]);
			ssize_t read_1 = read(pipe_izquierdo[0], &p, 4);
			if (read_1 < 0) {
				perror("read");
				_exit(-1);
			}


			int cerrar_2 = close(pipe_derecho[0]);

			if (cerrar_2 < 0) {
				perror("close");
				_exit(-1);
			}

			if (p != p_prev) {
				printf("primo %d\n", p);
				p_prev = p;
			} else {
				terminado = false;
			}
			int ejecucion_pipe2 = pipe(pipe_derecho);

			if (ejecucion_pipe2 < 0) {
				perror("pipe");
				_exit(-1);
			}
			do {
				cerrado = read(pipe_izquierdo[0], &n, 4);
				if (cerrado < 0) {
					perror("read");
					_exit(-1);
				}


				if (n % p != 0) {
					ssize_t write_2 =
					        write(pipe_derecho[1], &n, 4);
					if (write_2 < 0) {
						perror("write");
						_exit(-1);
					}
				}

			} while (cerrado > 0);
		}
		int cerrado_izquierdo_final = close(pipe_izquierdo[0]);

		if (cerrado_izquierdo_final < 0) {
			perror("close");
			_exit(-1);
		}


		int cerrado_derecho_final = close(pipe_derecho[1]);
		if (cerrado_derecho_final < 0) {
			perror("close");
			_exit(-1);
		}
	}
	return 0;
}