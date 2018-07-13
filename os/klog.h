#ifndef __OS_KLOG_H_
#define __OS_KLOG_H_

int ueld_os_klog_open(char *target);
int ueld_os_klog_write(char *msg);
void ueld_os_klog_close();

#endif /* __OS_KLOG_H_ */
