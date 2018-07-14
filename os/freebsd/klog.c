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
#endif /* CONFIG_ENABLE_OS_KLOG */
