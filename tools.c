






















#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "tools.h"

#define MAX_ARGS 32

char* config = NULL;
size_t config_length;

void* ueld_signal(int signum, void* handler, int restartsyscall)
{
	struct sigaction sa, osa;

	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	if (restartsyscall) {
		sa.sa_flags = SA_RESTART;
	} else {
#ifdef SA_INTERRUPT
		sa.sa_flags = SA_INTERRUPT;
#endif
	}
	if(sigaction(signum, &sa, &osa) < 0)
		return (void*) -1;
	return osa.sa_handler;
}

int ueld_unblock_signal(int signum)
{
	sigset_t set, oset;
	sigemptyset(&set);
	sigaddset(&set, signum);
	return sigprocmask(SIG_UNBLOCK, &set, &oset);
}

int ueld_block_signal(int signum)
{
	sigset_t set, oset;
	sigemptyset(&set);
	sigaddset(&set, signum);
	return sigprocmask(SIG_BLOCK, &set, &oset);
}

void ueld_echo(char* msg)
{
	printf("[ueld] %s\n", msg);
}

void ueld_print(char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	write(STDOUT_FILENO, "[ueld] ", 7);
	vprintf(fmt, ap);
	
	va_end(ap);
}

pid_t ueld_run(char* file, int flag, int vt, int* wait_status)
{
	pid_t pid;
	int status;
	struct sigaction ignore, save_intr, save_quit;

	if (file == NULL)
		return -1;

	if (ueld_block_signal(SIGCHLD) < 0)
		return -1;

	if (flag & URF_WAIT) {
		ignore.sa_handler = SIG_IGN;
		sigemptyset(&ignore.sa_mask);
		ignore.sa_flags = 0;

		if (sigaction(SIGINT, &ignore, &save_intr) < 0) {
			ueld_unblock_signal(SIGCHLD);
			return -1;
		}
		if (sigaction(SIGQUIT, &ignore, &save_quit) < 0) {
			sigaction(SIGINT, &save_intr, NULL);
			ueld_unblock_signal(SIGCHLD);
			return -1;
		}
	}

	if ((pid = fork()) == 0) {
		ueld_unblock_signal(SIGCHLD);

		if (flag & URF_SETSID) {
			if (setsid() == -1) {
				ueld_print("Could not run '%s': Setsid failed (%s)\n", file, strerror(errno));
				_exit(EXIT_FAILURE);
			}
		}

		if (flag & URF_NOOUTPUT) {
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);

			int fd0 = open("/dev/null", O_RDWR);
			int fd1 = dup(fd0);
			int fd2 = dup(fd0);

			if((fd0 != STDIN_FILENO) || (fd1 != STDOUT_FILENO) || (fd2 != STDERR_FILENO)){
				ueld_print("Could not run '%s': Unexcept file discriptor number.\n", file);
				_exit(EXIT_FAILURE);
			}
		}

		if (flag & URF_SETVT) {
			int fd0, fd1, fd2;
			char devname[256];

			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);

			setsid();

			snprintf(devname, sizeof(devname), "/dev/tty%d", vt);
			if((fd0 = open(devname, O_RDWR)) < 0) {
				ueld_print("Could not run '%s': Open vt '%s' error (%s)\n", file, devname, strerror(errno));
				_exit(EXIT_FAILURE);
			}

			fd1 = dup(fd0);
			fd2 = dup(fd0);

			if((fd0 != STDIN_FILENO) || (fd1 != STDOUT_FILENO) || (fd2 != STDERR_FILENO)){
				ueld_print("Could not run '%s': Unexcept file discriptor number.\n", file);
				_exit(EXIT_FAILURE);
			}

#ifdef BSD
			if (ioctl(fd0, TIOCSCTTY, (char*)vt) < 0) {
				ueld_print("Could not run '%s': Set control tty error (%s)\n", file, strerror(errno));
				_exit(EXIT_FAILURE);
			}
#endif
		}
		
		if (flag & URF_CMDLINE) {
			char* args[MAX_ARGS + 1];
			char* ptr;
			int argssz = 0;

			ptr = malloc(strlen(file) + 1);
			strcpy(ptr, file);
			while (*ptr != 0) {
				if (argssz > (MAX_ARGS - 1))
					break;
				args[argssz++] = ptr;
				ptr = strchr(ptr, ' ');

				if (!ptr || *ptr == 0) break;
				*ptr = 0;
				ptr++;
			}

			args[argssz++] = 0;
			execvp(args[0], args);
			free(ptr);
		} else {
			execl(file, file, 0);
		}
		ueld_print("Could not run '%s': execl failed(%s)\n", file, strerror(errno));
		_exit(EXIT_FAILURE);
	}
	
	/* parent */
	if (flag & URF_WAIT) {
		if (pid > 0) {
			while (waitpid(pid, &status, 0) != pid) {
				if (errno != EINTR) {
					status = -1;
					break;
				}
			}

			if (wait_status)
				*wait_status = status;
		}

		sigaction(SIGINT, &save_intr, NULL);
		sigaction(SIGQUIT, &save_quit, NULL);
	}

	ueld_unblock_signal(SIGCHLD);
	
	return pid;
}

static char* memstr(char* src, const char* needle, size_t size)
{
	int p = 0;
	int len = strlen(needle);

	for (size_t i = 0; i < size; i++) {
		if (src[i] == needle[p]) {
			if (p == len - 1) return &(src[i - p]);
			p++;
		} else {
			if (p) {
				i--;
				p = 0;
			}
		}
	}

	return 0;
}

char* ueld_readconfig(char* name)
{
	char *p, *value, *end;

	if (!config) {
		int fd;

		if ((fd = open("/etc/ueld/ueld.conf", O_RDONLY)) < 0)
			return NULL;
		config_length = lseek(fd, 0, SEEK_END) + 1;
		config = mmap(NULL, config_length, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);

		if (!config) {
			close(fd);
			return NULL;
		}

		config[config_length - 1] = '\n';
		close(fd);
	}

	p = memstr(config, name, config_length);
	if (!p) return NULL;
	end = strchrnul(p, '\n');
	*end = 0;
	value = strchr(p, '=');
	if (!value || *value == 0) return NULL;
	return ++value;

/*	p = strstr(config, name);
	if (!p || *p == 0) return NULL;

	end = strchrnul(p, '\n');
	*end = 0;

	value = strchr(p, '=');
	if (!value || *value == 0) {
		*end = '\n';
		return NULL;
	}
	value++;
*/
	return value;
}

long ueld_readconfiglong(char* name, long defaultval)
{
	long i = defaultval;

	char* value = ueld_readconfig(name);
	if (value) {
		i = atol(value);
		ueld_freeconfig(value);
	}
	
	return i;
}

void ueld_freeconfig(char* value)
{
	return;
/*	if (value)
		*strchrnul(value, '\n') = '\n';*/
}

void ueld_closeconfig()
{
	if (config) {
		if (munmap(config, config_length) == 0)
			config = NULL;
	}
}
