#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../proc.h"

struct kinfo_proc *kp = NULL;
int now, count = 0;

int ueld_os_for_each_process()
{
	int len, mib[4];

	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_ALL;
	mib[3] = 0;

	if (sysctl(mib, 3, NULL, &len, NULL, 0) == -1)
		return -1;

	len += sizeof(struct kinfo_proc) * 10;
	if ((kp = malloc(len)) == NULL) {
		errno = ENOMEM;
		return -1
	}

	if (sysctl(mib, 3, kp, &len, NULL, 0) == -1) {
		free(kp)
		return -1;
	}

	count = len / sizeof(struct kinfo_proc);

	return 0;
}

int ueld_os_next_process(pid_t* pid)
{
	if (now >= count)
		return 0;

	*pid = kp[now].kp_proc.p_pid;
	now++;
	return 1;
}

int ueld_os_end_each_process()
{
	if (kp)
		free(kp);

	return 0;
}
