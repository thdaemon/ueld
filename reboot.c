#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

/* These codes will be moved to os/freebsd/mnt.c */
// 
//
//#ifdef BSD
//#include <sys/param.h>
//#endif /* BSD */

//#include <sys/mount.h>
//
//

#include <sys/wait.h>

#include "os/pw.h"
#include "os/chvt.h"
#include "os/proc.h"
#include "os/mnt.h"
#include "fileio.h"
#include "tools.h"
#include "respawn.h"

#include "config.h"

#ifndef CONFIG_TERM_WAITTIME
#define CONFIG_TERM_WAITTIME 2
#endif /* CONFIG_TERM_WAITTIME */

static void killproc(int signo)
{
	pid_t pid;

	if (ueld_os_for_each_process() < 0)
		return;

	while (ueld_os_next_process(&pid)) {
		if (pid != 1)
			kill(pid, signo);
	}

	ueld_os_end_each_process();
}

#ifndef CONFIG_CONSOLE_VT
#define CONFIG_CONSOLE_VT 1
#endif /* CONFIG_CONSOLE_VT */
int ueld_reboot(int cmd)
{
	int status, vt;
	pid_t pid;

	if (getpid() != 1) {
		ueld_echo("Must run as pid 1");
		return -1;
	}

	vt = (int)ueld_readconfiglong("ueld_console_vt", CONFIG_CONSOLE_VT);

	ueld_os_chvt(vt);

	clearpid(0);
	ueld_closeconfig();

	ueld_echo("Sending SIGTERM to all process...");
	killproc(SIGTERM);

	ueld_os_chvt(vt);
	sleep(CONFIG_TERM_WAITTIME);

	ueld_echo("Sending SIGKILL to all process...");
	killproc(SIGKILL);
	killproc(SIGKILL);

	ueld_os_chvt(vt);

	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {}

	ueld_echo("Running syshalt.sh...");
	ueld_run("/etc/ueld/syshalt.sh", URF_WAIT|URF_CONSOLE, 0, NULL);

	ueld_echo("Syncing disk...");
	sync();

	ueld_echo("Unmounting/Re-mounting file systems...");

	if ((ueld_os_umount_all() != 0) && (ueld_readconfiglong("ueld_must_remount_before_poweroff", -1) == 1)) {
		ueld_echo("Remount some filesystems failed, droping to a shell...");
		char* sh = ueld_readconfig("system_shell");
		if (!sh)
			sh = "/bin/sh";
		ueld_run(sh, URF_CONSOLE, 0, NULL);

		return -1;
	}

	ueld_echo("Doing poweroff...");
	if (ueld_os_reboot(cmd) < 0)
		ueld_print("reboot: %s", strerror(errno));
	exit(1);
}
