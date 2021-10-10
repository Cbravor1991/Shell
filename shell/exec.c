#include "exec.h"

// sets "key" with the key part of "arg"
// and null-terminates it
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  key = "KEY"
//
static void
get_environ_key(char *arg, char *key)
{
	int i;
	for (i = 0; arg[i] != '='; i++)
		key[i] = arg[i];

	key[i] = END_STRING;
}

// sets "value" with the value part of "arg"
// and null-terminates it
// "idx" should be the index in "arg" where "=" char
// resides
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  value = "value"
//
static void
get_environ_value(char *arg, char *value, int idx)
{
	size_t i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
		value[j] = arg[i];

	value[j] = END_STRING;
}

// sets the environment variables received
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here //te aisla la llave del valor
static void
set_environ_vars(char **eargv, int eargc)
{
	// Your code here
	char value[4096];
	char key[265];
	int i;
	for (int j = 0; j < eargc; j++) {
		char *arg = eargv[j];
		if ((i = block_contains(arg, '=')) > 0) {
			get_environ_key(arg, key);
			get_environ_value(arg, value, i);
			if (setenv(key, value, 1) < 0) {
				perror("setenv");
				_exit(-1);
			}
		}
	}
}

// opens the file in which the stdin/stdout/stderr
// flow will be redirected, and returns
// the file descriptor
//
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
static int
open_redir_fd(char *file, int flags, int file_descriptor)
{
	int fd, dupfd;

	if (strcmp(file, "&1") == 0) {
		fd = 1;
	} else {
		if ((fd = open(file, flags, 0644)) < 0) {
			perror("open");
			_exit(-1);
		}
	}
	if ((dupfd = dup2(fd, file_descriptor)) < 0) {
		perror("dup2");
		_exit(-1);
	}
}

// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
// - casting could be a good option


void
exec_cmd(struct cmd *cmd)
{
	// To be used in the different cases
	struct execcmd *e;
	struct backcmd *b;
	struct execcmd *r;
	struct pipecmd *p;

	switch (cmd->type) {
	case EXEC:
		e = (struct execcmd *) cmd;
		set_environ_vars(e->eargv, e->eargc);
		execvp(e->argv[0], e->argv);
		perror("execvp exec");
		_exit(-1);

		break;

	case BACK: {
		int n;
		b = (struct backcmd *) cmd;
		e = (struct execcmd *) b->c;
		int i = fork();


		if (i > 0) {
			waitpid(i, &n, WNOHANG);

			_exit(0);
		}
		if (i < 0) {
			perror("fork");
			_exit(-1);
		}

		if (i == 0) {
			exec_cmd(b->c);
		}


		break;
	}

	case REDIR: {
		// changes the input/output/stderr flow
		//
		// To check if a redirection has to be performed
		// verify if file name's length (in the execcmd struct)
		// is greater than zero
		//
		r = (struct execcmd *) cmd;
		int out_redir_fd, in_redir_fd, err_redir_fd;
		if (strlen(r->out_file) > 0) {
			char *archivo_salida = r->out_file;
			if ((out_redir_fd = open_redir_fd(archivo_salida,
			                                  O_CLOEXEC | O_CREAT |
			                                          O_RDWR | O_TRUNC,
			                                  1)) < 0) {
				perror("open");
				_exit(-1);
			}
		}


		if (strlen(r->in_file) > 0) {
			char *archivo_entrada = r->in_file;
			if (in_redir_fd = open_redir_fd(archivo_entrada,
			                                O_CLOEXEC | O_RDONLY,
			                                0) < 0) {
				perror("open");
				_exit(-1);
			}

			printf("Archivo entrada");
		}


		if (strlen(r->err_file) > 0) {
			char *archivo_error = r->err_file;
			if (err_redir_fd = open_redir_fd(archivo_error,
			                                 O_CLOEXEC | O_CREAT |
			                                         O_RDWR | O_TRUNC,
			                                 2) < 0) {
				perror("open");
				_exit(-1);
			}
			printf("Archivo salida error");
		}


		execvp(r->argv[0], r->argv);
		perror("orden no encontrada");
		_exit(-1);
		break;
	}

	case PIPE: {
		// pipes two commands
		//

		p = (struct pipecmd *) cmd;


		// lo que voy a necesitar para los pipes
		int fds[2];
		int ejecucion_pipe = pipe(fds);


		if ((ejecucion_pipe) < 0) {
			perror("pipe");
			_exit(-1);
		}

		int pid_izq = fork();


		if (pid_izq < 0) {
			perror("fork");
			_exit(-1);
		}


		if (pid_izq == 0) {
			if (close(fds[0]) < 0) {
				perror("close");
				_exit(-1);
			}
			if (dup2(fds[1], 1) < 0) {
				perror("dup");
				_exit(-1);
			}
			if (close(fds[1]) < 0) {
				perror("close");
				_exit(-1);
			}
			exec_cmd(p->leftcmd);
		}


		int pid_der = fork();

		if (pid_der < 0) {
			perror("fork");
			_exit(-1);
		}

		if (pid_der == 0) {
			if (close(fds[1]) < 0) {
				perror("close");
				_exit(-1);
			}


			if (dup2(fds[0], 0) < 0) {
				perror("dup");
				_exit(-1);
			}
			if ((close(fds[0]) < 0)) {
				perror("close_1");
				_exit(-1);
			}
			exec_cmd(p->rightcmd);
		}


		if ((close(fds[1]) < 0)) {
			perror("close_2");
			_exit(-1);
		}

		if ((close(fds[0]) < 0)) {
			perror("close_2");
			_exit(-1);
		}


		waitpid(pid_izq, NULL, 0);
		waitpid(pid_der, NULL, 0);

		_exit(0);


		// free the memory allocated
		// for the pipe tree structure

		free_command(parsed_pipe);


		break;
	}
	}
}
