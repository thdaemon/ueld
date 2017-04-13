#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#ifdef LINUX
#include <sys/prctl.h>
#endif // LINUX

#include "reboot.h"
#include "os/pw.h"
#include "os/ctrlaltdel.h"
#include "tools.h"
#include "restarts.h"

static void sig_term(int signo)
{
	ueld_reboot(UELD_OS_PW_POWER_OFF);
}

static void sig_hup(int signo)
{
	ueld_reboot(UELD_OS_PW_REBOOT);
}

static void sig_user1(int signo)
{
	ueld_closeconfig();
	char* path = ueld_readconfig("ueld_execfile");
	if (!path)
		path = "/sbin/init";

	ueld_print("Trying to reload init '%s'\n", path);

	if (path)
		execl(path, "-ueld", 0);

	ueld_print("Reload init '%s' failed.\n", path ? path : "Unknown");
}

/* this function will be called when press ctrl-alt-del on a vt. */
static void cad_handler()
{
	ueld_run("/etc/ueld/ctrlaltdel.sh", URF_WAIT|URF_CONSOLE, 0, NULL);
}

static void setevar()
{
	char* path = ueld_readconfig("ueld_path_var");
	if (!path) return;

	int override = ueld_readconfiglong("ueld_override_path_var", 0);

	if (getenv("PATH") == NULL || override)
		setenv("PATH", path, 1);
}

int ueld_main(int argc, char* argv[])
{
	int status;
	pid_t pid;

	if (getpid() != 1) {
		exit(0);
	}

#ifdef LINUX
	prctl(PR_SET_NAME, "ueld", 0, 0);
#endif // LINUX

	ueld_unblock_signal(SIGUSR1);

	setevar();

	ueld_signal(SIGTERM, &sig_term, 1);
	ueld_signal(SIGHUP, &sig_hup, 1);
	ueld_signal(SIGUSR1, &sig_user1, 1);

	if (argc > 0 && (strcmp(argv[0], "-ueld") != 0)) {
		ueld_run("/etc/ueld/sysinit.sh", URF_WAIT, 0, NULL);
		restarts_init();
		ueld_run("/etc/ueld/sysloaded.sh", URF_NOOUTPUT, 0, NULL);
	}

	if (ueld_os_handle_ctrl_alt_del(cad_handler) < 0)
		ueld_print("Warning: Could not handle ctrl-alt-del, %s\n", strerror(errno));

#ifdef LINUX
	/*
	 * FIXME: Linux Sysrq SAK Problem
	 * My system is linux 3.4.39
	 *
	 * There is a problem, init daemon start with fd0-2 to /dev/console.
	 * But /dev/console often is as /dev/tty1, so when user press the SAK
	 * keys of Sysrq (Alt-Sysrq-K) in vt1(tty1), if the kernel is configed
	 * to support the function, kernel will send SIGKILL to init daemon, it
	 * will cause kernel panic!
	 *
	 * Location: path-to-linux-src/drivers/tty/tty_io.c (__do_SAK() function)
	 *
	 * My methed is close fd0-2, so SAK won't kill init. But when ueld need
	 * print somethings to system console, ueld must open the '/dev/console'
	 * although we can close it after write(), etc., but it make a window
	 * which is not 'SAK safe'. I can't find a good methed to avoid it.
	 */
	close(0);
	close(1);
	close(2);

	int fd;

	if ((fd = open("/dev/null", O_RDWR)) == 0) {
		dup2(fd, 1);
		dup2(fd, 2);
	} else if (fd > 0) {
		close(fd);
	}
#endif // LINUX

	ueld_closeconfig();

	while (1) {
		if((pid = wait(&status)) > 0) {
			if ((WIFEXITED(status)) && (WEXITSTATUS(status) == EXIT_FAILURE)) {
				clearpid(pid);
				continue;
			}
			restartpid(pid);
		}
	}

	ueld_reboot(UELD_OS_PW_POWER_OFF);
	exit(0);
}
