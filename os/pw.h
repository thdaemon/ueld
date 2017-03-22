#ifndef __OS_PW_H_
#define __OS_PW_H_

#define UELD_OS_PW_POWER_OFF 1
#define UELD_OS_PW_REBOOT 2
int ueld_os_reboot(int cmd);
/*
int ueld_os_reboot_with_cmd(char* cmd);
*/

#endif // __OS_PW_H_
