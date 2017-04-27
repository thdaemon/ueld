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

#ifndef CONFIG_MANU_GET_MNTINFO
#include <mntent.h>
#endif /* CONFIG_MANU_GET_MNTINFO */

#include "os/pw.h"
#include "os/chvt.h"
#include "fileio.h"
#include "tools.h"
#include "restarts.h"

#include "config.h"

#ifndef CONFIG_TERM_WAITTIME
#define CONFIG_TERM_WAITTIME 2
#endif /* CONFIG_TERM_WAITTIME */

#define MOUNTS "/etc/mtab"

#ifdef LINUX
#define MOUNTS_LINUX "/proc/self/mounts"
#else
#define MOUNTS_LINUX MOUNTS
#endif /* LINUX */

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

static int _ueld_umount(char* fsname, char* dir, char* type)
{
	int fd;

	ueld_print("Trying to unmount %s\n", dir);

	if ((fd = open(dir, O_RDONLY)) >= 0) {
		syncfs(fd);
		close(fd);
	}

	if (umount(dir) == 0)
		return 0;

	if (mount(fsname, dir, type, MS_MGC_VAL|MS_RDONLY|MS_REMOUNT, NULL) < 0) {
		/* EINVAL means that the filesystem is not mounted. */
		if (errno != EINVAL) {
			ueld_print("Re-mount %s failed (%s), system will"
			           " not poweroff or poweroff unsafely if"
			           " the it can not be fixup later.\n",
			           dir, strerror(errno));
			return -1;
		}
	}

	return 0;
}

#ifdef CONFIG_MANU_GET_MNTINFO
#ifndef LINUX
#error "CONFIG_MANU_GET_MNTINFO only support Linux platform."
#endif /* LINUX */
static int ueld_umount(char* info)
{
	char *src, *target, *type;

	src = info;
	target = strchr(src, ' ');
	*target++ = 0;
	type = strchr(target, ' ');
	*type++ = 0;

	return _ueld_umount(src, target, type);
}

static char* readline(char* buffer, char** next)
{
	char* tmp;

	tmp = strchrnul(buffer, '\n');

	if (*tmp == 0)
		return 0;

	*tmp = 0;
	*next = tmp + 1;

	return buffer;
}

static int _umount_all(char* mntinfo, size_t length)
{
	int umount_fail_cnt;
	char *mnts, *p, *next;

	if ((mnts = malloc(length)) == NULL)
		return -1;

	memcpy(mnts, mntinfo, length);

	umount_fail_cnt = 0;
	next = mnts;
	while ((p = readline(next, &next)) != 0) {
		if (ueld_umount(p) < 0)
			umount_fail_cnt++;
	}

	free(mnts);
	return umount_fail_cnt;
}

static int umount_all()
{
	int fd, umount_fail_cnt, n;
	off_t length;
	char* buffer;

	if ((fd = open(MOUNTS_LINUX, O_RDONLY)) < 0) {
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

	length++;

	n = 0;
	if ((umount_fail_cnt = _umount_all(buffer, length)) > 0)
		n = umount_fail_cnt;

	for (int i = 0; i < n; i++) {
		if ((umount_fail_cnt = _umount_all(buffer, length)) == 0)
			break;
	}

	free(buffer);

	return umount_fail_cnt;
}
#else
/
 * TODO: getmntent() may cause a problem, but I have not met it. And
 *       getmntent() is portable, so I use it.
 *       When I get a mountent, and I umount it, then the '/etc/mtab'
 *       and '/proc/self/mounts' will be changed, so in this situation,
 *       can getmntent() work ok? The getmntent(3) manpage does not say
 *       it, and I test it for some systems, it is no problem, but I
 *       worry about it...
 *       getmntent() may use the stdio's buffer, so maybe problems will
 *       not be decovered when the '/etc/mtab' file is small.
 *       So, I left the CONFIG_MANU_GET_MNTINFO config, you can enable it
 *       in Linux system.
 *       Can you help me to get the answer to the question, can I use the
 *       function safely?
 */
static int umount_all()
{
	FILE* mnt;
	struct mntent* ent;
	unsigned int umount_fail_cnt;

	if ((mnt = setmntent(MOUNTS_LINUX, "r")) == NULL)
		mnt = setmntent(MOUNTS, "r");

	if (mnt == NULL)
		return -1;

	umount_fail_cnt = 0;
	while ((ent = getmntent(mnt)) != NULL) {
		if (_ueld_umount(ent->mnt_fsname, ent->mnt_dir, ent->mnt_type) < 0)
			umount_fail_cnt++;
	}

	endmntent(mnt);
	return umount_fail_cnt;
}
#endif /* CONFIG_MANU_GET_MNTINFO */

#ifndef CONFIG_CONSOLE_VT
#define CONFIG_CONSOLE_VT 1
#endif /* CONFIG_CONSOLE_VT */
void ueld_chvt()
{
	int vt = (int)ueld_readconfiglong("ueld_console_vt", CONFIG_CONSOLE_VT);
	if (vt)
		ueld_os_chvt(vt);
}

int ueld_reboot(int cmd)
{
	int status;
	pid_t pid;

	if (getpid() != 1) {
		ueld_echo("Must run as pid 1");
		return -1;
	}

	ueld_chvt();

	clearpid(0);
	ueld_closeconfig();

	ueld_echo("Sending SIGTERM to all process...");
	killproc(SIGTERM);

	ueld_os_chvt(1);
	sleep(CONFIG_TERM_WAITTIME);

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

	if ((umount_all() != 0) && (ueld_readconfiglong("ueld_must_remount_before_poweroff", -1) == 1)) {
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
