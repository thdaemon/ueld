#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "fileio.h"

/*
 * The function name 'nsa' means 'None Seekable'.
 * But it can be used for seekable file too. :-)
 * Why did I use the name? Because the function read the
 * whole file to a buffer which alloced at runtime
 * so that it was usually used for noneseekable files.
 */
ssize_t readnsa(int fd, char** ptr)
{
	ssize_t nread, n;
	char* buffer;

	buffer = 0;
	nread = 0;
	n = 0;

	while (1) {
		buffer = realloc(buffer, nread + 1024);
		if (!buffer) {
			errno = ENOMEM;
			return -1;
		}

		if ((n = read(fd, buffer + nread, 1024)) < 0) {
			free(buffer);
			return -1;
		} else if (n == 0) {
			break;		/* EOF */
		}

		nread += n;
		*ptr = buffer;
	}
	return nread;
}

/*
 * When I get a mountent, and I umount it, then the '/etc/mtab'
 * and '/proc/self/mounts' will be changed, so in this situation,
 * can getmntent() work ok? The getmntent(3) manpage does not say
 * it, and I test it for some systems, it is no problem, but I
 * worry about it...
 *
 * getmntent() may use the stdio's buffer, so maybe problems will
 * not be decovered when the '/etc/mtab' file is small.
 *
 * So, I replace setmntent() by mysetmntent(), mysetmntent() read
 * all the mtab file to a buffer, so the situation will not appaer!
 *
 * And I left the CONFIG_MANU_GET_MNTINFO config (default not use) to
 * manu get the mntents, you can enable it in Linux system.
 */
FILE* mysetmntent(const char* mtab, const char* type, int* pfd, char** pbuffer)
{
	int fd;
	char* buffer;
	ssize_t len;
	FILE* mnt;

	if ((fd = open(mtab, O_RDONLY)) < 0)
		return NULL;

	*pfd = fd;

	if ((len = readnsa(fd, &buffer)) < 0) {
		close(fd);
		return NULL;
	}

	*pbuffer = buffer;

	if ((mnt = fmemopen(buffer, len, "r")) == NULL) {
		free(buffer);
		close(fd);
	}

	return mnt;
}

int myendmntent(FILE* mnt, int* pfd, char** pbuffer)
{
	fclose(mnt);
	free(*pbuffer);
	close(*pfd);
	return 1;
}
