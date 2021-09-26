#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <dirent.h>
#include <string.h>
void buscar_directorios(DIR *dir, char *ruta, char *busqueda, bool mayuscula);

void
buscar_directorios(DIR *dir, char *ruta, char *busqueda, bool mayuscula)
{
	if (dir != NULL) {
		int subdirectorio;
		struct dirent *entity;
		DIR *subdir;
		entity = readdir(dir);
		int id = dirfd(dir);


		if (mayuscula == true) {
			while (entity != NULL) {
				if (strcasestr(entity->d_name, busqueda)) {
					printf("%s%s\n", ruta, entity->d_name);
				}

				if (entity->d_type == DT_DIR &&
				    strcmp(entity->d_name, ".") != 0 &&
				    strcmp(entity->d_name, "..") != 0)

				{
					char *ruta_alternativa[200] = {};
					strcpy(ruta_alternativa, ruta);
					strcat(ruta_alternativa, entity->d_name);
					strcat(ruta_alternativa, "/");
					int subdirectorio = openat(
					        id, entity->d_name, O_DIRECTORY);
					if (subdirectorio < 0) {
						perror('openat');
						_exit(-1);
					}

					subdir = fdopendir(subdirectorio);  // ESTO ME DA UN DIR
					buscar_directorios(subdir,
					                   ruta_alternativa,
					                   busqueda,
					                   mayuscula);
					closedir(subdir);
				}
				entity = readdir(dir);
			}
		} else {
			while (entity != NULL) {
				if (strstr(entity->d_name, busqueda)) {
					printf("%s%s\n", ruta, entity->d_name);
				}

				if (entity->d_type == DT_DIR &&
				    strcmp(entity->d_name, ".") != 0 &&
				    strcmp(entity->d_name, "..") != 0)

				{
					char *ruta_alternativa[200] = {};
					strcpy(ruta_alternativa, ruta);
					strcat(ruta_alternativa, entity->d_name);
					strcat(ruta_alternativa, "/");
					subdirectorio = openat(id,
					                       entity->d_name,
					                       O_DIRECTORY);

					subdir = fdopendir(subdirectorio);
					buscar_directorios(subdir,
					                   ruta_alternativa,
					                   busqueda,
					                   mayuscula);
					closedir(subdir);
				}
				entity = readdir(dir);
			}
		}
	}
}

int
main(int argc, char *argv[])
{
	if ((argc != 2)) {
		char *busqueda = argv[2];
		bool mayuscula = true;
		DIR *dir = opendir(".");
		char ruta[200] = {};
		buscar_directorios(dir, ruta, busqueda, mayuscula);
		closedir(dir);

	} else {
		char *busqueda = argv[1];
		bool mayuscula = false;
		DIR *dir = opendir(".");
		char ruta[200] = {};
		buscar_directorios(dir, ruta, busqueda, mayuscula);
		closedir(dir);
	}


	return 0;
}
