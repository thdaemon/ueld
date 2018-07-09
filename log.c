#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "log.h"
#include "tools.h"

static char block = 1;
static char proto_type;
static int logfd;

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

	if (strcmp(proto, "none") == 0) {
		proto_type = UELD_LOG_PROTO_NONE;
		return;
	} else if (strcmp(proto, "file") == 0) {
		proto_type = UELD_LOG_PROTO_FILE;

		logfd = open(target, O_CREAT|O_WRONLY|O_APPEND|O_NOCTTY);
		if (logfd > 0) {
			block = 0;
		} else {
			ueld_print("log: open '%s' failed (%s).", target, strerror(errno));
			return;
		}
	} else {
		ueld_echo("ueld_log_target proto unknown. Ignore all logs.");
		return;
	}
}

void ueld_log_close()
{
	switch (proto_type) {
	case UELD_LOG_PROTO_FILE:
		close(logfd);
		break;
	default:
		return;
		break;
	}

	block = 1;
}

/* FIXME: multiple proto write functions */
void ueld_write_log(char *msg)
{
	if (block)
		return;

	/* TODO: need current time */
	write(logfd, msg, strlen(msg));
}
