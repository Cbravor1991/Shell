#include "builtin.h"

// returns true if the 'exit' call
// should be performed
//
// (It must not be called from here)
int
exit_shell(char *cmd)
{
	if (strcmp(cmd, "exit") == 0) {
		return 1;
	}
	// Your code here

	return 0;
}

// returns true if "chdir" was performed
//  this means that if 'cmd' contains:
// 	1. $ cd directory (change to 'directory')
// 	2. $ cd (change to $HOME)
//  it has to be executed and then return true
//
//  Remember to update the 'prompt' with the
//  	new directory.
//
// Examples:
//  1. cmd = ['c','d', ' ', '/', 'b', 'i', 'n', '\0']
//  2. cmd = ['c','d', '\0']
int
cd(char *cmd)
{
	if (strncmp(cmd, "cd", 2) == 0) {
		if (strlen(cmd) == 2) {
			status = 0;


			if (chdir(getenv("HOME")) < 0) {
				status = 1;
				perror("chdir");
			}

			char *resultado;

			resultado = getcwd(NULL, 0);

			snprintf(promt, sizeof promt, "%s", resultado);


			return 1;
		}
		int chdir_1 = chdir(cmd + 3);
		status = 0;
		if (chdir_1 < 0) {
			status = 1;
			perror("chdir");
		}

		char *resultado;

		resultado = getcwd(NULL, 0);

		snprintf(promt, sizeof promt, "%s", resultado);
		return 1;
	}


	return 0;
}

// returns true if 'pwd' was invoked
// in the command line
//
// (It has to be executed here and then
// 	return true)
int
pwd(char *cmd)
{
	if (strcmp(cmd, "pwd") == 0) {
		char *resultado;
		// char buffer
		resultado = getcwd(NULL, 0);

		printf("%s\n", resultado);
		status = 0;
		free(resultado);
		return 1;
	}
	// Your code here

	return 0;
}
