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
#define HASHTABLE_FIRST_NODE_SIZE 32

struct respawn_node {
	int nref;
	int vt;
	pid_t pid;
	char *cmd;
	struct respawn_node *prev;
	struct respawn_node *next;
};

static struct respawn_node *first_nodes[HASHTABLE_FIRST_NODE_SIZE];
static unsigned int disable = 0;

static size_t length;
static char *respawn;

#define hashpid(pid)	((pid) % HASHTABLE_FIRST_NODE_SIZE)
#define nodefree(node)	(free(node))
#define noderef(node)	(((node)->nref)++)

static struct respawn_node *nodenew()
{
	static struct respawn_node *node;

	if ((node = malloc(sizeof(struct respawn_node))) == NULL) {
		return NULL;
	}

	node->nref = 1;
	return node;
}

static void nodeunref(struct respawn_node *node)
{
	node->nref = node->nref - 1;
	if (node->nref <= 0) {
		nodefree(node);
	}
}

static void insert(pid_t key, struct respawn_node *node)
{
	int index = hashpid(key);

	noderef(node);
	node->prev = NULL;
	node->next = NULL;

	if (first_nodes[index] == NULL) {
		first_nodes[index] = node;
	} else {
		struct respawn_node *prenode = first_nodes[index];
		while (prenode->next != NULL) {
			prenode = prenode->next;
		}
		prenode->next = node;
		node->prev = prenode;
	}
}

static struct respawn_node *get(pid_t key)
{
	int index = hashpid(key);

	struct respawn_node *node = first_nodes[index];
	while (node != NULL) {
		if (node->pid == key) {
			return node;
		}

		node = node->next;
	}

	return NULL;
}

#define delete_node_without_index(node)	delete_node((node), -1)

static void delete_node(struct respawn_node *node, int index)
{
	struct respawn_node *prenode = node->prev;

	if (prenode) {
		prenode->next = node->next;
	} else {
		if (index == -1)
			index = hashpid(node->pid);
		first_nodes[index] = node->next;
	}

	if (node->next) {
		node->next->prev = prenode;
	}

	nodeunref(node);
}

static void delete(pid_t key)
{
	int index = hashpid(key);

	struct respawn_node *node = first_nodes[index];
	while (node != NULL) {
		if (node->pid == key) {
			delete_node(node, index);
		}

		node = node->next;
	}
}

static pid_t runapp(struct respawn_node *app)
{
	pid_t pid;

	if (app->vt) {
		pid = ueld_run(app->cmd, URF_SETVT|URF_CMDLINE, app->vt, NULL);
	} else {
		pid = ueld_run(app->cmd, URF_CMDLINE, 0, NULL);
	}

	if (pid < 0)
		pid = 0;

	return pid;
}

void respawn_init()
{
	int fd;
	int appssz = 0;
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

		struct respawn_node *node;
		if ((node = nodenew()) != NULL) {
			node->vt = atoi(vt);
			node->cmd = cmd;

			pid_t pid = runapp(node);

			node->pid = pid;

			if (unlikely(pid == 0)) {
				ueld_print("Run respwan process '%s' first time failed\n", cmd);
			} else {
				appssz++;
				insert(pid, node);
			}

			nodeunref(node);
		} else {
			ueld_print("Create respwan node for '%s' failed\n", cmd);
		}
	}

	if (!appssz)
		munmap(respawn, length);
}

void respawnpid(pid_t pid)
{
	if (disable) {
		return;
	}

	struct respawn_node *node;
	if ((node = get(pid)) != NULL) {
		pid_t newpid = runapp(node);

		if (unlikely(newpid == 0)) {
			ueld_print("Respwan process '%s' failed, remove it from list...\n", node->cmd);
			delete_node_without_index(node);
		} else {
			noderef(node);
			delete_node_without_index(node);
			node->pid = newpid;
			insert(newpid, node);
			nodeunref(node);
		}
	}
}

void clearpid(pid_t pid)
{
	if (pid != 0) {
		delete(pid);
	} else {
		disable = 1;
		for (int i = 0; i < HASHTABLE_FIRST_NODE_SIZE; i++) {
			if (first_nodes[i] != NULL) {
				struct respawn_node *node = first_nodes[i];
				while (node->next != NULL) {
					struct respawn_node *tmp = node;
					node = node->next;
					nodeunref(tmp);
				}
				nodeunref(node);
				first_nodes[i] = NULL;
			}
		}
		munmap(respawn, length);
	}
}
