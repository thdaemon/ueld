#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include "fileio.h"

#ifdef CONFIG_MANU_GET_MNTINFO
/* the function name 'nsa' means 'None Seekable' */
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
#endif /* CONFIG_MANU_GET_MNTINFO */
