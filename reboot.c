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
#include <sys/wait.h>

#include "os/pw.h"
#include "os/chvt.h"
#include "fileio.h"
#include "tools.h"
#include "restarts.h"

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

	ueld_print("Trying to unmount %s\n", target);

	if ((fd = open(target, O_RDONLY)) >= 0) {
		syncfs(fd);
		close(fd);
	}

	if (strcmp(target, "/") != 0) {
		if (umount(target) < 0) {
			ueld_print("Unmounting %s failed (%s), trying to remount it to read-only.\n", target, strerror(errno));
		} else {
			return 0;
		}
	}

	if (mount(src, target, type, MS_MGC_VAL|MS_RDONLY|MS_REMOUNT, NULL) < 0) {
		ueld_print("Error: Re-mount %s failed (%s), system will not poweroff or poweroff unsafely.", target, strerror(errno));
		return -1;
	}

	return 0;
}

char* readline(char* buffer, char** next)
{
	char* tmp;

	tmp = strchrnul(buffer, '\n');

	if (*tmp == 0)
		return 0;

	*tmp = 0;
	*next = tmp + 1;

	return buffer;
}

static int _umount_all(char* mntinfo)
{
	int umount_fail_cnt;
	char *p, *next;

	umount_fail_cnt = 0;
	next = mntinfo;
	while ((p = readline(next, &next)) != 0) {
		if (ueld_umount(p) < 0)
			umount_fail_cnt++;
	}

	return umount_fail_cnt;
}

static int umount_all(char* mntinfo)
{
	static int n = 0;

	int umount_fail_cnt;

	umount_fail_cnt = _umount_all(mntinfo);
	if (umount_fail_cnt)
		n = umount_fail_cnt;

	for (int i = 0; i < n; i++) {
		if ((umount_fail_cnt = _umount_all(mntinfo)) == 0)
			break;
	}

	return umount_fail_cnt;
}

int ueld_reboot(int cmd)
{
	int fd, status;
	off_t length;
	pid_t pid;
	char* buffer;

	if (getpid() != 1) {
		ueld_echo("Must run as pid 1");
		return -1;
	}

	ueld_os_chvt(1);

	clearpid(0);
	ueld_closeconfig();

	ueld_echo("Sending SIGTERM to all process...");
	killproc(SIGTERM);

	ueld_os_chvt(1);
	sleep(2);

	ueld_echo("Sending SIGKILL to all process...");
	killproc(SIGKILL);
	killproc(SIGKILL);

	ueld_os_chvt(1);

	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {}

	ueld_echo("Running syshalt.sh...");
	ueld_run("/etc/ueld/syshalt.sh", URF_WAIT|URF_CONSOLE, 0, NULL);

	ueld_echo("Syncing disk...");
	sync();

	ueld_echo("Unmounting/Re-mounting file systems...");

	if ((fd = open(MOUNTS, O_RDONLY)) < 0) {
		ueld_print("open: %s", strerror(errno));
		return -1;
	}

	if ((length = readnsa(fd, &buffer)) < 0) {
		ueld_print("read: %s", strerror(errno));
		return -1;
	}

	if (realloc(buffer, length + 1) == 0) {
		ueld_print("realloc mem failed.");
		return -1;
	}
	buffer[length] = 0;
	close(fd);

	if (umount_all(buffer) != 0 && ueld_readconfiglong("ueld_must_remount_before_poweroff", -1) == 1) {
		ueld_echo("Remount some filesystems failed, droping to a shell...");
		char* sh = ueld_readconfig("system_shell");
		if (!sh)
			sh = "/bin/sh";
		ueld_run(sh, URF_CONSOLE, 0, NULL);

		return -1;
	}

	free(buffer);

	ueld_echo("Doing poweroff...");
	if (ueld_os_reboot(cmd) < 0)
		ueld_print("reboot: %s", strerror(errno));
	exit(1);
}
