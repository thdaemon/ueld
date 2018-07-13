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
	if ((logfd = open(target, O_WRONLY|O_NOCTTY)) >= 0) {
		return 0;
	} else {
		return -1;
	}
}

int ueld_os_klog_write(char *msg)
{
	size_t len = strlen(msg);

	write(logfd, "<5>", 3);
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
#else
int ueld_os_klog_open(char *target)
{
	ueld_echo("WARNNING: klog feature not compiled.");
	return -1;
}

int ueld_os_klog_write(char *msg) { return 0; }
void ueld_os_klog_close() {}
#endif /* CONFIG_ENABLE_OS_KLOG */
