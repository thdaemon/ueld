#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "../../tools.h"

#include "../ctrlaltdel.h"

static CAD_handler phandler = NULL;

static void sig_int(int signo)
{
	phandler();
}

static int my_sysctl(char* path, char* value) {
	int fd;

	if ((fd = open(path, O_RDWR)) < 0)
		return -1;
	if (write(fd, value, strlen(value)) < 0)
		return -1;

	close(fd);
	return 0;
}

int ueld_os_handle_ctrl_alt_del(CAD_handler handler)
{
	phandler = handler;

	if (ueld_signal(SIGINT, sig_int, 1) < 0)
		return -1;

	if (my_sysctl("/proc/sys/kernel/ctrl-alt-del", "0") < 0)
		return -1;

	if (my_sysctl("/proc/sys/kernel/cad_pid", "1") < 0)
		return -1;

	return 0;
}
