#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef NARGS
#define NARGS 4
#endif


int
main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Se necesita un argumento.\n");
		_exit(-1);
	}


	char *comando = NULL;
	char *linea[NARGS + 2] = {};

	int status;
	int cantidad_lineas = 1;

	comando = argv[1];


	if (comando == NULL) {
		printf("el comando no fue hallado\n");
		_exit(-1);
	}


	linea[0] = comando;

	size_t n = 0;
	while (cantidad_lineas < (NARGS + 1) &&
	       (getline(&linea[cantidad_lineas], &n, stdin)) > 0) {
		cantidad_lineas++;
	}

	if (cantidad_lineas > 1) {
		linea[cantidad_lineas] = NULL;

		for (int j = 1; j < cantidad_lineas; j++) {
			int longitud = strlen(linea[j]);
			if (linea[j][longitud - 1] == '\n') {
				linea[j][longitud - 1] = '\0';
			}
		}


		int tenedor = fork();
		if (tenedor > 0) {
			waitpid(tenedor, &status, 0);
		}

		if (tenedor < 0) {
			perror("fork");
			_exit(-1);
		}

		if (tenedor == 0) {
			int ejecucion_execvp = execvp(comando, linea);
			if (ejecucion_execvp < 1) {
				perror("execvp");
				_exit(-1);
			}
		}


		return 0;
	}
}