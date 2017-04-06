#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/vt.h>

static int setconsole(int fd, int vt)
{
	char bytes[2] = {11, 0};

	bytes[1] = (char)vt;

	if (ioctl(fd, TIOCLINUX, bytes) < 0)
		return -1;

	return 0;
}

int ueld_os_chvt(int vt)
{
	int fd;

	if ((fd = open("/dev/console", O_RDONLY)) < 0)
		return -1;

	setconsole(fd, vt);
	close(fd);

	if ((fd = open("/dev/tty0", O_RDONLY)) < 0)
		return -1;

	if (ioctl(fd, VT_ACTIVATE, vt) < 0)
		return -1;

	if (ioctl(fd, VT_WAITACTIVE, vt) < 0)
		return -1;

	close(fd);

	return 0;
}
