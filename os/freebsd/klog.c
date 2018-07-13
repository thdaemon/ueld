#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "../../tools.h"
#include "../../log.h"

#include "../klog.h"

#ifdef CONFIG_ENABLE_OS_KLOG
#error "ueld klog not currently support FreeBSD"
#else
int ueld_os_klog_open(char *target)
{
	ueld_echo("WARNNING: klog feature not compiled.");
	return -1;
}

int ueld_os_klog_write(char *msg) { return 0; }
void ueld_os_klog_close() {}
#endif /* CONFIG_ENABLE_OS_KLOG */
