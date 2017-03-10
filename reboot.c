








#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/mount.h>

#include "os/pw.h"
#include "readline.h"
#include "tools.h"

#define MOUNTS "/proc/self/mounts"

static void killproc(int signo){
	DIR* dir;
	struct dirent* dent;

	dir = opendir("/proc");
	dent = readdir(dir);	

	while (dent != NULL){
		pid_t pid = atol(dent->d_name);
		if (pid != 0 && pid != 1){
			kill(pid, signo);
		}
		dent = readdir(dir);
	}

	closedir(dir);
}

static int ueld_umount(char* info)
{
	int fd;
	char *src, *target, *type;

	src = info;
	target = strchr(src, ' ');
	*target++ = 0;
	type = strchr(target, ' ');
	*type++ = 0;
	
	printf("Trying to unmount %s\n", target);

	if ((fd = open(target, O_RDONLY)) >= 0) {
		syncfs(fd);
		close(fd);
	}

	if (strcmp(target, "/") != 0) {
		if (umount(target) < 0) {
			printf("Unmounting %s failed (%s), trying to remount it to read-only.\n", target, strerror(errno));
		} else {
			return 0;
		}
	}
	
	if (mount(src, target, type, MS_MGC_VAL|MS_RDONLY|MS_REMOUNT, NULL) < 0) {
		perror("mount");
		return -1;
	}
	
	return 0;
}

int ueld_reboot(int cmd)
{
	if (getpid() != 1) {
		ueld_echo("Must run as pid 1");
		return -1;
	}

	ueld_echo("Sending SIGTERM to all process...");
	killproc(SIGTERM);

	sleep(2);

	ueld_echo("Sending SIGKILL to all process...");
	killproc(SIGKILL);
	killproc(SIGKILL);

	ueld_echo("Running syshalt.sh...");
	ueld_run("/etc/ueld/syshalt.sh", URF_WAIT, 0, NULL);

	ueld_echo("Syncing disk...");
	sync();

	ueld_echo("Unmounting/Re-mounting file systems...");
	
	int fd;
	char s[256];
	if ((fd = open(MOUNTS, O_RDONLY)) < 0) {
		perror("open");
		return -1;
	}

	int hasumountfs = 0;

	while (readline(fd, s, sizeof(s)) > 0) {
		if (ueld_umount(s) < 0)
			hasumountfs = 1;
		//system("echo -n \"u\" > /proc/sysrq-trigger");
	}

	close(fd);

	if (hasumountfs && ueld_readconfiglong("ueld_must_remount_before_poweroff", -1) == 1) {
		ueld_echo("Remount some filesystems failed, droping to a shell...");
		char* sh = ueld_readconfig("system_shell");
		if (!sh) sh = "/bin/sh";
		ueld_run(sh, 0, 0, NULL);
		ueld_freeconfig(sh);
		return -1;
	}

	ueld_echo("Doing poweroff...");
	if (ueld_os_reboot(cmd) < 0)
		perror("reboot");
	exit(1);
}
