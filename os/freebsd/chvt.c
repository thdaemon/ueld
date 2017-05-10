#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 * TODO: Change active tty on freebsd.
 */
int ueld_os_chvt(int vt)
{
	return 0;
}
