#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "../../config.h"

#include "../../tools.h"
#include "../../log.h"

#include "../klog.h"

#ifdef CONFIG_ENABLE_OS_KLOG
int ueld_os_klog_open(char *target)
{
	if ((logfd = open("/dev/kmsg", O_WRONLY|O_NOCTTY)) >= 0) {
		return 0;
	} else {
		return -1;
	}
}

int ueld_os_klog_write(char *msg)
{
	size_t len = strlen(msg);

	write(logfd, "<5>ueld: ", 9);
	write(logfd, msg, len);

	if (unlikely(msg[len-1] != '\n')) {
		write(logfd, "\n", 1);
	}

	return 0;
}

void ueld_os_klog_close()
{
	close(logfd);
}
#endif /* CONFIG_ENABLE_OS_KLOG */