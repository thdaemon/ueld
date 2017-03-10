#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include "readline.h"

#define MAXLINE 2048

static int read_cnt;
static char* read_ptr;
static char read_buf[MAXLINE];

static ssize_t my_read(int fd, char *ptr)
{
	if (read_cnt <= 0) {
again:
		if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (read_cnt == 0) {
			return(0);
		}
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ((rc = my_read(fd, &c)) >= 0) {
			if (c == '\n' || rc == 0) {
				*ptr = 0;
				return (n-1);
			} else {
				*ptr++ = c;
			}
		} else {
			return -1;
		}
	}

	*ptr = 0;
	return (n-1);
}
