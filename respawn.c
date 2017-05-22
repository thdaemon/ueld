#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#include "tools.h"

#define RESPAWN_FILE "/etc/ueld/respawn.list"
#define MAX_APP 32

struct respawn_app {
	int vt;
	pid_t pid;
	char* cmd;
};

static struct respawn_app apps[MAX_APP];
static unsigned int appssz = 0;

static size_t length;
static char* respawn;

static void runapp(struct respawn_app* app)
{
	pid_t pid;

	if (app->vt) {
		pid = ueld_run(app->cmd, URF_SETVT|URF_CMDLINE, app->vt, NULL);
	} else {
		pid = ueld_run(app->cmd, URF_CMDLINE, 0, NULL);
	}

	if (pid < 0) pid = 0;
	app->pid = pid;
}

void respawn_init()
{
	int fd;
	char *p, *vt, *cmd;

	if ((fd = open(RESPAWN_FILE, O_RDONLY)) < 0)
		return;

	length = lseek(fd, 0, SEEK_END) + 1;
	respawn = mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);

	if (!respawn) {
		close(fd);
		return;
	}

	respawn[length - 1] = 0;
	close(fd);

	p = respawn;

	while (1) {
		cmd = strchr(p, ':');
		if (!cmd || *cmd == 0) break;
		*cmd = 0;
		cmd++;

		vt = p;

		p = strchr(cmd, '\n');
		if (!p || *p == 0) break;
		*p = 0;
		p++;

		apps[appssz].vt = atoi(vt);
		apps[appssz].pid = 0;
		apps[appssz].cmd = cmd;

		runapp(&apps[appssz++]);
	}

	if (!appssz)
		munmap(respawn, length);
}

void respawnpid(pid_t pid)
{
	for (int i = 0; i < appssz; i++) {
		if (apps[i].pid == pid) {
			runapp(&apps[i]);
			break;
		}
	}
}

void clearpid(pid_t pid)
{
	if (pid == 0) {
		munmap(respawn, length);
		appssz = 0;
	}

	for (int i = 0; i < appssz; i++) {
		if (apps[i].pid == pid) {
			apps[i].pid = 0;
			break;
		}
	}
}
