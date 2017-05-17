#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/param.h>
#include <sys/mount.h>

static int _ueld_umount(struct statfs* fs)
{
	char *dir, *type;

	dir = fs->f_mntonname;

	ueld_print("Trying to unmount %s\n", dir);

	if (unmount(dir, 0) == 0)
		return 0;

	type = fs->f_fstypename;

	if (mount(type, dir, MNT_RDONLY | MNT_UPDATE, NULL) < 0) {
		ueld_print("Re-mount %s failed (%s), system will"
		           " not poweroff or poweroff unsafely if"
		           " the problem can not be fixup later.\n",
		           dir, strerror(errno));
		return -1;
	}

	return 0;
}

int ueld_os_umount_all()
{
	int count;
	struct statfs* fs;
	unsigned int umount_fail_cnt;

	umount_fail_cnt = 0;

	if ((count = getfsstat(NULL, 0, MNT_NOWAIT)) < 0)
		return -1;

	if ((fs = malloc(count * sizeof(struct statfs))) == NULL) {
		errno = ENOMEM;
		return -1;
	}

	if ((getfsstat(fs, count * sizeof(struct statfs), MNT_NOWAIT)) < 0)
		return -1;

	for (int i = 0; i < count; i++) {
		if (_ueld_umount(&fs[i]) < 0)
			umount_fail_cnt++;
	}

	free(fs);

	return umount_fail_cnt;
}
