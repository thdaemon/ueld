#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
	ueld_run("/etc/ueld/ctrlaltdel.sh", URF_WAIT, 0, NULL);
}

int ueld_main(int argc, char* argv[])
{
	int status;
	pid_t pid;

	if (getpid() != 1) {
		exit(0);
	}
	
	ueld_unblock_signal(SIGUSR1);

	if (ueld_os_handle_ctrl_alt_del(cad_handler) < 0)
		ueld_print("Warning: Could not handle ctrl-alt-del, %s", strerror(errno));

	ueld_signal(SIGTERM, &sig_term, 1);
	ueld_signal(SIGHUP, &sig_hup, 1);
	ueld_signal(SIGUSR1, &sig_user1, 1);

	if (argc > 0 && (strcmp(argv[0], "-ueld") != 0)) {
		ueld_run("/etc/ueld/sysinit.sh", URF_WAIT, 0, NULL);
		restarts_init();
		ueld_run("/etc/ueld/sysloaded.sh", URF_NOOUTPUT, 0, NULL);
	}

#ifdef LINUX
	prctl(PR_SET_NAME, "ueld", 0, 0);
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
