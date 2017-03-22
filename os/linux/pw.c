#include <linux/reboot.h>
#include <sys/reboot.h>

#include "../pw.h"

int ueld_os_reboot(int cmd)
{
	int linux_cmd = 0;
	
	switch (cmd) {
	case UELD_OS_PW_POWER_OFF:
		linux_cmd = LINUX_REBOOT_CMD_POWER_OFF;
		break;
	case UELD_OS_PW_REBOOT:
		linux_cmd = LINUX_REBOOT_CMD_RESTART;
		break;
	default:
		break;
	}
	
	return reboot(linux_cmd);
}

/*
int ueld_os_reboot_with_cmd(char* cmd)
{
	return reboot(LINUX_REBOOT_MAGIC, LINUX_REBOOT_MAGIC2,
	        LINUX_REBOOT_CMD_RESTART2, cmd);
}
*/
