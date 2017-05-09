#include <unistd.h>
#include <sys/reboot.h>

#include "../pw.h"

int ueld_os_reboot(int cmd)
{
	int howto = 0;
	
	switch (cmd) {
	case UELD_OS_PW_POWER_OFF:
		howto = RB_POWEROFF;
		break;
	case UELD_OS_PW_REBOOT:
		howto = RB_AUTOBOOT;
		break;
	default:
		break;
	}
	
	return reboot(howto);
}
