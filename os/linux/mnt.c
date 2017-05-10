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
#include <errno.h>

#include <sys/mount.h>
#ifndef CONFIG_MANU_GET_MNTINFO
#include <mntent.h>
#endif /* CONFIG_MANU_GET_MNTINFO */

#include "../../tools.h"
#include "../../fileio.h"

#include "../../config.h"

#define MOUNTS "/proc/self/mounts"
#define MOUNTS2 "/etc/mtab"


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
			           " the problem can not be fixup later.\n",
			           dir, strerror(errno));
			return -1;
		}
	}

	return 0;
}

#ifndef CONFIG_MANU_GET_MNTINFO
/*
 * When I get a mountent, and I umount it, then the '/etc/mtab'
 * and '/proc/self/mounts' will be changed, so in this situation,
 * can getmntent() work ok? The getmntent(3) manpage does not say
 * it, and I test it for some systems, it is no problem, but I
 * worry about it...
 *
 * getmntent() may use the stdio's buffer, so maybe problems will
 * not be decovered when the '/etc/mtab' file is small.
 *
 * So, I replace setmntent() by mysetmntent(), mysetmntent() read
 * all the mtab file to a buffer, so the situation will not appaer!
 *
 * And I left the CONFIG_MANU_GET_MNTINFO config (default not use) to
 * manu get the mntents, you can enable it in Linux system.
 */
static FILE* mysetmntent(const char* mtab, const char* type, int* pfd, char** pbuffer)
{
	int fd;
	char* buffer;
	ssize_t len;
	FILE* mnt;

	if ((fd = open(mtab, O_RDONLY)) < 0)
		return NULL;

	*pfd = fd;

	if ((len = readnsa(fd, &buffer)) < 0) {
		close(fd);
		return NULL;
	}

	*pbuffer = buffer;

	if ((mnt = fmemopen(buffer, len, "r")) == NULL) {
		free(buffer);
		close(fd);
	}

	return mnt;
}

static int myendmntent(FILE* mnt, int* pfd, char** pbuffer)
{
	fclose(mnt);
	free(*pbuffer);
	close(*pfd);
	return 1;
}

int ueld_os_umount_all()
{
	int fd;
	char* buffer;
	FILE* mnt;
	struct mntent* ent;
	unsigned int umount_fail_cnt;

	if ((mnt = mysetmntent(MOUNTS, "r", &fd, &buffer)) == NULL)
		mnt = mysetmntent(MOUNTS2, "r", &fd, &buffer);

	if (mnt == NULL)
		return -1;

	umount_fail_cnt = 0;
	while ((ent = getmntent(mnt)) != NULL) {
		if (_ueld_umount(ent->mnt_fsname, ent->mnt_dir, ent->mnt_type) < 0)
			umount_fail_cnt++;
	}

	myendmntent(mnt, &fd, &buffer);
	return umount_fail_cnt;
}
#else
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

int ueld_os_umount_all()
{
	int fd, umount_fail_cnt, n;
	off_t length;
	char* buffer;

	if ((fd = open(MOUNTS, O_RDONLY)) < 0) {
		ueld_print("open: %s\n", strerror(errno));
		return -1;
	}

	if ((length = readnsa(fd, &buffer)) < 0) {
		ueld_print("read: %s\n", strerror(errno));
		return -1;
	}

	if (realloc(buffer, length + 1) == 0) {
		ueld_print("realloc mem failed.\n");
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
#endif /* CONFIG_MANU_GET_MNTINFO */
