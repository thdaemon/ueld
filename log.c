#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>

#include "config.h"

#include "log.h"
#include "tools.h"
#include "os/klog.h"

#ifdef CONFIG_ENABLE_LOG
static char block = 1;
static char proto_type;
int logfd;
#endif

static void ueld_log_write_discard(char *msg) {}
ueld_write_log_t ueld_write_log = ueld_log_write_discard;

#ifdef CONFIG_ENABLE_LOG_FILE
static void ueld_log_write_file(char *msg)
{
	if (block)
		return;

	/* TODO: need current time */
	write(logfd, msg, strlen(msg));
}

static void ueld_log_open_file(char *target)
{
	proto_type = UELD_LOG_PROTO_FILE;

	logfd = open(target, O_CREAT|O_WRONLY|O_APPEND|O_NOCTTY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
	if (logfd >= 0) {
		ueld_write_log = ueld_log_write_file;
		block = 0;
	} else {
		ueld_print("log: open '%s' failed (%s).\n", target, strerror(errno));
	}
}

static void ueld_log_close_file()
{
	close(logfd);
}
#endif

#ifdef CONFIG_ENABLE_OS_KLOG
static void ueld_log_write_klog(char *msg)
{
	if (block)
		return;

	ueld_os_klog_write(msg);
}

static void ueld_log_open_klog(char *target)
{
	proto_type = UELD_LOG_PROTO_KLOG;
	if (ueld_os_klog_open(target) == 0) {
		ueld_write_log = ueld_log_write_klog;
		block = 0;
	} else {
		ueld_print("log: open '%s' failed (%s).\n", target, strerror(errno));
	}
}
#endif

#ifdef CONFIG_ENABLE_LOG_SYSLOG
static void ueld_log_write_syslog(char *msg)
{
	if (block)
		return;

	syslog(LOG_INFO, "%s", msg);
}

static void ueld_log_open_syslog(char *target)
{
	proto_type = UELD_LOG_PROTO_SYSLOG;
	openlog("ueld", 0, LOG_DAEMON);
	ueld_write_log = ueld_log_write_syslog;
	block = 0;
}

static void ueld_log_close_syslog()
{
	closelog();
}
#endif

#ifdef CONFIG_ENABLE_LOG
static void ueld_log_open_none(char *target)
{
	proto_type = UELD_LOG_PROTO_NONE;
}

struct {
	char *name;
	int type;
	ueld_write_open_t f_open;
	ueld_write_close_t f_close;
} ueld_log_protos[] = {
	{ "none", UELD_LOG_PROTO_NONE, ueld_log_open_none, NULL },
#ifdef CONFIG_ENABLE_LOG_FILE
	{ "file", UELD_LOG_PROTO_FILE, ueld_log_open_file, ueld_log_close_file },
#endif
#ifdef CONFIG_ENABLE_OS_KLOG
	{ "klog", UELD_LOG_PROTO_KLOG, ueld_log_open_klog, ueld_os_klog_close },
#endif
#ifdef CONFIG_ENABLE_LOG_SYSLOG
	{ "syslog", UELD_LOG_PROTO_SYSLOG, ueld_log_open_syslog, ueld_log_close_syslog },
#endif
	{ NULL, 0, NULL, NULL }
};

void ueld_log_init()
{
	char *target = ueld_readconfig("ueld_log_target");
	if (!target || *target == 0) {
		ueld_echo("ueld_log_target empty. Ignore all logs.");
		return;
	}

	char *proto = target;
	target = strchr(target, ':');
	if (!target || *target == '\0') {
		ueld_echo("ueld_log_target proto unknown. Ignore all logs.");
		return;
	}

	*target = '\0';
	target++;

	if (target[0] == '\0') {
		ueld_echo("ueld_log_target target name empty. Ignore all logs.");
		return;
	}

	int i = 0;
	while (ueld_log_protos[i].name != NULL) {
		if (strcmp(proto, ueld_log_protos[i].name) == 0) {
			ueld_log_protos[i].f_open(target);
			i = -1;
			break;
		}
		i++;
	}

	if (i != -1) {
		ueld_echo("ueld_log_target proto unknown. Ignore all logs.");
	}
}

void ueld_log_close()
{
	int i = 0;
	while (ueld_log_protos[i].name != NULL) {
		if (ueld_log_protos[i].type == proto_type) {
			block = 1;
			ueld_log_protos[i].f_close();
			break;
		}
		i++;
	}
}
#else
void ueld_log_init()
{
	ueld_echo("log feature not compiled.");
}

void ueld_log_close() {}
#endif
