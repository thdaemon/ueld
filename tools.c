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
#include <wordexp.h>

#include "tools.h"
#include "names.h"
#include "log.h"

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

void ueld_print(char* fmt, ...)
{
	int fd;
	va_list ap;
	char buff[1024];

	va_start(ap, fmt);
	vsnprintf(buff, sizeof(buff), fmt, ap);

	/*
	 * See main.c, for Linux Sysrq SAK problem
	 * And it cause a window that is not 'SAK safe'
	 */
//#ifdef LINUX
	if ((fd = open("/dev/console", O_NOCTTY | O_WRONLY)) < 0)
		return;
//#else
//	fd = STDOUT_FILENO;
//#endif // LINUX

	write(fd, "[ueld] ", 7);
	write(fd, buff, strlen(buff));

//#ifdef LINUX
	close(fd);
//#endif // LINUX

	ueld_write_log(buff);

	va_end(ap);
}

void ueld_echo(char* msg)
{
	ueld_print("%s\n", msg);
}

static void setstdfd(char* pathname, int flags, char* exec_file)
{
	int fd0, fd1, fd2;

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	if ((fd0 = open(pathname, O_RDWR | flags)) < 0) {
		ueld_print("Could not run '%s': Open '%s' error (%s)\n", exec_file, pathname, strerror(errno));
		_exit(EXIT_FAILURE);
	}

	fd1 = dup(fd0);
	fd2 = dup(fd0);

	if((fd0 != STDIN_FILENO) || (fd1 != STDOUT_FILENO) || (fd2 != STDERR_FILENO)){
		ueld_print("Could not run '%s': Unexcept file discriptor number.\n", exec_file);
		_exit(EXIT_FAILURE);
	}
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
			setstdfd("/dev/null", 0, file);
		} else if (flag & URF_SETVT) {
			char devname[256];

			setsid();

			snprintf(devname, sizeof(devname), VT_TTY_NAME, vt);
			setstdfd(devname, 0, file);

#ifdef BSD
			if (ioctl(0, TIOCSCTTY, (char*)vt) < 0) {
				ueld_print("Could not run '%s': Set control tty error (%s)\n", file, strerror(errno));
				_exit(EXIT_FAILURE);
			}
#endif
		} else if (flag & URF_CONSOLE) {
			setstdfd("/dev/console", O_NOCTTY, file);
		}

		if (flag & URF_CMDLINE) {
			wordexp_t p;
			int ret;

			p.we_offs = 0;
			if ((ret = wordexp(file, &p, WRDE_DOOFFS)) != 0) {
				ueld_print("Could not run '%s': wordexp error %d\n", file, ret);
				_exit(EXIT_FAILURE);
			}

			execvp(p.we_wordv[0], p.we_wordv);

			wordfree(&p);
		} else {
			execl(file, file, NULL);
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
}

long ueld_readconfiglong(char* name, long defaultval)
{
	long i = defaultval;

	char* value = ueld_readconfig(name);
	if (value) {
		i = atol(value);
	}

	return i;
}

void ueld_closeconfig()
{
	if (config) {
		if (munmap(config, config_length) == 0)
			config = NULL;
	}
}
