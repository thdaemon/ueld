#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../proc.h"

DIR* dir;

int ueld_os_for_each_process()
{
	if ((dir = opendir("/proc")) == NULL)
		return -1;

	return 0;
}

int ueld_os_next_process(pid_t* pid)
{
	struct dirent* dent;

reread:
	if ((dent = readdir(dir)) == NULL)
		return 0;

	pid_t p = atol(dent->d_name);
	if (p != 0) {
		*pid = p;
	} else {
		goto reread;
	}

	return 1;
}

int ueld_os_end_each_process()
{
	return closedir(dir);
}
